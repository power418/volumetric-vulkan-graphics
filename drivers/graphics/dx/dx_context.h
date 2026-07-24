#pragma once

#ifndef __WIN32_PLATFORM_DX_CONTEXT_H__
#define __WIN32_PLATFORM_DX_CONTEXT_H__

#ifndef NOMINMAX
#define NOMINMAX
#endif

#if defined(_WIN32) || defined(_WIN64)
#include <d3d12.h>
#include <dxgi1_6.h>
#include <windows.h>
#include <wrl/client.h>
#else
#include <vkd3d.h>
#include <vkd3d_d3d12.h>
#endif

namespace dx {
    
    struct DXContextState {
#if defined(_WIN32) || defined(_WIN64)
        Microsoft::WRL::ComPtr<ID3D12Device> device;
        Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
        Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
#else
        ID3D12Device* device = nullptr;
        void* factory = nullptr; // VKD3D native doesn't use DXGIFactory natively
        ID3D12CommandQueue* commandQueue = nullptr;
        void* swapChain = nullptr; // VKD3D native uses Vulkan Surface
#endif
    };

    class DXContext {
    private:
        DXContextState state;
        
    public:
        // Parameter preferredGpuIndex dari command line argument
        void Initialize(int preferredGpuIndex = 0);
        
        // Contoh getter sederhana
#if defined(_WIN32) || defined(_WIN64)
        ID3D12Device* GetDevice() const { return state.device.Get(); }
#else
        ID3D12Device* GetDevice() const { return state.device; }
#endif
    };
}

#endif // __WIN32_PLATFORM_DX_CONTEXT_H__
