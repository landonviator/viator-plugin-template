#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p), m_header(processorRef.getTreeState())
{
    juce::ignoreUnused(processorRef);

    addAndMakeVisible(m_header);
    addAndMakeVisible(m_info_panel);

    m_info_panel.setVisible(false);
    m_header.addActionListener(this);

    initSliders();

    initMeters();
    startTimerHz(30.0f);

    setSize(1230, 730);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    m_header.removeActionListener(this);

    for (auto &slider: m_sliders) {
        slider.setLookAndFeel(nullptr);
    }

    stopTimer();
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics &g)
{
    auto bg = viator::gui_utils::Colors::editor_bg_color();
    g.setGradientFill(juce::ColourGradient::vertical(bg.darker(0.25f),
                                                     static_cast<float>(getHeight()), bg,
                                                     static_cast<float>(getHeight()) * 0.4f));
    g.fillRect(getLocalBounds());

    // FOOTER
    bg = viator::gui_utils::Colors::header_bg_color();
    g.setColour(bg);
    g.fillRect(getLocalBounds().withY(juce::roundToInt(getHeight() * 0.9)).withHeight(getHeight() / 10));

    // HOVER
    for (int i = 0; i < m_sliders.size(); ++i) {
        const auto is_over = m_sliders[i].isMouseOverOrDragging();
        const auto value = m_sliders[i].getValue();
        const auto name = m_sliders[i].getName();
        m_slider_popup_labels[i].setText(is_over ? juce::String(value, 2) : name, juce::dontSendNotification);
    }

    g.drawImage(viator::gui_utils::Images::texture(), getLocalBounds().toFloat().withSizeKeepingCentre
    (static_cast<float>(getWidth()) * 0.95f, static_cast<float>(getHeight()) * 0.75f), juce::RectanglePlacement::stretchToFit);

    g.setColour(juce::Colour(52, 76, 100).withAlpha(0.8f));
    g.fillRect(getLocalBounds().toFloat().withSizeKeepingCentre
    (static_cast<float>(getWidth()) * 0.95f, static_cast<float>(getHeight()) * 0.75f));
}

void AudioPluginAudioProcessorEditor::resized()
{
    m_header.setBounds(0, 0, getWidth(), getHeight() / 10);
    m_info_panel.setBounds(getLocalBounds().withSizeKeepingCentre(
        juce::roundToInt(getWidth() * 0.9),
        juce::roundToInt(getHeight() * 0.7)));

    const auto dial_size = juce::roundToInt(getHeight() * 0.09);
    const auto dial_y = getHeight() - juce::roundToInt(dial_size * 1.025);
    m_sliders[0].setBounds(dial_size, dial_y, dial_size, dial_size);
    m_sliders[1].setBounds(getWidth() - dial_size * 2, dial_y, dial_size, dial_size);

    m_input_meter.setBounds(m_sliders[0].getRight() + dial_size / 4, dial_y, dial_size / 5,
                            juce::roundToInt(dial_size * 0.95));
    m_output_meter.setBounds(m_sliders[1].getX() - dial_size / 2, dial_y, dial_size / 5,
                             juce::roundToInt(dial_size * 0.95));

    m_slider_popup_labels[0].setBounds(0, dial_y, dial_size, dial_size);
    m_slider_popup_labels[1].setBounds(getWidth() - dial_size, dial_y, dial_size, dial_size);

    const auto font_size = juce::jmax(static_cast<float>(getHeight()) * 0.02f, 10.0f);
    m_slider_popup_labels[0].setFont(viator::gui_utils::Fonts::regular(font_size));
    m_slider_popup_labels[1].setFont(viator::gui_utils::Fonts::regular(font_size));
}

void AudioPluginAudioProcessorEditor::initSliders()
{
    for (auto &slider: m_sliders) {
        slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setColour(juce::Slider::ColourIds::thumbColourId, viator::gui_utils::Colors::text());
        slider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colour(234, 234, 234));
        slider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId,
        viator::gui_utils::Colors::widget_center().brighter(0.15f));
        slider.setLookAndFeel(&m_thin_dial_laf);
        addAndMakeVisible(slider);
    }

    m_sliders[0].setComponentID(viator::globals::PluginParameters::gainID);
    m_sliders[1].setComponentID(viator::globals::PluginParameters::outputID);

    m_sliders[0].setName(viator::globals::PluginParameters::gainName);
    m_sliders[1].setName(viator::globals::PluginParameters::outputName);

    m_gain_attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef
        .getTreeState(), viator::globals::PluginParameters::gainID, m_sliders[0]);
    m_volume_attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef
        .getTreeState(), viator::globals::PluginParameters::outputID, m_sliders[1]);

    m_sliders[0].onValueChange = [this]()
    {
        save_preset(m_sliders[0].getComponentID(), static_cast<float>(m_sliders[0].getValue()));
    };

    m_sliders[1].onValueChange = [this]()
    {
        save_preset(m_sliders[1].getComponentID(), static_cast<float>(m_sliders[1].getValue()));
    };

    for (auto &label: m_slider_popup_labels) {
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);
    }
}


void AudioPluginAudioProcessorEditor::actionListenerCallback(const juce::String &message)
{
    if (message == viator::globals::ActionMessages::infoToggled) {
        m_info_panel.setVisible(!m_info_panel.isVisible());
    } else if (message == viator::globals::ActionMessages::presetAToggled) {
        apply_preset(true);
    } else if (message == viator::globals::ActionMessages::presetBToggled) {
        apply_preset(false);
    }
}

void AudioPluginAudioProcessorEditor::apply_preset(const bool isPresetA) const
{
    const auto &preset_map = isPresetA ? processorRef.getPresetA() : processorRef.getPresetB();
    const auto &tree = processorRef.getTreeState();

    for (const auto &item: preset_map) {
        const auto &parameter = tree.getParameter(item.first);
        const auto normalizedValue = parameter->convertTo0to1(item.second);

        if (item.first == viator::globals::PluginParameters::oversamplingID) {
            //hq_menu.setSelectedId(juce::roundToInt(item.second));
        } else if (item.first == viator::globals::PluginParameters::midSideID) {
            //stereo_menu.setSelectedId(juce::roundToInt(item.second));
        } else {
            parameter->setValueNotifyingHost(normalizedValue);
        }
    }
}

void AudioPluginAudioProcessorEditor::save_preset(const juce::String &param, const float paramValue)
{
    auto &preset_map = m_header.getPresetAButton().getToggleState()
                           ? processorRef.getPresetA()
                           : processorRef
                           .getPresetB();
    preset_map[param] = paramValue;
}

void AudioPluginAudioProcessorEditor::initMeters()
{
    addAndMakeVisible(m_input_meter);
    addAndMakeVisible(m_output_meter);
}

void AudioPluginAudioProcessorEditor::timerCallback()
{
    m_input_meter.setLevel(processorRef.getInputLevel(0));
    m_output_meter.setLevel(processorRef.getOutputLevel(0));
}
