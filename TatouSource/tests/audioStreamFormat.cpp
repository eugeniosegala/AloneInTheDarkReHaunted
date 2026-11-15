///////////////////////////////////////////////////////////////////////////////
// Alone In The Dark Re-Haunted
// SDL3 mixer regression: stream PCM must not be mixed in the device's format.
///////////////////////////////////////////////////////////////////////////////

#include "soloud.h"
#include "soloud_backend_data_sdl3.h"
#include <SDL3/SDL.h>
#include <cstdio>

static bool testDevice(int channels)
{
    // SDL_Quit resets hints between cases; never open the user's speakers.
    SDL_SetHintWithPriority(SDL_HINT_AUDIO_DRIVER, "dummy", SDL_HINT_OVERRIDE);
    if (!SDL_Init(SDL_INIT_AUDIO))
        return false;

    // Keep a different physical format open while SoLoud requests F32 stereo.
    SDL_AudioSpec hardware = { SDL_AUDIO_S16, channels, 48000 };
    SDL_AudioDeviceID device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &hardware);
    if (!device)
    {
        SDL_Quit();
        return false;
    }

    SoLoud::Soloud mixer;
    bool passed = mixer.init(SoLoud::Soloud::CLIP_ROUNDOFF, SoLoud::Soloud::SDL3,
                            44100, 2048, 2) == SoLoud::SO_NO_ERROR;
    if (passed)
    {
        auto* backend = static_cast<SoLoud::SoLoudBackendDataSdl3*>(mixer.mBackendData);
        SDL_AudioSpec input = {};
        SDL_AudioSpec output = {};
        passed = SDL_GetAudioStreamFormat(backend->audioStream, &input, &output) &&
                 input.format == SDL_AUDIO_F32 && input.channels == 2 && input.freq == 44100 &&
                 output.channels == channels && output.freq == 48000 &&
                 backend->activeAudioSpec.format == input.format &&
                 mixer.getBackendChannels() == static_cast<unsigned int>(input.channels) &&
                 mixer.getBackendSamplerate() == static_cast<unsigned int>(input.freq);
    }

    mixer.deinit();
    SDL_CloseAudioDevice(device);
    SDL_Quit();
    std::printf("SDL3 stream format with %d-channel device: %s\n", channels, passed ? "PASS" : "FAIL");
    return passed;
}

int main()
{
    // Exercise real SDL conversion without playing through a user's speakers.
    return testDevice(2) && testDevice(6) ? 0 : 1;
}
