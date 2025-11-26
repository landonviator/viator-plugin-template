//
// Created by Landon Viator on 11/24/25.
//

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../Utils/Fonts.h"

namespace viator::gui::laf
{
    class MenuLAF final : public juce::LookAndFeel_V4
    {
    public:
        void drawComboBox(juce::Graphics &g, const int width, const int height, bool,
                          int, int, int, int, juce::ComboBox &box) override
        {
            constexpr auto cornerSize = 6.0f;
            const juce::Rectangle<int> boxBounds(0, 0, width, height);

            constexpr auto reduction = 3.0f;

            // SHADOW
            {
                constexpr float shadowOffset = 3.0f;
                constexpr float shadowSize   = 15.0f;

                const juce::Rectangle<float> shadowBounds = boxBounds.toFloat()
                                                            .translated(0.0f, shadowOffset)
                                                            .reduced(reduction, reduction);

                const juce::ColourGradient shadowGradient(
                    juce::Colours::black.withAlpha(0.25f),
                    shadowBounds.getCentreX(),
                    shadowBounds.getY(),
                    juce::Colours::black.withAlpha(0.0f),
                    shadowBounds.getCentreX(),
                    shadowBounds.getBottom() + shadowSize,
                    false
                );

                g.setGradientFill(shadowGradient);
                g.fillRoundedRectangle(shadowBounds, cornerSize);
            }

            g.setColour(box.findColour(juce::ComboBox::backgroundColourId));
            g.fillRoundedRectangle(boxBounds.toFloat().reduced(reduction, reduction), cornerSize);

            g.setColour(box.findColour(juce::ComboBox::outlineColourId));
            g.drawRoundedRectangle(boxBounds.toFloat().reduced(reduction, reduction), cornerSize, 2.0f);

            const juce::Rectangle<int> arrowZone(width - 25, 0, 20, height);
            juce::Path path;
            path.startNewSubPath(static_cast<float>(arrowZone.getX()) + 3.0f,
                                 static_cast<float>(arrowZone.getCentreY()) - 2.0f);
            path.lineTo(static_cast<float>(arrowZone.getCentreX()),
                        static_cast<float>(arrowZone.getCentreY()) + 3.0f);
            path.lineTo(static_cast<float>(arrowZone.getRight()) - 3.0f,
                        static_cast<float>(arrowZone.getCentreY()) - 2.0f);

            g.setColour(
                box.findColour(juce::ComboBox::arrowColourId).withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
            g.strokePath(path, juce::PathStrokeType(2.0f));
        }

        juce::Font getComboBoxFont(juce::ComboBox &box) override
        {
            const auto font_size = static_cast<float>(box.getHeight()) * 0.5f;
            return viator::gui_utils::Fonts::regular(font_size);
        }

        void positionComboBoxText(juce::ComboBox &box, juce::Label &label) override
        {
            label.setBounds(box.getLocalBounds().reduced(3.0f, 3.0f));
            label.setFont(getComboBoxFont(box));
        }

        void drawPopupMenuBackground(juce::Graphics &g, [[maybe_unused]] int width,
                                     [[maybe_unused]] int height) override
        {
            g.fillAll(viator::gui_utils::Colors::widget_center());

#if ! JUCE_MAC
            g.setColour (viator::gui_utils::Colors::widget_center());
            g.drawRect (0, 0, width, height);
#endif
        }

        void drawPopupMenuItem(juce::Graphics &g, const juce::Rectangle<int> &area,
                               const bool isSeparator, const bool isActive,
                               const bool isHighlighted, const bool isTicked,
                               const bool hasSubMenu, const juce::String &text,
                               const juce::String &shortcutKeyText,
                               const juce::Drawable *icon, const juce::Colour *const textColourToUse) override
        {
            if (isSeparator) {
                auto r = area.reduced(5, 0);
                r.removeFromTop(juce::roundToInt((static_cast<float>(r.getHeight()) * 0.5f) - 0.5f));

                g.setColour(findColour(juce::PopupMenu::textColourId).withAlpha(0.3f));
                g.fillRect(r.removeFromTop(1));
            } else {
                const auto textColour = (textColourToUse == nullptr
                                       ? findColour(juce::PopupMenu::textColourId)
                                       : *textColourToUse);

                auto r = area.reduced(1);

                if (isHighlighted && isActive) {
                    g.setColour(juce::Colours::black.brighter(0.1f).withAlpha(0.5f));
                    g.fillRect(r);

                    g.setColour(juce::Colours::white);
                } else {
                    g.setColour(textColour.withMultipliedAlpha(isActive ? 1.0f : 0.5f));
                }

                r.reduce(juce::jmin(5, area.getWidth() / 20), 0);

                auto font = getPopupMenuFont();

                const auto maxFontHeight = static_cast<float>(r.getHeight()) / 1.3f;

                if (font.getHeight() > maxFontHeight)
                    font.setHeight(maxFontHeight);

                g.setFont(font);

                const auto iconArea = r.removeFromLeft(juce::roundToInt(maxFontHeight)).toFloat();

                if (icon != nullptr) {
                    icon->drawWithin(g, iconArea,
                                     juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize,
                                     1.0f);
                    r.removeFromLeft(juce::roundToInt(maxFontHeight * 0.5f));
                } else if (isTicked) {
                    const auto tick = getTickShape(1.0f);
                    g.fillPath(
                        tick, tick.getTransformToScaleToFit(
                            iconArea.reduced(iconArea.getWidth() / 5, 0).toFloat(), true));
                }

                if (hasSubMenu) {
                    const auto arrowH = 0.6f * getPopupMenuFont().getAscent();

                    const auto x = static_cast<float>(r.removeFromRight(static_cast<int>(arrowH)).getX());
                    const auto halfH = static_cast<float>(r.getCentreY());

                    juce::Path path;
                    path.startNewSubPath(x, halfH - arrowH * 0.5f);
                    path.lineTo(x + arrowH * 0.6f, halfH);
                    path.lineTo(x, halfH + arrowH * 0.5f);

                    g.strokePath(path, juce::PathStrokeType(2.0f));
                }

                r.removeFromRight(3);
                g.drawFittedText(text, r, juce::Justification::centredLeft, 1);

                if (shortcutKeyText.isNotEmpty()) {
                    auto f2 = font;
                    f2.setHeight(f2.getHeight() * 0.75f);
                    f2.setHorizontalScale(0.95f);
                    g.setFont(f2);

                    g.drawText(shortcutKeyText, r, juce::Justification::centredRight, true);
                }
            }
        }
    };
}
