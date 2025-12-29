#include "pipeline.hpp"
#include "swap_chain.hpp"
#include <app.hpp>

#include <memory>
#include <cassert>
#include <stdexcept>

namespace engine {

App::App() {
    createPipelineLayout();
    recreateSwapChain();
}

App::~App() {
    vkDestroyPipelineLayout(device.getLogicalDevice(), pipelineLayout, nullptr);
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
    createPipeline();
}

void App::createPipelineLayout() {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType                    = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount           = 0;        // Optional
    pipelineLayoutInfo.pSetLayouts              = nullptr;  // Optional
    pipelineLayoutInfo.pushConstantRangeCount   = 0;        // Optional
    pipelineLayoutInfo.pPushConstantRanges      = nullptr;  // Optional

    if (vkCreatePipelineLayout(device.getLogicalDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout!");
    }
}

void App::createPipeline() {
    assert(swapChain != nullptr && "Cannot create pipeline before swap chain");
    assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    Pipeline::defaultPipelineConfigInfo(pipelineConfig);
    
    pipelineConfig.renderPass = swapChain->getRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout;
    pipeline = std::make_unique<Pipeline>(
        device, 
        "shaders/shader.vert.spv", 
        "shaders/shader.frag.spv", 
        pipelineConfig
    );
}

void App::recordCommandBuffer(int imageIndex) {
    VkViewport viewport{};
    viewport.x          = 0.0f;
    viewport.y          = 0.0f;
    viewport.width      = static_cast<float>(swapChain->getSwapChainExtent().width);
    viewport.height     = static_cast<float>(swapChain->getSwapChainExtent().height);
    viewport.minDepth   = 0.0f;
    viewport.maxDepth   = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChain->getSwapChainExtent();
}

} // namespace engine