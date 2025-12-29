#include "swap_chain.hpp"
#include <app.hpp>
#include <memory>

namespace engine {

App::App() {
    recreateSwapChain();
}

App::~App() {
    // destroy pipeline layout soon
}

void App::run() {
    mainLoop();
    vkDeviceWaitIdle(device.getLogicalDevice());
}

void App::mainLoop() {
    while (!window.shouldClose()) {
        glfwPollEvents();
        // drawFrame();
    }
}

void App::recreateSwapChain() {
    VkExtent2D extent = window.getExtent();
    while (extent.width == 0 || extent.height == 0) {
        extent = window.getExtent();
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device.getLogicalDevice());

    if (swapChain == nullptr) {
        swapChain = std::make_unique<SwapChain>(device, extent);
    }
    else if (swapChain != nullptr) {
        swapChain = std::make_unique<SwapChain>(device, extent, std::move(swapChain));
        // if image count not same as command buffer size, then recreate command buffer (free -> create)
    }
    // create pipeline
}

} // namespace engine