#include "gl_context.h"

#include <utils/logger.h> 

namespace gl {

    void GLContext::Initialize(void* windowHandle) {
        WR_LOG_INFO("[GL] Initializing OpenGL Context...");
        
#if defined(_WIN32) || defined(_WIN64)
        if (!windowHandle) {
            WR_LOG_WARN("[GL] Window HWND pointer is null, cannot initialize WGL properly!");
            return;
        }
        
        HWND hwnd = static_cast<HWND>(windowHandle);
        state.deviceContext = GetDC(hwnd);
        if (!state.deviceContext) {
            WR_LOG_ERR("[GL] Failed to get Device Context (DC) from HWND.");
            return;
        }
        
        PIXELFORMATDESCRIPTOR pfd = {};
        pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cDepthBits = 24;
        pfd.cStencilBits = 8;
        pfd.iLayerType = PFD_MAIN_PLANE;
        
        int pixelFormat = ChoosePixelFormat(state.deviceContext, &pfd);
        if (!pixelFormat) {
            WR_LOG_ERR("[GL] Failed to find a suitable pixel format.");
            return;
        }
        
        if (!SetPixelFormat(state.deviceContext, pixelFormat, &pfd)) {
            WR_LOG_ERR("[GL] Failed to set pixel format.");
            return;
        }
        
        state.renderContext = wglCreateContext(state.deviceContext);
        if (!state.renderContext) {
            WR_LOG_ERR("[GL] Failed to create WGL Context.");
            return;
        }
        
        wglMakeCurrent(state.deviceContext, state.renderContext);
        WR_LOG_INFO("[GL] WGL Context initialized successfully.");
        
#else
        if (!windowHandle) {
            WR_LOG_WARN("[GL] X11 Display pointer is null, cannot initialize GLX properly!");
            return;
        }
        
        state.display = static_cast<Display*>(windowHandle);
        
        // Basic GLX setup (assuming GLX version 1.2+ is supported)
        int dummy;
        if (!glXQueryExtension(state.display, &dummy, &dummy)) {
            WR_LOG_ERR("[GL] GLX is not supported on this X11 display!");
            return;
        }
        
        // TODO: Full GLX window & context creation logic here
        // usually requires XVisualInfo and glXCreateContext...
        
        WR_LOG_INFO("[GL] GLX Context initialization is WIP.");
#endif
    }
    
    void GLContext::MakeCurrent() {
#if defined(_WIN32) || defined(_WIN64)
        if (state.deviceContext && state.renderContext) {
            wglMakeCurrent(state.deviceContext, state.renderContext);
        }
#else
        // glXMakeCurrent(state.display, window, state.renderContext);
#endif
    }
    
    void GLContext::SwapBuffers() {
#if defined(_WIN32) || defined(_WIN64)
        if (state.deviceContext) {
            ::SwapBuffers(state.deviceContext);
        }
#else
        // glXSwapBuffers(state.display, window);
#endif
    }
}
