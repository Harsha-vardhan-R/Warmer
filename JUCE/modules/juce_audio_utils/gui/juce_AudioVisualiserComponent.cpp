/*
  ==============================================================================

   This file is part of the JUCE framework.
   Copyright (c) Raw Material Software Limited

   JUCE is an open source framework subject to commercial or open source
   licensing.

   By downloading, installing, or using the JUCE framework, or combining the
   JUCE framework with any other source code, object code, content or any other
   copyrightable work, you agree to the terms of the JUCE End User Licence
   Agreement, and all incorporated terms including the JUCE Privacy Policy and
   the JUCE Website Terms of Service, as applicable, which will bind you. If you
   do not agree to the terms of these agreements, we will not license the JUCE
   framework to you, and you must discontinue the installation or download
   process and cease use of the JUCE framework.

   JUCE End User Licence Agreement: https://juce.com/legal/juce-8-licence/
   JUCE Privacy Policy: https://juce.com/juce-privacy-policy
   JUCE Website Terms of Service: https://juce.com/juce-website-terms-of-service/

   Or:

   You may also use this code under the terms of the AGPLv3:
   https://www.gnu.org/licenses/agpl-3.0.en.html

   THE JUCE FRAMEWORK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL
   WARRANTIES, WHETHER EXPRESSED OR IMPLIED, INCLUDING WARRANTY OF
   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

struct AudioVisualiserComponent::ChannelInfo
{
    ChannelInfo (AudioVisualiserComponent& o, int bufferSize) : owner (o)
    {
        setBufferSize (bufferSize);
        clear();
    }

    void clear() noexcept
    {
        levels.fill ({});
        value = {};
        subSample = 0;
    }

    void pushSamples (const float* inputSamples, int num) noexcept
    {
        for (int i = 0; i < num; ++i)
            pushSample (inputSamples[i]);
    }

    void pushSample (float newSample) noexcept
    {
        if (--subSample <= 0)
        {
            if (++nextSample == levels.size())
                nextSample = 0;

            levels.getReference (nextSample) = value;
            subSample = owner.getSamplesPerBlock();
            value = Range<float> (newSample, newSample);
        }
        else
        {
            value = value.getUnionWith (newSample);
        }
    }

    void setBufferSize (int newSize)
    {
        levels.removeRange (newSize, levels.size());
        levels.insertMultiple (-1, {}, newSize - levels.size());

        if (nextSample >= newSize)
            nextSample = 0;
    }

    AudioVisualiserComponent& owner;
    Array<Range<float>> levels;
    Range<float> value;
    std::atomic<int> nextSample { 0 }, subSample { 0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChannelInfo)
};

//==============================================================================
AudioVisualiserComponent::AudioVisualiserComponent (int initialNumChannels)
  : numSamples (1024),
    inputSamplesPerBlock (256),
    backgroundColour (Colours::black),
    waveformColour (Colours::white)
{
    setOpaque (true);
    setNumChannels (initialNumChannels);
    setRepaintRate (60);
}

AudioVisualiserComponent::~AudioVisualiserComponent()
{
}

void AudioVisualiserComponent::setNumChannels (int numChannels)
{
    channels.clear();

    for (int i = 0; i < numChannels; ++i)
        channels.add (new ChannelInfo (*this, numSamples));
}

void AudioVisualiserComponent::setBufferSize (int newNumSamples)
{
    numSamples = newNumSamples;

    for (auto* c : channels)
        c->setBufferSize (newNumSamples);
}

void AudioVisualiserComponent::clear()
{
    for (auto* c : channels)
        c->clear();
}

void AudioVisualiserComponent::pushBuffer (const float* const* d, int numChannels, int num)
{
    numChannels = jmin (numChannels, channels.size());

    for (int i = 0; i < numChannels; ++i)
        channels.getUnchecked (i)->pushSamples (d[i], num);
}

void AudioVisualiserComponent::pushBuffer (const AudioBuffer<float>& buffer)
{
    pushBuffer (buffer.getArrayOfReadPointers(),
                buffer.getNumChannels(),
                buffer.getNumSamples());
}

void AudioVisualiserComponent::pushBuffer (const AudioSourceChannelInfo& buffer)
{
    auto numChannels = jmin (buffer.buffer->getNumChannels(), channels.size());

    for (int i = 0; i < numChannels; ++i)
        channels.getUnchecked (i)->pushSamples (buffer.buffer->getReadPointer (i, buffer.startSample),
                                               buffer.numSamples);
}

void AudioVisualiserComponent::pushSample (const float* d, int numChannels)
{
    numChannels = jmin (numChannels, channels.size());

    for (int i = 0; i < numChannels; ++i)
        channels.getUnchecked (i)->pushSample (d[i]);
}

void AudioVisualiserComponent::setSamplesPerBlock (int newSamplesPerPixel) noexcept
{
    inputSamplesPerBlock = newSamplesPerPixel;
}

void AudioVisualiserComponent::setRepaintRate (int frequencyInHz)
{
    startTimerHz (frequencyInHz);
}

void AudioVisualiserComponent::timerCallback()
{
    repaint();
}

void AudioVisualiserComponent::setColours (Colour bk, Colour fg) noexcept
{
    backgroundColour = bk;
    waveformColour = fg;
    repaint();
}

void AudioVisualiserComponent::paint (Graphics& g)
{
    g.fillAll (backgroundColour);

    auto r = getLocalBounds().toFloat();
    juce::Rectangle<float> newBounds(0, 3, r.getWidth(), r.getHeight() - 6);

    g.setColour(juce::Colours::grey);
    g.drawRect(r, 1.0f);

    int index = 0;
    for (auto* c : channels) {
        if (index == 0) {
            g.setColour(juce::Colours::red);
            index++;
        } else {
            g.setColour(juce::Colours::blue);
        }
        paintChannel(g, newBounds,
                     c->levels.begin(), c->levels.size(), c->nextSample);
    }
}

void AudioVisualiserComponent::getChannelAsPath (Path& path, const Range<float>* levels,
                                                 int numLevels, int nextSample)
{
    path.preallocateSpace(2 * numLevels); // Adjusted space preallocation

    bool startNewPath = true;

    for (int i = 0; i < numLevels; i += 2) // Skip every other point
    {
        auto level = -(levels[(nextSample + i) % numLevels].getEnd());

        if (startNewPath)
        {
            path.startNewSubPath((float)i, level);
            startNewPath = false;
        }
        else
        {
            path.lineTo((float)i, level);
        }
    }

    startNewPath = true;

    for (int i = numLevels - 2; i >= 0; i -= 2) // Skip every other point
    {
        auto level = -(levels[(nextSample + i) % numLevels].getStart());

        if (startNewPath)
        {
            path.lineTo((float)i, level);
            startNewPath = false;
        }
        else
        {
            path.lineTo((float)i, level);
        }
    }

    path.closeSubPath();
}

void AudioVisualiserComponent::paintChannel (Graphics& g, Rectangle<float> area,
                                             const Range<float>* levels, int numLevels, int nextSample)
{
    Path p;

    getChannelAsPath(p, levels, numLevels, nextSample);

    // Create a PathStrokeType with a thickness of 2.0f
    PathStrokeType strokeType(0.8f);

    // Draw the stroked path with the specified thickness
    g.strokePath(p, strokeType, AffineTransform::fromTargetPoints(0.0f, -1.0f,               area.getX(), area.getY(),
                                                                  0.0f, 1.0f,                area.getX(), area.getBottom(),
                                                                  (float)numLevels, -1.0f,  area.getRight(), area.getY()));
}

} // namespace juce

