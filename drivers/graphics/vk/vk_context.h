#pragma once

#ifndef __VK_CONTEXT_H__
#define __VK_CONTEXT_H__

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace vk {

    struct VKContextState {
        VkInstance instance = VK_NULL_HANDLE;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;
        VkQueue graphicsQueue = VK_NULL_HANDLE;
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VmaAllocator allocator = VK_NULL_HANDLE;
    };

    class VKContext {
    private:
        VKContextState state;

    public:
        void Initialize(const char* appName = "Volumetric Engine");
        void Cleanup();
        
        const VKContextState& GetState() const { return state; }
    };
}

#endif // __VK_CONTEXT_H__
