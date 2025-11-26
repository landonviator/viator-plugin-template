//
// Created by Landon Viator on 10/15/25.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "BinaryData.h"


namespace viator::gui_utils
{
    class Images final
    {
    public:

        static const juce::Image &logo() { return images().m_logo; }

    private:

        Images() = default;

        static Images &images()
        {
            static Images instance;
            return instance;
        }

    protected:
        juce::Image m_logo{juce::ImageCache::getFromMemory(BinaryData::landon_png,
        BinaryData::landon_pngSize)};

        JUCE_DECLARE_NON_COPYABLE(Images)
    };
}