#pragma once

#ifndef __GL_CONTEXT_H__
#define __GL_CONTEXT_H__

#if defined(_WIN32) || defined(_WIN64)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <GL/gl.h>
#else
// X11 / GLX untuk Linux
#include <GL/gl.h>
#include <GL/glx.h>
#endif

namespace gl {

    struct GLContextState {
#if defined(_WIN32) || defined(_WIN64)
        HGLRC renderContext = nullptr;
        HDC deviceContext = nullptr;
#else
        GLXContext renderContext = nullptr;
        Display* display = nullptr;
#endif
    };

    class GLContext {
    private:
        GLContextState state;

    public:
        // Initialize context. 
        // Windows expects an HWND casted to void*. 
        // Linux expects a Display* casted to void*.
        void Initialize(void* windowHandle = nullptr);
        
        void MakeCurrent();
        void SwapBuffers();
        
        const GLContextState& GetState() const { return state; }
    };
}

#endif // __GL_CONTEXT_H__
