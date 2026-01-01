#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include <window.hpp>
#include <device.hpp>
#include <swap_chain.hpp>

#include <memory>
#include <cassert>

namespace engine {

class Renderer {
public:
    Renderer(Window& windowRef, Device& deviceRef);
    ~Renderer();

    Renderer() = delete;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    // The render pass and swapchain extent are often needed by pipelines or models
    VkRenderPass getSwapChainRenderPass() const { 
        return swapChain->getRenderPass(); 
    }

    float getAspectRatio() const { return swapChain->extentAspectRatio(); }
    bool isFrameInProgress() const { return isFrameStarted; }

    VkCommandBuffer getCurrentCommandBuffer() const {
        assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
        return commandBuffers[currentFrameIndex];
    }

    // Getter used by App or other systems to know current frame index
    // (useful for dynamic uniform buffers)
    int getFrameIndex() const { 
        assert(isFrameStarted && "Cannot get frame index when frame not in progress");
        return currentFrameIndex; 
    }

    // Returns the command buffer to record into for the current frame
    VkCommandBuffer beginFrame();

    // Submits the command buffer and presents the image
    void endFrame();

    // Helper to begin the render pass
    void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    
    // Helper to end the render pass
    void endSwapChainRenderPass(VkCommandBuffer commandBuffer);
    
private:
    void createCommandBuffers();
    void freeCommandBuffers();
    void recreateSwapChain();

    Window& window;
    Device& device;

    std::unique_ptr<SwapChain> swapChain;
    
    std::vector<VkCommandBuffer> commandBuffers;
    
    uint32_t currentImageIndex;
    int currentFrameIndex{0};
    bool isFrameStarted{false};
};

} // namespace engine

#endif