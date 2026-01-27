/*
  ==============================================================================

    DSP_Sketchbook.cpp
    Created: 22 Jan 2026 3:01:23pm
    Author:  Billy James

  ==============================================================================
*/
#ifdef DSP_SKETCHBOOK_INCLUDED
 /* When you add this cpp file to your project, you mustn't include it in a file where you've
    already included any other headers - just put it inside a file on its own, possibly with your config
    flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
    header files that the compiler may be using.
 */
 #error "Incorrect use of JUCE cpp file"
#endif //DSP_SKETCHBOOK_INCLUDED

//include our header files
#include "DSP_Sketchbook.h"

//BINARY DATA
#include "Resources/BinaryData/DSP_SKETCHBOOK_BINARY.cpp"

//ENGINE
#include "Engine/Module.cpp"
#include "Engine/Voices.cpp"
//#include "Engine/Engine.cpp"

//MODULES
#include "Modules/EnvelopeModule.cpp"
#include "Modules/ModulationSources.cpp"

//App
#include "App/PluginEditor.cpp"
#include "App/PluginProcessor.cpp"

//UI
#include "UI/LookAndFeel.cpp"
#include "UI/StyledSlider.cpp"
