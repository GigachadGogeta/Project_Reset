#include <app.hpp>

#include <cstddef>
#include <memory>
#include <cassert>
#include <stdexcept>
#include <array>

namespace engine {

App::App() {
    createPipelineLayout();
    recreateSwapChain();
    createCommandBuffers();
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
        drawFrame();
    }
}

void App::createCommandBuffers() {
    commandBuffers.resize(swapChain->imageCount());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType                 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool           = device.getCommandPool();
    allocInfo.level                 = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount    = static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(device.getLogicalDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers!");
    }
}

void App::freeCommandBuffers() {
    vkFreeCommandBuffers(
        device.getLogicalDevice(),
        device.getCommandPool(),
        static_cast<uint32_t>(commandBuffers.size()),
        commandBuffers.data()
    );
    commandBuffers.clear();
}

void App::drawFrame() {
    size_t currentFrame = swapChain->getCurrentFrame();

    uint32_t imageIndex;
    VkResult result = swapChain->acquireNextImage(&imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    vkResetCommandBuffer(commandBuffers[currentFrame], 0);
    recordCommandBuffer(imageIndex);

    result = swapChain->submitCommandBuffers(&commandBuffers[currentFrame], &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.wasWindowResized()) {
        window.resetWindowResizedFlag();
        recreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swap chain image!");
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
        if (swapChain->imageCount() != commandBuffers.size()) {
            freeCommandBuffers();
            createCommandBuffers();
        }
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
    size_t currentFrame = swapChain->getCurrentFrame();

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType             = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags             = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT; // Optional // Review
    beginInfo.pInheritanceInfo  = nullptr; // Optional

    if (vkBeginCommandBuffer(commandBuffers[currentFrame], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType                = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass           = swapChain->getRenderPass();
    renderPassInfo.framebuffer          = swapChain->getFrameBuffer(imageIndex);
    renderPassInfo.renderArea.offset    = {0, 0};
    renderPassInfo.renderArea.extent    = swapChain->getSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color            = {0.47f, 0.66f, 1.0f, 1.0f};
    clearValues[1].depthStencil     = {1.0f, 0};
    renderPassInfo.clearValueCount  = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues     = clearValues.data();

    vkCmdBeginRenderPass(commandBuffers[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getGraphicsPipeline());

    VkViewport viewport{};
    viewport.x          = 0.0f;
    viewport.y          = 0.0f;
    viewport.width      = static_cast<float>(swapChain->getSwapChainExtent().width);
    viewport.height     = static_cast<float>(swapChain->getSwapChainExtent().height);
    viewport.minDepth   = 0.0f;
    viewport.maxDepth   = 1.0f;
    vkCmdSetViewport(commandBuffers[currentFrame], 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChain->getSwapChainExtent();
    vkCmdSetScissor(commandBuffers[currentFrame], 0, 1, &scissor);

    // model->bind(commandBuffers[currentFrame]);
    // model->draw(commandBuffers[currentFrame]);
    vkCmdDraw(commandBuffers[currentFrame], 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffers[currentFrame]);

    if (vkEndCommandBuffer(commandBuffers[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer!");
    }
}

} // namespace engine