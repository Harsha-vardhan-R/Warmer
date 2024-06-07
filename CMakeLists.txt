cmake_minimum_required(VERSION 3.15)
project(Warmer VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type to Release by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

include_directories(
        ${CMAKE_HOME_DIRECTORY}/JuceLibraryCode
        ${CMAKE_HOME_DIRECTORY}/JuceLibraryCode/modules
        ${CMAKE_HOME_DIRECTORY}/Source
)

# From Projucer generated MakeFile.
add_compile_definitions(
        LINUX=1
        JUCE_DISPLAY_SPLASH_SCREEN=1
        JUCE_USE_DARK_SPLASH_SCREEN=0
        JUCE_PROJUCER_VERSION=0x70009
        JUCE_MODULE_AVAILABLE_juce_audio_basics=1
        JUCE_MODULE_AVAILABLE_juce_audio_devices=1
        JUCE_MODULE_AVAILABLE_juce_audio_formats=1
        JUCE_MODULE_AVAILABLE_juce_audio_processors=1
        JUCE_MODULE_AVAILABLE_juce_audio_utils=1
        JUCE_MODULE_AVAILABLE_juce_core=1
        JUCE_MODULE_AVAILABLE_juce_data_structures=1
        JUCE_MODULE_AVAILABLE_juce_dsp=1
        JUCE_MODULE_AVAILABLE_juce_events=1
        JUCE_MODULE_AVAILABLE_juce_graphics=1
        JUCE_MODULE_AVAILABLE_juce_gui_basics=1
        JUCE_MODULE_AVAILABLE_juce_gui_extra=1
        JUCE_MODULE_AVAILABLE_juce_midi_ci=1
        JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1
        JUCE_STRICT_REFCOUNTEDPOINTER=1
        JUCE_STANDALONE_APPLICATION=1
        JUCER_LINUX_MAKE_6D53C8B4=1
        JUCE_APP_VERSION=0.1.0
        JUCE_APP_VERSION_HEX=0x100
        JucePlugin_Build_VST=0
        JucePlugin_Build_VST3=0
        JucePlugin_Build_AU=0
        JucePlugin_Build_AUv3=0
        JucePlugin_Build_AAX=0
        JucePlugin_Build_Standalone=0
        JucePlugin_Build_Unity=0
        JucePlugin_Build_LV2=0
)

# Compiler flags for debug and release builds
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O2 -ggdb -pthread")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -flto -Wall -Wcast-align -Wfloat-equal -Wno-ignored-qualifiers -Wsign-compare -Wsign-conversion -Wstrict-aliasing -Wswitch-enum -Wuninitialized -Wunreachable-code -Wunused-parameter -Wmissing-field-initializers -Wextra -Wsign-compare -Wno-implicit-fallthrough -Wno-maybe-uninitialized -Wredundant-decls -Wno-strict-overflow -Wshadow -Woverloaded-virtual -Wreorder -Wzero-as-null-pointer-constant -s")

# Add source files
set(SOURCES
        ./Source/MyDataStructures.cpp
        ./Source/WaveShapeDisp.cpp
        ./Source/MyLookAndFeel.cpp
        ./Source/VolumeLevel.cpp
        ./Source/Instrument.cpp
        ./Source/Wheels.cpp
        ./Source/Piano.cpp
        ./Source/MIDIPianoComponent.cpp
        ./Source/GraphNodes/GraphNode.cpp
        ./Source/GraphNodes/Connection.cpp
        ./Source/MasterComponent.cpp
        ./Source/Main.cpp
        ./Source/MenuComponent.cpp
        ./Source/MainComponent.cpp
        ./JuceLibraryCode/include_juce_audio_basics.cpp
        ./JuceLibraryCode/include_juce_audio_devices.cpp
        ./JuceLibraryCode/include_juce_audio_formats.cpp
        ./JuceLibraryCode/include_juce_audio_processors.cpp
        ./JuceLibraryCode/include_juce_audio_processors_ara.cpp
        ./JuceLibraryCode/include_juce_audio_processors_lv2_libs.cpp
        ./JuceLibraryCode/include_juce_audio_utils.cpp
        ./JuceLibraryCode/include_juce_core.cpp
        ./JuceLibraryCode/include_juce_data_structures.cpp
        ./JuceLibraryCode/include_juce_dsp.cpp
        ./JuceLibraryCode/include_juce_events.cpp
        ./JuceLibraryCode/include_juce_graphics.cpp
        ./JuceLibraryCode/include_juce_gui_basics.cpp
        ./JuceLibraryCode/include_juce_gui_extra.cpp
        ./JuceLibraryCode/include_juce_midi_ci.cpp
)

add_executable(Warmer ${SOURCES})

find_package(PkgConfig REQUIRED)
pkg_check_modules(ALSA REQUIRED alsa)
pkg_check_modules(FREETYPE2 REQUIRED freetype2)
pkg_check_modules(GL REQUIRED gl)
pkg_check_modules(LIBCURL REQUIRED libcurl)
pkg_check_modules(WEBKIT2GTK REQUIRED webkit2gtk-4.0)
pkg_check_modules(GTK REQUIRED gtk+-x11-3.0)

target_link_libraries(Warmer
        ${ALSA_LIBRARIES}
        ${FREETYPE2_LIBRARIES}
        ${GL_LIBRARIES}
        ${LIBCURL_LIBRARIES}
        ${WEBKIT2GTK_LIBRARIES}
        ${GTK_LIBRARIES}
        pthread
        atomic
        rt
        dl
)

target_include_directories(Warmer PRIVATE
        ${ALSA_INCLUDE_DIRS}
        ${FREETYPE2_INCLUDE_DIRS}
        ${GL_INCLUDE_DIRS}
        ${LIBCURL_INCLUDE_DIRS}
        ${WEBKIT2GTK_INCLUDE_DIRS}
        ${GTK_INCLUDE_DIRS}
)


#if(CMAKE_BUILD_TYPE STREQUAL "Release")
#    add_custom_command(TARGET Warmer POST_BUILD
#            COMMAND ${CMAKE_STRIP} $<TARGET_FILE:Warmer>
#            COMMENT "Stripping symbols to reduce binary size.")
#endif()

# Add custom command to clean
add_custom_target(clean-all
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/cmake_clean_target.cmake
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
        COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/Makefile
        COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeCache.txt
        COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/cmake_install.cmake
)
