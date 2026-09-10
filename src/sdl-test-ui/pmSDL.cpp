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
* pmSDL.cpp
* Authors: Created by Mischa Spiegelmock on 2017-09-18.
*
*
* experimental Stereoscopic SBS driver functionality by
*	RobertPancoast77@gmail.com
*/

#include "pmSDL.hpp"

#include <algorithm>
#include <vector>

namespace {
auto DispatchLoadProc(const char* name, void* /* userData */) -> void*
{
    // Dispatch load proc to SDL
    return SDL_GL_GetProcAddress(name);
}
} // namespace

ProjectMSDL::ProjectMSDL(SDL_GLContext glCtx, const std::string& presetPath)
    : openGlContext(glCtx)
    , m_projectM(projectm_create_with_opengl_load_proc(&DispatchLoadProc, nullptr))
    , m_playlist(projectm_playlist_create(m_projectM))
{
    projectm_get_window_size(m_projectM, &m_width, &m_height);
    projectm_playlist_set_preset_switched_event_callback(m_playlist, &ProjectMSDL::PresetSwitchedEvent, static_cast<void*>(this));
    projectm_playlist_add_path(m_playlist, presetPath.c_str(), true, false);
    projectm_playlist_set_shuffle(m_playlist, m_shuffle);
    DumpOpenGlInfo();
    EnableGlDebugOutput();
}

ProjectMSDL::~ProjectMSDL()
{
    projectm_playlist_destroy(m_playlist);
    m_playlist = nullptr;
    projectm_destroy(m_projectM);
    m_projectM = nullptr;
}

/* Stretch projectM across multiple monitors */
void ProjectMSDL::StretchMonitors() const
{
    int displayCount = SDL_GetNumVideoDisplays();
    if (displayCount >= 2)
    {
        std::vector<SDL_Rect> displayBounds;
        for (int i = 0; i < displayCount; i++)
        {
            displayBounds.emplace_back();
            SDL_GetDisplayBounds(i, &displayBounds.back());
        }

        int mostXLeft = 0;
        int mostXRight = 0;
        int mostYUp = 0;
        int mostYDown = 0;

        for (int i = 0; i < displayCount; i++)
        {
            mostXLeft = std::min(displayBounds.at(i).x, mostXLeft);
            mostXRight = std::max(displayBounds.at(i).x + displayBounds.at(i).w, mostXRight);
        }
        for (int i = 0; i < displayCount; i++)
        {
            mostYUp = std::min(displayBounds.at(i).y, mostYUp);
            mostYDown = std::max(displayBounds.at(i).y + displayBounds.at(i).h, mostYDown);
        }

        int const mostWide = abs(mostXLeft) + abs(mostXRight);
        int const mostHigh = abs(mostYUp) + abs(mostYDown);

        SDL_SetWindowPosition(m_sdlWindow, mostXLeft, mostYUp);
        SDL_SetWindowSize(m_sdlWindow, mostWide, mostHigh);
    }
}

/* Moves projectM to the next monitor */
void ProjectMSDL::NextMonitor() const
{
    int const displayCount = SDL_GetNumVideoDisplays();
    int const currentWindowIndex = SDL_GetWindowDisplayIndex(m_sdlWindow);
    if (displayCount >= 2)
    {
        std::vector<SDL_Rect> displayBounds;
        int nextWindow = currentWindowIndex + 1;
        if (nextWindow >= displayCount)
        {
            nextWindow = 0;
        }

        for (int i = 0; i < displayCount; i++)
        {
            displayBounds.emplace_back();
            SDL_GetDisplayBounds(i, &displayBounds.back());
        }
        SDL_SetWindowPosition(m_sdlWindow, displayBounds.at(nextWindow).x, displayBounds.at(nextWindow).y);
        SDL_SetWindowSize(m_sdlWindow, displayBounds.at(nextWindow).w, displayBounds.at(nextWindow).h);
    }
}

void ProjectMSDL::ToggleFullScreen()
{
    if (m_isFullScreen)
    {
        SDL_SetWindowFullscreen(m_sdlWindow, 0);
        m_isFullScreen = false;
        SDL_ShowCursor(SDL_TRUE);
    }
    else
    {
        SDL_ShowCursor(SDL_FALSE);
        SDL_SetWindowFullscreen(m_sdlWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
        m_isFullScreen = true;
    }
}

void ProjectMSDL::ScrollHandler(const SDL_Event* sdlEvent)
{
    // handle mouse scroll wheel - up++
    if (sdlEvent->wheel.y > 0)
    {
        projectm_playlist_play_previous(m_playlist, true);
    }
    // handle mouse scroll wheel - down--
    if (sdlEvent->wheel.y < 0)
    {
        projectm_playlist_play_next(m_playlist, true);
    }
}

void ProjectMSDL::KeyHandler(SDL_Event* sdlEvent)
{
    const auto sdlModifier = static_cast<SDL_Keymod>(sdlEvent->key.keysym.mod);
    SDL_Keycode const sdlKeyCode = sdlEvent->key.keysym.sym;

    // Left or Right Gui or Left Ctrl
    if ((sdlModifier & KMOD_LGUI) != 0 || (sdlModifier & KMOD_RGUI) != 0 || (sdlModifier & KMOD_LCTRL) != 0)
    {
        keymod = true;
    }

    // handle keyboard input (for our app first, then projectM)
    switch (sdlKeyCode)
    {
        case SDLK_a:
            projectm_set_aspect_correction(m_projectM, !projectm_get_aspect_correction(m_projectM));
            break;

        case SDLK_q:
            if (keymod)
            {
                // cmd/ctrl-q = quit
                done = true;
                return;
            }
            break;

        case SDLK_i:
            if (keymod)
            {
                ToggleAudioInput();
            }
            break;

        case SDLK_s:
            if (keymod)
            {
                // command-s: [s]tretch monitors
                if (!this->stretch)
                { // if stretching is not already enabled, enable it.
                    StretchMonitors();
                    this->stretch = true;
                }
                else
                {
                    ToggleFullScreen(); // else, just toggle full screen so we leave stretch mode.
                    this->stretch = false;
                }
            }
            break;

        case SDLK_m:
            if (keymod)
            {
                // command-m: change [m]onitor
                NextMonitor();
                this->stretch = false; // if we are switching monitors, ensure we disable monitor stretching.
            }
            break;

        case SDLK_f:
            if (keymod)
            {
                // command-f: fullscreen
                ToggleFullScreen();
                this->stretch = false; // if we are toggling fullscreen, ensure we disable monitor stretching.
                return;                // handled
            }
            break;

        case SDLK_r:
            // Use playlist shuffle to randomize.
            projectm_playlist_set_shuffle(m_playlist, true);
            projectm_playlist_play_next(m_playlist, true);
            projectm_playlist_set_shuffle(m_playlist, m_shuffle);
            break;

        case SDLK_y:
            m_shuffle = !m_shuffle;
            projectm_playlist_set_shuffle(m_playlist, m_shuffle);
            break;

        case SDLK_LEFT:
            projectm_playlist_play_previous(m_playlist, true);
            break;

        case SDLK_RIGHT:
            projectm_playlist_play_next(m_playlist, true);
            break;

        case SDLK_UP:
            projectm_set_beat_sensitivity(m_projectM, projectm_get_beat_sensitivity(m_projectM) + 0.01f);
            break;

        case SDLK_DOWN:
            projectm_set_beat_sensitivity(m_projectM, projectm_get_beat_sensitivity(m_projectM) - 0.01f);
            break;

        case SDLK_SPACE:
            projectm_set_preset_locked(m_projectM, !projectm_get_preset_locked(m_projectM));
            UpdateWindowTitle();
            break;

        default:;
    }
}

void ProjectMSDL::AddFakePcm() const
{
    std::array<int16_t, 1024> pcmData{};
    /** Produce some fake PCM data to stuff into projectM */
    for (unsigned long i = 0; i < 512; i++)
    {
        pcmData.at(2 * i) = static_cast<int16_t>(static_cast<float>(rand() / static_cast<float>(RAND_MAX) * pow(2, 14)));
        pcmData.at((2 * i) + 1) = static_cast<int16_t>(static_cast<float>(rand() / static_cast<float>(RAND_MAX) * pow(2, 14)));
        if (i % 2 == 1)
        {
            pcmData.at(2 * i) = -pcmData.at(2 * i);
            pcmData.at((2 * i) + 1) = -pcmData.at((2 * i) + 1);
        }
    }

    /** Add the waveform data */
    projectm_pcm_add_int16(m_projectM, pcmData.data(), 512, PROJECTM_STEREO);
}

void ProjectMSDL::Resize(const unsigned int width, const unsigned int height)
{
    m_width = width;
    m_height = height;

    // Hide cursor if window size equals desktop size
    SDL_DisplayMode displayMode;
    if (SDL_GetDesktopDisplayMode(0, &displayMode) == 0)
    {
        SDL_ShowCursor(m_isFullScreen ? SDL_DISABLE : SDL_ENABLE);
    }

    projectm_set_window_size(m_projectM, m_width, m_height);
}

void ProjectMSDL::PollEvent()
{
    SDL_Event evt;

    while (SDL_PollEvent(&evt) != 0)
    {
        switch (evt.type)
        {
            case SDL_WINDOWEVENT: {
                int height{};
                int width{};
                SDL_GL_GetDrawableSize(m_sdlWindow, &width, &height);
                switch (evt.window.event)
                {
                    case SDL_WINDOWEVENT_RESIZED:
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        Resize(width, height);
                        break;
                    default:;
                }
                break;
            }
            case SDL_MOUSEWHEEL:
                ScrollHandler(&evt);
                break;

            case SDL_KEYDOWN:
                KeyHandler(&evt);
                break;

            case SDL_MOUSEBUTTONUP:
                mouseDown = false;
                break;

            case SDL_QUIT:
                done = true;
                break;

            default:;
        }
    }
}

void ProjectMSDL::RenderFrame() const
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    projectm_opengl_render_frame(m_projectM);

    SDL_GL_SwapWindow(m_sdlWindow);
}

void ProjectMSDL::Init(SDL_Window* window)
{
    m_sdlWindow = window;
    projectm_set_window_size(m_projectM, m_width, m_height);

#ifdef WASAPI_LOOPBACK
    wasapi = true;
#endif
}

auto ProjectMSDL::GetActivePresetName() const -> std::string
{
    unsigned int const index = projectm_playlist_get_position(m_playlist);
    if (index != 0u)
    {
        auto *presetName = projectm_playlist_item(m_playlist, index);
        std::string presetNameString(presetName);
        projectm_playlist_free_string(presetName);
        return presetNameString;
    }
    return {};
}

void ProjectMSDL::PresetSwitchedEvent(bool /* isHardCut */, unsigned int index, void* context)
{
    auto *app = reinterpret_cast<ProjectMSDL*>(context);
    auto *presetName = projectm_playlist_item(app->m_playlist, index);
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Displaying preset: %s\n", presetName);

    app->m_presetName = presetName;
    projectm_playlist_free_string(presetName);

    app->UpdateWindowTitle();
}

auto ProjectMSDL::ProjectM() const -> projectm_handle
{
    return m_projectM;
}

void ProjectMSDL::SetFps(size_t fps)
{
    m_fps = fps;
}

auto ProjectMSDL::Fps() const -> size_t
{
    return m_fps;
}

void ProjectMSDL::UpdateWindowTitle() const
{
    std::string title = "projectM ➫ " + m_presetName;
    if (projectm_get_preset_locked(m_projectM))
    {
        title.append(" [locked]");
    }
    SDL_SetWindowTitle(m_sdlWindow, title.c_str());
}
