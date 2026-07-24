#include "mtl_context.h"
#include <utils/logger.h>

#if defined(__APPLE__) || defined(USE_GNUSTEP)
#import <Metal/Metal.h>
#endif

namespace mtl {

    void MTLContext::Initialize(void* windowHandle) {
        WR_LOG_INFO("[MTL] Initializing Metal Context...");
        
#if defined(__APPLE__)
        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        if (!device) {
            WR_LOG_ERR("[MTL] Metal is not supported on this device.");
            return;
        }
        
        // Simpan instance ke dalam void* (tipe jembatan ObjC/C++)
#if __has_feature(objc_arc)
        state.device = (__bridge_retained void*)device;
#else
        state.device = (void*)device;
#endif
        
        id<MTLCommandQueue> commandQueue = [device newCommandQueue];
#if __has_feature(objc_arc)
        state.commandQueue = (__bridge_retained void*)commandQueue;
#else
        state.commandQueue = (void*)commandQueue;
#endif
        
        WR_LOG_INFO("[MTL] Metal Context initialized successfully.");
#else
        WR_LOG_WARN("[MTL] Metal Context is only natively supported on Apple platforms!");
        // Placeholder untuk GNUstep di Linux/Windows jika Metal backend tersedia
#endif
    }
}
