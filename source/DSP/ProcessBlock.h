//
// Created by Landon Viator on 11/24/25.
//

#pragma once
#include <juce_dsp/juce_dsp.h>

namespace viator::dsp
{
    class ProcessBlock
    {
    public:
        ProcessBlock() = default;
        ~ProcessBlock() = default;

        void prepare(const double sample_rate, const int samples_per_block, const int num_channels, int factor)
        {
            juce::dsp::ProcessSpec spec{};
            spec.sampleRate = sample_rate;
            spec.maximumBlockSize = samples_per_block;
            spec.numChannels= num_channels;

            m_oversampler = std::make_unique<juce::dsp::Oversampling<float>>(spec.numChannels,
                    factor,
                    juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR,
                    true);
            m_oversampler->initProcessing(spec.maximumBlockSize);
        }

        void process(juce::AudioBuffer<float>& buffer, const int num_samples)
        {
            const int num_oversampled_samples = num_samples * static_cast<int>(m_oversampler->getOversamplingFactor());
            juce::dsp::AudioBlock<float> block (buffer);
            auto up_sampled_block = m_oversampler->processSamplesUp(block);

            for (size_t channel = 0; channel < up_sampled_block.getNumChannels(); ++channel) {
                auto *data = up_sampled_block.getChannelPointer(channel);
                for (size_t sample = 0; sample < num_oversampled_samples; ++sample) {
                    const float xn = data[sample];
                    const float yn = 2.0f / 3.14f * std::atan(xn * 10.0f);
                    data[sample] = yn;
                }
            }

            m_oversampler->processSamplesDown(block);
        }

        static void updateParameters(viator::globals::PluginParameters::parameters& parameters)
        {
        }

    private:
        std::unique_ptr<juce::dsp::Oversampling<float>> m_oversampler;
    };
}

