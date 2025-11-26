//
// Created by Landon Viator on 11/24/25.
//

#pragma once

namespace viator::gui_utils
{
    class Colors final
    {
    public:

        static juce::Colour editor_bg_color()      { return {26, 27, 27}; }
        static juce::Colour info_bg_color()      { return {54, 54, 54}; }
        static juce::Colour header_bg_color()      { return {34, 34, 34}; }
        static juce::Colour widget_off()      { return {186, 187, 188}; }
        static juce::Colour widget_on()      { return {110, 172, 218}; }
        static juce::Colour widget_center()      { return {70, 70, 70}; }
        static juce::Colour power_on()      { return {120, 185, 181}; }
        static juce::Colour text()      { return {249, 250, 251}; }
    };
}
