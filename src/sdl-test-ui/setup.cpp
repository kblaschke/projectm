#include "setup.hpp"

#include "ConfigFile.h"
#include <projectM-4/logging.h>

#include <SDL2/SDL_hints.h>

#include <chrono>
#include <cmath>
#include <utility>

#if OGL_DEBUG
void DebugGl(GLenum source,
             GLenum type,
             GLuint id,
             GLenum severity,
             GLsizei length,
             const GLchar* message,
             const void* userParam)
{

    /*if (type != GL_DEBUG_TYPE_OTHER)*/
    {
        std::cerr << " -- \n"
                  << "Type: " << type << "; Source: " << source << "; ID: " << id << "; Severity: " << severity << "\n"
                  << message << "\n";
    }
}
#endif

// return path to config file to use
auto GetConfigFilePath(const std::string& datadirPath) -> std::string
{
    std::string projectMHome;
    std::string projectMConfig = datadirPath;

#ifdef _MSC_VER
    char const* home = getenv("USERPROFILE");
#else
    char const* home = getenv("HOME");
#endif

    projectMHome = std::string(home);
    projectMHome += "/.projectM";

    // Create the ~/.projectM directory. If it already exists, mkdir will do nothing
#if defined _MSC_VER
    _mkdir(projectM_home.c_str());
#else
#ifdef _WIN32
    mkdir(projectM_home.c_str());
#else
    mkdir(projectMHome.c_str(), 0755);
#endif
#endif

    projectMHome += "/config.inp";
    projectMConfig += "/config.inp";

    std::ifstream const fHome(projectMHome);
    std::ifstream const fConfig(projectMConfig);
    std::cout << "f_home " << fHome.good() << "\n";

    if (fConfig.good() && !fHome.good())
    {
        std::ifstream fSrc;
        std::ofstream fDst;

        fSrc.open(projectMConfig, std::ios::in | std::ios::binary);
        fDst.open(projectMHome, std::ios::out | std::ios::binary);
        fDst << fSrc.rdbuf();
        fDst.close();
        fSrc.close();
        return projectMHome;
    }

    if (fHome.good())
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Read ~/.projectM/config.inp\n");
        return projectMHome;
    }

    if (fConfig.good())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Cannot create ~/.projectM/config.inp, using %s\n", projectMConfig.c_str());
        return projectMConfig;
    }

    SDL_LogWarn(SDL_LOG_CATEGORY_ERROR, "Using implementation defaults, your system is really messed up, I'm surprised we even got this far\n");
    return {};
}


void SeedRand()
{
#ifndef _WIN32
    srand(static_cast<int>(time(nullptr)));
#endif
}

void InitGl()
{
#ifdef USE_GLES
    // use GLES 3.2
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
    // Disabling compatibility profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
}

void DumpOpenGlInfo()
{
    SDL_Log("- GL_VERSION: %s", glGetString(GL_VERSION));
    SDL_Log("- GL_SHADING_LANGUAGE_VERSION: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    SDL_Log("- GL_VENDOR: %s", glGetString(GL_VENDOR));
}

void EnableGlDebugOutput()
{
#if OGL_DEBUG && !defined(USE_GLES)
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(DebugGl, NULL);
#endif
}

namespace {
void LogMessage(const char* message, projectm_log_level severity, void* /* userData */)
{
    switch (severity)
    {
        case PROJECTM_LOG_LEVEL_FATAL:
        case PROJECTM_LOG_LEVEL_ERROR:
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s\n", message);
            break;
        case PROJECTM_LOG_LEVEL_WARN:
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "%s\n", message);
            break;
        case PROJECTM_LOG_LEVEL_TRACE:
        case PROJECTM_LOG_LEVEL_DEBUG:
            // redirect debug logs to info for now
            //SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "%s\n", message);
            //break;
        case PROJECTM_LOG_LEVEL_NOTSET:
        case PROJECTM_LOG_LEVEL_INFO:
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s\n", message);
            break;
    }
}
} // namespace

// initialize SDL, openGL, config
auto SetupSdlApp() -> ProjectMSDL*
{
    ProjectMSDL* app{nullptr};
    SeedRand();

    projectm_set_log_callback(&LogMessage, false, nullptr);
    projectm_set_log_level(PROJECTM_LOG_LEVEL_DEBUG, false);

    if (!InitLoopback())
    {
        SDL_Log("Failed to initialize audio loopback device.");
        exit(1);
    }

#if UNLOCK_FPS
    setenv("vblank_mode", "0", 1);
#endif

#ifdef SDL_HINT_AUDIO_INCLUDE_MONITORS
    SDL_SetHint(SDL_HINT_AUDIO_INCLUDE_MONITORS, "1");
#endif

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    if (!SDL_VERSION_ATLEAST(2, 0, 5))
    {
        SDL_Log("SDL version 2.0.5 or greater is required. You have %i.%i.%i", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);
        exit(1);
    }

    // default window size to usable bounds (e.g. minus menubar and dock)
    SDL_Rect initialWindowBounds;
#if SDL_VERSION_ATLEAST(2, 0, 5)
    // new and better
    SDL_GetDisplayUsableBounds(0, &initialWindowBounds);
#else
    SDL_GetDisplayBounds(0, &initialWindowBounds);
#endif
    int width = initialWindowBounds.w;
    int height = initialWindowBounds.h;

    InitGl();

    SDL_Window* win = SDL_CreateWindow("projectM", 0, 0, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_GL_GetDrawableSize(win, &width, &height);

    SDL_GLContext glCtx = SDL_GL_CreateContext(win);

    SDL_SetWindowTitle(win, "projectM");

    SDL_GL_MakeCurrent(win, glCtx);          // associate GL context with main window
    int const adaptiveVSync = SDL_GL_SetSwapInterval(-1); // try to enable adaptive vsync
    if (adaptiveVSync == -1)
    {                              // adaptive vsync not supported
        SDL_GL_SetSwapInterval(1); // enable updates synchronized with vertical retrace
    }

#ifdef USE_GLES
    if (!gladLoadGLES2(reinterpret_cast<GLADloadfunc>(SDL_GL_GetProcAddress)))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error loading GLAD for GLES2\n");
    }
#else
    if (gladLoadGL(reinterpret_cast<GLADloadfunc>(SDL_GL_GetProcAddress)) == 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error loading GLAD for GL Core Profile\n");
    }
#endif

    std::string const basePath = DATADIR_PATH;
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Using data directory: %s\n", basePath.c_str());

    // load configuration file
    std::string const configFilePath = GetConfigFilePath(basePath);
    std::string const presetURL = basePath + "/presets";

    app = new ProjectMSDL(glCtx, presetURL);

    if (!configFilePath.empty())
    {
        // found config file, load it
        SDL_Log("Using config from %s", configFilePath.c_str());

        ConfigFile const config(configFilePath);
        auto* projectMHandle = app->ProjectM();

        projectm_set_mesh_size(projectMHandle, config.Read<uint32_t>("Mesh X", 32), config.Read<uint32_t>("Mesh Y", 24));
        SDL_SetWindowSize(win, config.Read<int32_t>("Window Width", 1024), config.Read<int32_t>("Window Height", 768));
        projectm_set_soft_cut_duration(projectMHandle, config.Read<double>("Smooth Preset Duration", config.Read<int>("Smooth Transition Duration", 3)));
        projectm_set_preset_duration(projectMHandle, config.Read<double>("Preset Duration", 30));
        projectm_set_easter_egg(projectMHandle, config.Read<float>("Easter Egg Parameter", 0.0));
        projectm_set_hard_cut_enabled(projectMHandle, config.Read<bool>("Hard Cuts Enabled", false));
        projectm_set_hard_cut_duration(projectMHandle, config.Read<double>("Hard Cut Duration", 60));
        projectm_set_hard_cut_sensitivity(projectMHandle, config.Read<float>("Hard Cut Sensitivity", 1.0));
        projectm_set_beat_sensitivity(projectMHandle, config.Read<float>("Beat Sensitivity", 1.0));
        projectm_set_aspect_correction(projectMHandle, config.Read<bool>("Aspect Correction", true));
        projectm_set_fps(projectMHandle, config.Read<int32_t>("FPS", 60));

        app->SetFps(config.Read<uint32_t>("FPS", 60));
    }

    // center window and full desktop screen
    SDL_DisplayMode displayMode;
    if (SDL_GetDesktopDisplayMode(0, &displayMode) == 0)
    {
        width = displayMode.w;
        height = displayMode.h;
    }
    else
    {
        SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
    }
    SDL_SetWindowPosition(win, initialWindowBounds.x, initialWindowBounds.y);
    SDL_SetWindowSize(win, width, height);
    app->Resize(width, height);
    app->Init(win);

#if FAKE_AUDIO
    app->fakeAudio = true;
#endif

    ConfigureLoopback(app);

#if !FAKE_AUDIO && !WASAPI_LOOPBACK
    // get an audio input device
    if (app->OpenAudioInput() != 0)
    {
        app->BeginAudioCapture();
    }
#endif

#if TEST_ALL_PRESETS
    TestAllPresets(app);
    return 0;
#endif

    return app;
}

auto StartUnlockedFpsCounter() -> int64_t
{
    using namespace std::chrono;
    const auto currentTime = steady_clock::now();
    const auto currentTimeMs = time_point_cast<milliseconds>(currentTime);
    const auto elapsedMs = currentTimeMs.time_since_epoch();

    return elapsedMs.count();
}

void AdvanceUnlockedFpsCounterFrame(int64_t startFrame)
{
    static int32_t frameCount = 0;

    frameCount++;
    auto currentElapsedMs = StartUnlockedFpsCounter();
    if (currentElapsedMs - startFrame > 5000)
    {
        printf("Frames[%d]\n", frameCount);
        exit(0);
    }
}
