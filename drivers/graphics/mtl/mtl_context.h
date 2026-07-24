#pragma once

#ifndef __MTL_CONTEXT_H__
#define __MTL_CONTEXT_H__

namespace mtl {

    struct MTLContextState {
        // Menggunakan tipe void* di sini.
        // Tujuannya agar file C++ murni yang melakukan #include header ini
        // tidak perlu mengaktifkan compiler Objective-C++ / meng-include <Metal/Metal.h>.
        void* device = nullptr;
        void* commandQueue = nullptr;
    };

    class MTLContext {
    private:
        MTLContextState state;

    public:
        void Initialize(void* windowHandle = nullptr);
        
        const MTLContextState& GetState() const { return state; }
    };
}

#endif // __MTL_CONTEXT_H__
