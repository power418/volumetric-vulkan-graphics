#include "vk_context.h"
#include <utils/logger.h>
#include <vector>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

namespace vk {

    void VKContext::Initialize(const char* appName) {
        WR_LOG_INFO("[VK] Initializing Vulkan Context...");
        
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = appName ? appName : "Volumetric Engine";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Volumetric Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        
        std::vector<const char*> extensions;
        // Tambahkan ekstensi window surface seperti VK_KHR_surface dll nanti di sini.
        
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.empty() ? nullptr : extensions.data();

#if defined(_DEBUG) || !defined(NDEBUG)
        const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
#else
        createInfo.enabledLayerCount = 0;
#endif

        if (vkCreateInstance(&createInfo, nullptr, &state.instance) != VK_SUCCESS) {
            WR_LOG_ERR("[VK] Failed to create Vulkan Instance!");
            return;
        }
        
        WR_LOG_INFO("[VK] Vulkan Instance created successfully.");
        
        // Setup placeholder untuk VulkanMemoryAllocator (VMA).
        // Ini nantinya memerlukan state.physicalDevice dan state.device yang valid!
        // VmaAllocatorCreateInfo allocatorInfo = {};
        // allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;
        // allocatorInfo.physicalDevice = state.physicalDevice;
        // allocatorInfo.device = state.device;
        // allocatorInfo.instance = state.instance;
        // vmaCreateAllocator(&allocatorInfo, &state.allocator);
        
        WR_LOG_INFO("[VK] Vulkan Context initialization is WIP (Pending Physical Device/VMA).");
    }
    
    void VKContext::Cleanup() {
        // Hapus VMA terlebih dahulu jika sudah terinisialisasi
        if (state.allocator != VK_NULL_HANDLE) {
            vmaDestroyAllocator(state.allocator);
            state.allocator = VK_NULL_HANDLE;
            WR_LOG_INFO("[VK] VMA Allocator destroyed.");
        }
        
        if (state.instance != VK_NULL_HANDLE) {
            vkDestroyInstance(state.instance, nullptr);
            state.instance = VK_NULL_HANDLE;
            WR_LOG_INFO("[VK] Vulkan Instance destroyed.");
        }
    }
}
