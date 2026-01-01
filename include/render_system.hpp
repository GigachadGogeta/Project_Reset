#ifndef __RENDER_SYSTEM_HPP__
#define __RENDER_SYSTEM_HPP__

#include <device.hpp>
#include <pipeline.hpp>
#include <frame_info.hpp>

#include <vulkan/vulkan.h>

#include <memory>

namespace engine {

class RenderSystem {
public:
    RenderSystem(Device& deviceRef, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~RenderSystem();

    RenderSystem(const RenderSystem&) = delete;
    RenderSystem& operator=(const RenderSystem&) = delete;

    void renderGameObjects(FrameInfo& frameInfo);

private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    Device& device;

    std::unique_ptr<Pipeline> pipeline;
    VkPipelineLayout pipelineLayout;
};

} // namespace engine

#endif