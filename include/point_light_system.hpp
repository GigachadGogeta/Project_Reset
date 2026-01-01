#ifndef __POINT_LIGHT_SYSTEM_HPP__
#define __POINT_LIGHT_SYSTEM_HPP__

#include <device.hpp>
#include <frame_info.hpp>
#include <pipeline.hpp>

namespace engine {

class PointLightSystem {
public:
    PointLightSystem(Device& deviceRef, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~PointLightSystem();

    PointLightSystem(const PointLightSystem &) = delete;
    PointLightSystem& operator=(const PointLightSystem &) = delete;

    void update(FrameInfo& frameInfo, GlobalUbo& ubo);
    void render(FrameInfo& frameInfo);

private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    Device& device;

    std::unique_ptr<Pipeline> pipeline;
    VkPipelineLayout pipelineLayout;
};

} // namespace engine

#endif