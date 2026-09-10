/**
* projectM -- Milkdrop-esque visualisation SDK
* Copyright (C)2003-2019 projectM Team
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
* See 'LICENSE.txt' included within this release
*
* projectM-sdl
* This is an implementation of projectM using libSDL2
* 
* pmSDL.hpp 
* Authors: Created by Mischa Spiegelmock on 2017-09-18.
*
*/

#pragma once

#include "opengl.h"
#include <SDL2/SDL.h>

// Disable LOOPBACK and FAKE audio to enable microphone input
#ifdef _WIN32
#define WASAPI_LOOPBACK 1
#endif /** _WIN32 */
#define FAKE_AUDIO 0
// ----------------------------
#define TEST_ALL_PRESETS 0

// projectM
#include <projectM-4/playlist.h>
#include <projectM-4/projectM.h>

// projectM SDL
#include "audioCapture.hpp"
#include "loopback.hpp"
#include "setup.hpp"


#if defined _MSC_VER
#include <direct.h>
#endif

#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <sys/stat.h>

#ifdef WASAPI_LOOPBACK
#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>

#include <functiondiscoverykeys_devpkey.h>
#include <avrt.h>

#include <mmsystem.h>
#include <stdio.h>


#define LOG(format, ...) wprintf(format L"\n", ##__VA_ARGS__)
#define ERR(format, ...) LOG(L"Error: " format, ##__VA_ARGS__)

#endif /** WASAPI_LOOPBACK */

#ifdef _WIN32
#define SDL_MAIN_HANDLED
#include "SDL.h"
#else
#include <SDL2/SDL.h>
#endif /** _WIN32 */

// DATADIR_PATH should be set by CMake to the current build dir
#ifndef DATADIR_PATH
#error "DATADIR_PATH is not defined!"
#endif

class ProjectMSDL
{

public:
    ProjectMSDL(SDL_GLContext glCtx, const std::string& presetPath);

    ~ProjectMSDL();

    void Init(SDL_Window* window);
    auto OpenAudioInput() -> int;
    auto ToggleAudioInput() -> int;
    auto InitAudioInput() -> int;
    void BeginAudioCapture() const;
    void EndAudioCapture() const;
    void StretchMonitors() const;
    void NextMonitor() const;
    void ToggleFullScreen();
    void Resize(unsigned int width, unsigned int height);
    void RenderFrame() const;
    void PollEvent();
    auto GetActivePresetName() const -> std::string;
    void AddFakePcm() const;
    auto ProjectM() const -> projectm_handle;
    void SetFps(size_t fps);
    auto Fps() const -> size_t;

    bool keymod = false;
    bool done{false};
    bool mouseDown{false};
    bool wasapi{false};    // Used to track if wasapi is currently active. This bool will allow us to run a WASAPI app and still toggle to microphone inputs.
    bool fakeAudio{false}; // Used to track fake audio, so we can turn it off and on.
    bool stretch{false};   // used for toggling stretch mode

    SDL_GLContext openGlContext{nullptr};

private:
    static void PresetSwitchedEvent(bool isHardCut, uint32_t index, void* context);

    static void AudioInputCallbackF32(void* userdata, unsigned char* stream, int len);

    void UpdateWindowTitle() const;

    void ScrollHandler(const SDL_Event*);
    void KeyHandler(SDL_Event*);

    projectm_handle m_projectM{nullptr};
    projectm_playlist_handle m_playlist{nullptr};

    SDL_Window* m_sdlWindow{nullptr};
    bool m_isFullScreen{false};
    size_t m_width{0};
    size_t m_height{0};
    size_t m_fps{60};

    bool m_shuffle{true};

    // audio input device characteristics
    int m_numAudioDevices{0};
    int m_curAudioDevice{0}; // SDL's device indexes are 0-based, -1 means "system default"
    unsigned short m_audioChannelsCount{0};
    SDL_AudioDeviceID m_audioDeviceId{0};
    int m_selectedAudioDevice{0};

    std::string m_presetName; //!< Current preset name
};
