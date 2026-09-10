#include "audioCapture.hpp"

#include "pmSDL.hpp"

auto ProjectMSDL::InitAudioInput() -> int
{
    // params for audio input
    SDL_AudioSpec want, have;

    // requested format
    // https://wiki.libsdl.org/SDL_AudioSpec#Remarks
    SDL_zero(want);
    want.freq = 44100;
    want.format = AUDIO_F32; // float
    want.channels = 2;       // mono might be better?
    want.samples = want.freq / 60;
    want.callback = AudioInputCallbackF32;
    want.userdata = this;

    // index -1 means "system default", which is used if we pass deviceName == NULL
    const char* deviceName = m_selectedAudioDevice == -1 ? nullptr : SDL_GetAudioDeviceName(m_selectedAudioDevice, true);
    m_audioDeviceId = SDL_OpenAudioDevice(deviceName, true, &want, &have, 0);

    if (m_audioDeviceId == 0)
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to open audio capture device: %s", SDL_GetError());
        return 0;
    }

    // read characteristics of opened capture device
    if (deviceName == nullptr)
    {
        deviceName = "<System default capture device>";
    }
    SDL_Log("Opened audio capture device index=%i devId=%i: %s", m_selectedAudioDevice, m_audioDeviceId, deviceName);
    std::string deviceToast = deviceName; // Example: Microphone rear
    deviceToast += " selected";
#ifdef DEBUG
    SDL_Log("Samples: %i, frequency: %i, channels: %i, format: %i", have.samples, have.freq, have.channels, have.format);
#endif
    m_audioChannelsCount = have.channels;

    return 1;
}

void ProjectMSDL::AudioInputCallbackF32(void* userdata, unsigned char* stream, int len)
{
    ProjectMSDL const* app = static_cast<ProjectMSDL*>(userdata);
    //    printf("\nLEN: %i\n", len);
    //    for (int i = 0; i < 64; i++)
    //        printf("%X ", stream[i]);
    // stream is (i think) samples*channels floats (native byte order) of len BYTES
    if (app->m_audioChannelsCount == 1)
    {
        projectm_pcm_add_float(app->m_projectM, reinterpret_cast<float*>(stream), len / sizeof(float) / 2, PROJECTM_MONO);
    }
    else if (app->m_audioChannelsCount == 2)
    {
        projectm_pcm_add_float(app->m_projectM, reinterpret_cast<float*>(stream), len / sizeof(float) / 2, PROJECTM_STEREO);
    }
    else
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Multichannel audio not supported");
        SDL_Quit();
    }
}

auto ProjectMSDL::ToggleAudioInput() -> int
{
    // trigger a toggle with CMD-I or CTRL-I
    if (wasapi)
    { // we are currently on WASAPI, so we are going to revert to a microphone/line-in input.
        if (this->OpenAudioInput() != 0)
        {
            this->BeginAudioCapture();
        }
        m_curAudioDevice = -1; // start from system default device
        m_selectedAudioDevice = m_curAudioDevice;
        this->wasapi = false; // Track wasapi as off so projectMSDL will stop listening to WASAPI loopback in pmSDL_main.
    }
    else
    {
        this->EndAudioCapture(); // end current audio capture.
        m_curAudioDevice++;      // iterate device index
        if (m_curAudioDevice >= m_numAudioDevices)
        {                          // We reached outside the boundaries of available audio devices.
            m_curAudioDevice = -1; // Return to the default audio device.
#ifdef WASAPI_LOOPBACK
            // If we are at the boundary and WASAPI is enabled then let's load WASAPI instead.
            SDL_Log("Loopback audio selected");
            this->fakeAudio = false; // disable fakeAudio in case it was enabled.
            this->wasapi = true;     // Track wasapi as on so projectMSDL will listen to it.
#else
            if (m_numAudioDevices == 0) // If WASAPI_LOOPBACK was not enabled and there is only the default audio device, it's pointless to toggle anything.
            {
                SDL_Log("Only the default audio capture device is available. There is nothing to toggle at this time.");
                return 1;
            }
            // If WASAPI_LOOPBACK is not enabled and we have multiple input devices, return to device index 0 and let's listen to that device.
            m_selectedAudioDevice = m_curAudioDevice;
            InitAudioInput();
            this->BeginAudioCapture();
#endif
        }
        else
        {
            // This is a normal scenario where we move forward in the audio device index.
            m_selectedAudioDevice = m_curAudioDevice;
            InitAudioInput();
            this->BeginAudioCapture();
        }
    }
    return 1;
}

auto ProjectMSDL::OpenAudioInput() -> int
{
    fakeAudio = false; // if we are opening an audio input then there is no need for fake audio.
    // get audio driver name (static)
#ifdef DEBUG
    const char* driverName = SDL_GetCurrentAudioDriver();
    SDL_Log("Using audio driver: %s\n", driverName);
#endif

    // get audio input device
    m_numAudioDevices = SDL_GetNumAudioDevices(SDL_TRUE); // capture, please

#ifdef DEBUG
    for (int i = 0; i < m_numAudioDevices; i++)
    {
        SDL_Log("Found audio capture device %d: %s", i, SDL_GetAudioDeviceName(i, SDL_TRUE));
    }
#endif

    // We start with the system default capture device (index -1).
    // Note: this might work even if NumAudioDevices == 0 (example: if only a
    // monitor device exists, and SDL_HINT_AUDIO_INCLUDE_MONITORS is not set).
    // So we always try it, and revert to fakeAudio if the default fails _and_ NumAudioDevices == 0.
    m_curAudioDevice = -1;
    m_selectedAudioDevice = -1;
    if (InitAudioInput() == 0 && m_numAudioDevices == 0)
    {
        // the default device doesn't work, and there's no other device to try
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "No audio capture devices found");
        fakeAudio = true;
        return 0;
    }

    return 1;
}

void ProjectMSDL::BeginAudioCapture() const
{
    // allocate a buffer to store PCM data for feeding in
    SDL_PauseAudioDevice(m_audioDeviceId, SDL_FALSE);
}

void ProjectMSDL::EndAudioCapture() const
{
    SDL_PauseAudioDevice(m_audioDeviceId, SDL_TRUE);
    SDL_CloseAudioDevice(m_audioDeviceId);
}
