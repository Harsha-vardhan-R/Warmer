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


static auto textColourID = juce::Colours::darkgrey;
static auto textSelectedColourID = juce::Colours::black;

static auto ButtonBackColourID = juce::Colours::white;  // F2910A
static auto ButtonBackSelectedColourID = juce::Colours::white;  // F2910A
static auto ButtonOutlineColourID = juce::Colours::white;  // F2910A

static auto MenuBackgroundID = juce::Colours::white;
static auto MenuMouseOverColourID = juce::Colour(0x447B8788);

static auto MouseOverTabColourID = juce::Colour(0xfff3f3e1);
static auto TabBackgroundColourID = juce::Colour(0xfff5f5e7);
static auto SelectTabColourID = juce::Colour(0xffeeeed5);
static auto IdleTabColourID = juce::Colour(0xfff5f5e7);
static auto IdleTabTextColourID = juce::Colours::darkgrey;
static auto SelectedTabTextColourID = juce::Colours::darkgrey;
static auto SelectedTabButtonBorderColourID = juce::Colours::grey;  // F2910A

static auto ComboBoxArrowColourID = juce::Colours::darkgrey;
static auto SeparatorColourID = juce::Colours::lightgrey;
static auto ComboBoxTextSelectedColourID = juce::Colours::darkgrey;

static auto WheelComponentBackgroundID = juce::Colours::white;  // F2910A
static auto WheelArrowColourID = juce::Colours::grey;
static auto WheelBackgroundID = juce::Colours::white;
static auto WheelSliderBackgroundID = juce::Colours::grey;
static auto WheelSliderLineBackgroundID = juce::Colours::lightgrey;


static auto EditPageBackColourOneID = juce::Colours::white;  // F2910A
static auto EditPageBackColourTwoID = juce::Colours::white;  // F2910A

static auto InstrumentDefaultBackColourID = juce::Colours::white;  // F2910A

static auto VolumeLevelBackgroundID = juce::Colours::grey;
static auto VolumeSliderID = juce::Colours::darkgrey;
static auto VolumeLevelID = juce::Colours::white;
static auto VolumeSliderSlitID = juce::Colours::white;

// node colours.
static auto GraphNodeBackgroundColourID = juce::Colour(0xDDEBEBEB);
static auto GraphNodeHeadingTextID = juce::Colours::darkgrey;
static auto GraphNodeHeadingColourID = juce::Colour(0xffcacaca);

static auto GraphNodeConnectionColourID = juce::Colours::grey;
static auto GraphSliderBackgroundID = juce::Colours::black;
static auto GraphSliderThumbID = juce::Colour(0x44000000);
static auto GraphSliderOutlineID = juce::Colours::darkgrey;
static auto GraphSliderTextID = juce::Colours::darkgrey;

static auto GraphNodeConnectionBoxID = juce::Colour(0xFF643843);
static auto GraphNodeSocketTextID = juce::Colour(0xFF8C6A5D);
static auto GraphNodeSocketTextMustID = juce::Colour(0xFF5F374B);
//static auto textColourID = juce::Colours::lightgrey;
//static auto textSelectedColourID = juce::Colours::white;
//
//static auto ButtonBackColourID = juce::Colour(0xff333333); // Dark grey
//static auto ButtonBackSelectedColourID = juce::Colour(0xff444444); // Slightly lighter dark grey
//static auto ButtonOutlineColourID = juce::Colour(0xff555555); // Even lighter dark grey
//
//static auto MenuBackgroundID = juce::Colour(0xff222222); // Very dark grey
//static auto MenuMouseOverColourID = juce::Colour(0x447B8788); // Maintain transparency
//
//static auto MouseOverTabColourID = juce::Colour(0xff2b2b2b); // Dark grey similar to mouse over tab
//static auto TabBackgroundColourID = juce::Colour(0xff2a2a2a); // Slightly different dark grey
//static auto SelectTabColourID = juce::Colour(0xff252525); // Darker grey for selected tab
//static auto IdleTabColourID = juce::Colour(0xff2a2a2a); // Same as TabBackgroundColourID
//static auto IdleTabTextColourID = juce::Colours::lightgrey;
//static auto SelectedTabTextColourID = juce::Colours::white;
//static auto SelectedTabButtonBorderColourID = juce::Colour(0xff555555); // Dark grey for border
//
//static auto ComboBoxArrowColourID = juce::Colours::lightgrey;
//static auto SeparatorColourID = juce::Colours::darkgrey;
//static auto ComboBoxTextSelectedColourID = juce::Colours::lightgrey;
//
//static auto WheelComponentBackgroundID = juce::Colour(0xff333333); // Dark grey
//static auto WheelArrowColourID = juce::Colours::lightgrey;
//static auto WheelBackgroundID = juce::Colour(0xff333333); // Same as WheelComponentBackgroundID
//static auto WheelSliderBackgroundID = juce::Colours::darkgrey;
//static auto WheelSliderLineBackgroundID = juce::Colours::grey;
//
//static auto EditPageBackColourOneID = juce::Colour(0xff333333); // Dark grey
//static auto EditPageBackColourTwoID = juce::Colour(0xff333333); // Same as EditPageBackColourOneID
//
//static auto InstrumentDefaultBackColourID = juce::Colour(0xff333333); // Same dark grey
//
//static auto VolumeLevelBackgroundID = juce::Colours::darkgrey;
//static auto VolumeSliderID = juce::Colours::grey;
//static auto VolumeLevelID = juce::Colour(0xff555555); // Slightly lighter grey
//static auto VolumeSliderSlitID = juce::Colour(0xff555555); // Same as VolumeLevelID
//
//// Node colours.
//static auto GraphNodeBackgroundColourID = juce::Colour(0xff3a3a3a); // Darker grey for nodes
//static auto GraphNodeHeadingTextID = juce::Colours::lightgrey;
//static auto GraphNodeHeadingColourID = juce::Colour(0xff444444); // Dark grey
//
//static auto GraphNodeConnectionColourID = juce::Colours::darkgrey;
//static auto GraphSliderBackgroundID = juce::Colours::lightgrey;
//static auto GraphSliderThumbID = juce::Colour(0x44ffffff); // Light thumb with transparency
//static auto GraphSliderOutlineID = juce::Colours::grey;
//static auto GraphSliderTextID = juce::Colours::lightgrey;
//
//static auto GraphNodeConnectionBoxID = juce::Colour(0xffb35a60); // Darker red tone
//static auto GraphNodeSocketTextID = juce::Colour(0xffc38a7e); // Muted pink tone
//static auto GraphNodeSocketTextMustID = juce::Colour(0xffa04e5e); // Darker muted pink



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