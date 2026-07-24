#pragma once

#ifndef __AUDIO_CONTEXT_H__
#define __AUDIO_CONTEXT_H__

// Miniaudio header
#include <miniaudio.h>

namespace audio {

    struct AudioContextState {
        ma_engine engine;
        bool isInitialized = false;
    };

    class AudioContext {
    private:
        AudioContextState state;

    public:
        void Initialize();
        void Cleanup();

        const AudioContextState& GetState() const { return state; }
    };

} // namespace audio

#endif // __AUDIO_CONTEXT_H__
