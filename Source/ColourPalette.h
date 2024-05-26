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
static auto ComboBoxOutlineColourID = juce::Colours::grey;
static auto SeparatorColourID = juce::Colours::lightgrey;
static auto ComboBoxTextSelectedColourID = juce::Colours::darkgrey;
static auto ComboBoxBackgroundColourID = juce::Colour(0xFFECEFF0);

static auto jucePopupOutlineColourID = juce::Colours::white;


static auto WheelComponentBackgroundID = juce::Colours::white;  // F2910A
static auto WheelArrowColourID = juce::Colours::grey;
static auto WheelBackgroundID = juce::Colours::white;
static auto WheelSliderBackgroundID = juce::Colours::grey;
static auto WheelSliderLineBackgroundID = juce::Colours::lightgrey;


static auto EditPageBackColourOneID = juce::Colours::aqua;  // F2910A
static auto EditPageBackColourTwoID = juce::Colours::coral;  // F2910A

static auto PlayBackgroundColourID = juce::Colours::white;  // F2910A

static auto VolumeLevelBackgroundID = juce::Colours::grey;
static auto VolumeSliderID = juce::Colours::darkgrey;
static auto VolumeLevelID = juce::Colours::white;
static auto VolumeSliderSlitID = juce::Colours::white;

static auto GraphPageBackgroundColourID = juce::Colours::white;

// node colours.
static auto GraphNodeBackgroundColourID = juce::Colour(0xDDFEFEFE);
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





//static auto textColourID = juce::Colours::lightgrey; // Inverse of darkgrey for dark theme
//static auto textSelectedColourID = juce::Colours::white; // Inverse of black for dark theme
//
//static auto ButtonBackColourID = juce::Colour(0xFF333333); // Darker shade
//static auto ButtonBackSelectedColourID = juce::Colour(0xFF444444); // Darker shade
//static auto ButtonOutlineColourID = juce::Colour(0xFF555555); // Darker shade
//
//static auto MenuBackgroundID = juce::Colour(0xFF222222); // Darker background
//static auto MenuMouseOverColourID = juce::Colour(0x44888888); // Adjusted transparency
//
//static auto MouseOverTabColourID = juce::Colour(0xff3b3b3b); // Darker shade
//static auto TabBackgroundColourID = juce::Colour(0xff2b2b2b); // Darker shade
//static auto SelectTabColourID = juce::Colour(0xff1e1e1e); // Darker shade
//static auto IdleTabColourID = juce::Colour(0xff2b2b2b); // Darker shade
//static auto IdleTabTextColourID = juce::Colours::lightgrey; // Inverse of darkgrey
//static auto SelectedTabTextColourID = juce::Colours::lightgrey; // Inverse of darkgrey
//static auto SelectedTabButtonBorderColourID = juce::Colours::silver; // Inverse of grey
//
//static auto ComboBoxArrowColourID = juce::Colours::lightgrey; // Inverse of darkgrey
//static auto ComboBoxOutlineColourID = juce::Colours::silver; // Inverse of grey
//static auto SeparatorColourID = juce::Colours::darkgrey; // Darker shade
//static auto ComboBoxTextSelectedColourID = juce::Colours::lightgrey; // Inverse of darkgrey
//static auto ComboBoxBackgroundColourID = juce::Colour(0xFF2B2E30); // Darker shade
//
//static auto jucePopupOutlineColourID = juce::Colour(0xFF333333); // Darker shade
//
//static auto WheelComponentBackgroundID = juce::Colour(0xFF333333); // Darker shade
//static auto WheelArrowColourID = juce::Colours::silver; // Inverse of grey
//static auto WheelBackgroundID = juce::Colour(0xFF333333); // Darker shade
//static auto WheelSliderBackgroundID = juce::Colours::silver; // Inverse of grey
//static auto WheelSliderLineBackgroundID = juce::Colours::darkgrey; // Darker shade
//
//static auto EditPageBackColourOneID = juce::Colour(0xFF005f5f); // Darker shade of aqua
//static auto EditPageBackColourTwoID = juce::Colour(0xFF5f1f1f); // Darker shade of coral
//
//static auto PlayBackgroundColourID = juce::Colour(0xFF333333); // Darker shade
//
//static auto VolumeLevelBackgroundID = juce::Colours::silver; // Inverse of grey
//static auto VolumeSliderID = juce::Colours::lightgrey; // Inverse of darkgrey
//static auto VolumeLevelID = juce::Colour(0xFF333333); // Darker shade
//static auto VolumeSliderSlitID = juce::Colour(0xFF333333); // Darker shade
//
//static auto GraphPageBackgroundColourID = juce::Colour(0xFF222222); // Darker background
//
//// node colours
//static auto GraphNodeBackgroundColourID = juce::Colour(0xDD2B2B2B); // Darker shade with transparency
//static auto GraphNodeHeadingTextID = juce::Colours::lightgrey; // Inverse of darkgrey
//static auto GraphNodeHeadingColourID = juce::Colour(0xff3b3b3b); // Darker shade
//
//static auto GraphNodeConnectionColourID = juce::Colours::silver; // Inverse of grey
//static auto GraphSliderBackgroundID = juce::Colours::white; // Keeping white for contrast
//static auto GraphSliderThumbID = juce::Colour(0x44FFFFFF); // Adjusted to fit dark theme with transparency
//static auto GraphSliderOutlineID = juce::Colours::lightgrey; // Inverse of darkgrey
//static auto GraphSliderTextID = juce::Colours::lightgrey; // Inverse of darkgrey
//
//static auto GraphNodeConnectionBoxID = juce::Colour(0xFF964349); // Darker and complimentary
//static auto GraphNodeSocketTextID = juce::Colour(0xFFAB8977); // Darker and complimentary
//static auto GraphNodeSocketTextMustID = juce::Colour(0xFF9A4B61); // Darker and complimentary

