/*
  ==============================================================================

    Collection.h
    Created: 24 Feb 2024 3:59:01pm
    Author:  harshavardhan

  ==============================================================================
*/

/*
 * Collection of GraphNodes to include all at the same time.
 */
#pragma once

#include "GraphNode.h"

// Mandatory nodes that should be present in the graph.
#include "InputMasterGraphNode.h"
#include "OutputMasterGraphNode.h"

#include "Oscillators/Oscillator.h"
#include "Oscillators/Noise.h"
#include "Oscillators/RandomOscillator.h"

#include "Math/ClampSignal.h"
#include "Math/MixSignal.h"
#include "Math/ReRamp.h"
#include "Math/AddAndMul.h"
#include "Math/AddOrSubtractSignal.h"
#include "Math/MathClamp.h"
#include "Math/Absolute.h"
#include "Math/Mixer.h"
#include "Math/FM.h"

#include "General/Utility.h"
#include "General/AddLag.h"
#include "General/Convolution.h"
#include "General/ADSR.h"
#include "General/Feedback.h"
#include "General/Envelope.h"

#include "Effects/Delay.h"
#include "Effects/Distortion.h"
#include "Effects/Reverb.h"

#include "MIDI/MIDI_Polyphony.h"
#include "MIDI/MIDI_Transpose.h"
#include "MIDI/MIDI_NoteToFreq.h"

#include "Filters/Digital.h"
#include "Filters/ladder.h"
