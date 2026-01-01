#ifndef __FRAME_INFO_HPP__
#define __FRAME_INFO_HPP__

#include <game_object.hpp>
#include <camera.hpp>

#define GLM_FORCE_CXX17
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vulkan/vulkan.h>

namespace engine {

#define MAX_LIGHTS 10

struct PointLight {
    glm::vec4 position{};  // ignore w
    glm::vec4 color{};     // w is intensity
};

struct GlobalUbo {
    glm::mat4 projection{1.f};
    glm::mat4 view{1.f};
    glm::mat4 inverseView{1.f};
    glm::vec4 ambientLightColor{1.f, 1.f, 1.f, .02f};  // w is intensity
    PointLight pointLights[MAX_LIGHTS];
    int numLights;
};

struct FrameInfo {
    int frameIndex;
    float frameTime;
    VkCommandBuffer commandBuffer;
    Camera &camera;
    VkDescriptorSet globalDescriptorSet;
    GameObject::Map &gameObjects;
};

} // namespace engine

#endif