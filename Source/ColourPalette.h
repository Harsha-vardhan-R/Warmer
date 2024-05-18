/*
  ==============================================================================

    ColourPalette.h
    Created: 20 Feb 2024 11:21:37pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

// E1F0DA e1f0da

// cool aqua blue : 0xFF6DF1CC
// cool peach : E3FFC3

#define COLOUR 0xFFEEF7FF

static auto textColourID = juce::Colours::darkgrey;
static auto textSelectedColourID = juce::Colours::black;

static auto ButtonBackColourID = juce::Colour(COLOUR).withBrightness(0.95f);  // F2910A
static auto ButtonBackSelectedColourID = juce::Colour(COLOUR).withBrightness(0.9f);  // F2910A
static auto ButtonOutlineColourID = juce::Colour(COLOUR).withBrightness(0.85f);  // F2910A

static auto MenuBackgroundID = juce::Colour(0xfffdfafa);
static auto MenuMouseOverColourID = juce::Colour(0xfffdf7f7);

static auto MouseOverTabColourID = juce::Colour(0xfff3f3e1);
static auto TabBackgroundColourID = juce::Colour(0xfff5f5e7);
static auto SelectTabColourID = juce::Colour(0xffeeeed5);
static auto IdleTabColourID = juce::Colour(0xfff5f5e7);
static auto IdleTabTextColourID = juce::Colours::darkgrey;
static auto SelectedTabTextColourID = juce::Colours::darkgrey;
static auto SelectedTabButtonBorderColourID = juce::Colour(COLOUR).withBrightness(0.9f);  // F2910A

static auto ComboBoxArrowColourID = juce::Colours::darkgrey;
static auto SeparatorColourID = juce::Colours::lightgrey;
static auto ComboBoxTextSelectedColourID = juce::Colours::darkgrey;

static auto WheelComponentBackgroundID = juce::Colour(0xfff5f5e7);  // F2910A
static auto WheelArrowColourID = juce::Colours::darkgrey;
static auto WheelBackgroundID = juce::Colour(0xffc4c4b8);  // F2910A
static auto WheelSliderBackgroundID = juce::Colours::grey; // F2910A
static auto WheelSliderLineBackgroundID = juce::Colours::white;


static auto EditPageBackColourOneID = juce::Colour(COLOUR).withBrightness(0.94f);  // F2910A
static auto EditPageBackColourTwoID = juce::Colour(COLOUR).withBrightness(0.92f);  // F2910A

static auto InstrumentDefaultBackColourID = juce::Colour(COLOUR).withBrightness(0.95f);  // F2910A

static auto VolumeLevelBackgroundID = juce::Colours::grey;
static auto VolumeSliderID = juce::Colours::darkgrey;
static auto VolumeLevelID = juce::Colours::white;
static auto VolumeSliderSlitID = juce::Colours::white;

// node colours.
static auto GraphNodeBackgroundColourID = juce::Colour(0xFFDFF5FF);
static auto GraphNodeHeadingTextID = juce::Colours::darkgrey;
static auto GraphNodeHeadingColourID = juce::Colour(0xFFBED7DC);

static auto GraphNodeConnectionColourID = juce::Colour(0xFFC63D2F);
static auto GraphSliderBackgroundID = juce::Colours::black;
static auto GraphSliderThumbID = juce::Colour(0xFFDCF2F1);
static auto GraphSliderOutlineID = juce::Colours::darkgrey;
static auto GraphSliderTextID = juce::Colour(0xFF6962AD);

static auto GraphNodeConnectionBoxID = juce::Colour(0xFF643843);
static auto GraphNodeSocketTextID = juce::Colour(0xFF8C6A5D);
static auto GraphNodeSocketTextMustID = juce::Colour(0xFF5F374B);



/*
// E1F0DA e1f0da

// Stores the colours for the present theme.
static auto textColourID = juce::Colours::darkgrey;
static auto textSelectedColourID = juce::Colours::white;

static auto ButtonBackColourID = juce::Colours::white;
static auto ButtonBackSelectedColourID = juce::Colours::black;
static auto ButtonOutlineColourID = juce::Colours::darkgrey;

static auto MenuBackgroundID = juce::Colour(0xffe1f0da);
static auto MenuMouseOverColourID = juce::Colour(0xffa9dfbf);


static auto MouseOverTabColourID = juce::Colour(0xcca9dfbf);
static auto SelectTabColourID = juce::Colour(0xffa9dfbf);
static auto IdleTabColourID = juce::Colour(0x88a9dfbf);
static auto IdleTabTextColourID = juce::Colours::grey;
static auto SelectedTabTextColourID = juce::Colours::darkgrey;
static auto SelectedTabButtonBorderColourID = juce::Colours::grey;


static auto ComboBoxArrowColourID = juce::Colours::darkgrey;
static auto SeparatorColourID = juce::Colours::grey;
static auto ComboBoxTextSelectedColourID = juce::Colours::grey;

static auto WheelComponentBackgroundID = juce::Colour(0xffe1f0da);
static auto WheelArrowColourID = juce::Colours::darkgrey;
static auto WheelBackgroundID = juce::Colour(0xffa9dfbf);
static auto WheelSliderBackgroundID = juce::Colours::darkgrey;
static auto WheelSliderLineBackgroundID = juce::Colours::white;


//static auto MIDISettingsBackgroundID = juce::Colour(0xffffffff);
//static auto MIDISettingsTextID = juce::Colours::white;
//static auto MIDISettingsTextSelectID = juce::Colours::darkgrey;
//static auto MIDISettingsButtonColourID = juce::Colours::grey;
//static auto MIDISettingsButtonColourSelectID = juce::Colour(0xffa9dfbf);


static auto GraphNodeBackgroundColourID = juce::Colours::black;
static auto GraphNodeHeadingTextID = juce::Colours::white;
static auto GraphNodeHeadingColourID = juce::Colours::darkgrey;

static auto GraphNodeMIDIColourID = juce::Colour(0xfff9a760);
static auto GraphNodeAudioBufferIntColourID = juce::Colour(0xffffeadb);
static auto GraphNodeAudioBufferFloatColourID = juce::Colour(0xff679b9b);
static auto GraphNodeIntegerColourID = juce::Colour(0xff637373);
static auto GraphNodeFloatingColourID = juce::Colour(0xfff1c2b);
static auto GraphNodeImpColourID = juce::Colours::white;


static auto EditPageBackColourOneID = juce::Colours::grey;
static auto EditPageBackColourTwoID = juce::Colour(0xffe1f0da);

static auto InstrumentDefaultBackColourID = juce::Colour(0xffe1f0da);
*/