#include "audio_context.h"

// Define the miniaudio implementation exactly once in the project
#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include <iostream>

namespace audio {

    void AudioContext::Initialize() {
        if (state.isInitialized) return;

        ma_result result = ma_engine_init(NULL, &state.engine);
        if (result != MA_SUCCESS) {
            std::cerr << "Failed to initialize audio engine." << std::endl;
            return;
        }

        state.isInitialized = true;
        std::cout << "Audio Engine Initialized Successfully." << std::endl;
    }

    void AudioContext::Cleanup() {
        if (!state.isInitialized) return;
        
        ma_engine_uninit(&state.engine);
        state.isInitialized = false;
        std::cout << "Audio Engine Cleaned Up." << std::endl;
    }

} // namespace audio
