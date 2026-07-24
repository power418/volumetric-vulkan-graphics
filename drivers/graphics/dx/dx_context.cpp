#include "dx_context.h"

#include <cstdlib>
#include <utils/logger.h>

namespace dx {

#if defined(_WIN32) || defined(_WIN64)
    void DXContext::Initialize(int preferredGpuIndex) {
        UINT dxgiFactoryFlags = 0;
        
#if defined(_DEBUG)
        // Enable D3D12 debug layer
        Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
            debugController->EnableDebugLayer();
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
#endif

        CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&state.factory));

        Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
        if (state.factory->EnumAdapters1(preferredGpuIndex, &adapter) == DXGI_ERROR_NOT_FOUND) {
            WR_LOG_WARN("[DX] GPU index " << preferredGpuIndex << " not found! Fallback to 0.");
            state.factory->EnumAdapters1(0, &adapter);
        }

        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);
        
        size_t converted = 0;
        // Hitung ukuran yang dibutuhkan
        wcstombs_s(&converted, nullptr, 0, desc.Description, _TRUNCATE);
        
        // Gunakan malloc sesuai request
        char* descStr = (char*)std::malloc(converted + 1);
        if (descStr) {
            wcstombs_s(&converted, descStr, converted + 1, desc.Description, _TRUNCATE);
            WR_LOG_INFO("[DX] Selected GPU: " << descStr);
            std::free(descStr);
        }
        
        // Create D3D12 Device
        if (FAILED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&state.device)))) {
            WR_LOG_ERR("[DX] Failed to create D3D12 Device!");
            return;
        }

        // Create Command Queue
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        state.device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&state.commandQueue));
        
        WR_LOG_INFO("[DX] D3D12 Context initialized successfully.");
    }
#else
    void DXContext::Initialize(int preferredGpuIndex) {
        WR_LOG_INFO("[DX] Initializing VKD3D on native Linux... (GPU Index: " << preferredGpuIndex << ")");
        
        struct vkd3d_instance_create_info instance_info = {};
        instance_info.type = VKD3D_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        // next di set ke nullptr otomatis oleh inisialisasi {}
        
        // Buat instance VKD3D
        // vkd3d_create_instance(&instance_info, &instance);
        
        WR_LOG_INFO("[DX] VKD3D Context initialization is WIP.");
    }
#endif

}

