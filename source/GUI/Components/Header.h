//
// Created by Landon Viator on 11/24/25.
//

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../Utils/Colors.h"
#include "../Utils/Images.h"
#include "../../Engine/Globals/Globals.h"
#include "../LookAndFeel/MenuLAF.h"
#include "../LookAndFeel/TextButtonLAF.h"

namespace viator::gui::components
{
    class Header : public juce::Component, public juce::ActionBroadcaster
    {
    public:
        Header(juce::AudioProcessorValueTreeState&);

        ~Header() override;

        void paint(juce::Graphics &) override;

        void resized() override;

        juce::TextButton& getPresetAButton() {return m_preset_a_button; }
        juce::TextButton& getPresetBButton() {return m_preset_b_button; }
    private:
        juce::AudioProcessorValueTreeState& m_tree_state;
        juce::ComboBox m_oversampling_menu, m_preset_browser, m_midside_menu;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> m_oversampling_attach,
        m_mid_side_attach;
        viator::gui::laf::MenuLAF m_menu_laf;
        void setComboBoxProps(juce::ComboBox &box, const juce::StringArray &items);

        viator::gui::laf::TextButtonLAF m_text_button_laf;
        juce::TextButton m_preset_a_button, m_preset_b_button;

        void setTextButtonProps(juce::TextButton &button, const juce::String &text);

        std::unique_ptr<juce::DropShadower> m_drop_shadow;

        std::unique_ptr<juce::DrawableButton> m_power_button;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> m_power_attach;
        std::unique_ptr<juce::DrawableButton> m_settings_button;
        void setSVGButtonProps(std::unique_ptr<juce::DrawableButton> &button,
                               const juce::Drawable *offImage,
                               const juce::Drawable *onImage,
                               const juce::String &name);
    };
}
