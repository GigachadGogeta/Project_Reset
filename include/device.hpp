#ifndef __DEVICE_HPP__
#define __DEVICE_HPP__

#include <vector>
#include <vulkan/vulkan.h>

namespace engine {

struct QueueFamilyIndices {
    uint32_t graphicsFamily;
};

class Device {
public:
    #ifdef NDEBUG
        const bool enableValidationLayers = false;
    #else
        const bool enableValidationLayers = true;
    #endif

    Device();
    ~Device();

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;
    Device(Device&&) = delete;
    Device& operator=(Device&&) = delete;
private:
    void createInstance();
    void setupDebugMessenger();
    void pickPhysicalDevice();

    // Helper functions
    bool isDeviceSuitable (VkPhysicalDevice device);
    std::vector<const char*> getRequiredExtensions();
    bool checkValidationLayerSupport();
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void hasGflwRequiredInstanceExtensions();

    const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

};

} // namespace engine

#endif