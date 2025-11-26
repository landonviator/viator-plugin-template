//
// Created by Landon Viator on 11/24/25.
//

#pragma once

namespace viator::gui_utils
{
    class Colors final
    {
    public:

        static juce::Colour theme()      { return {52, 76, 100}; }
        static juce::Colour editor_bg_color()      { return {34, 40, 49}; }
        static juce::Colour info_bg_color()      { return {67, 72, 80}; }
        static juce::Colour header_bg_color()      { return {57, 62, 70}; }
        static juce::Colour widget_off()      { return {186, 187, 188}; }
        static juce::Colour widget_on()      { return {110, 172, 218}; }
        static juce::Colour widget_center()      { return {67, 72, 80}; }
        static juce::Colour power_on()      { return {120, 185, 181}; }
        static juce::Colour text()      { return {249, 250, 251}; }
    };
}
