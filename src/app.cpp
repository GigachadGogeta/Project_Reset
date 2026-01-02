#include <app.hpp>
#include <keyboard_movement_controller.hpp>
#include <buffer.hpp>
#include <render_system.hpp>
#include <point_light_system.hpp>
#include <camera.hpp>
#include <frame_info.hpp>
#include <descriptors.hpp>

#include <memory>
#include <cassert>
#include <chrono>

namespace engine {

App::App() {
    globalPool = 
        DescriptorPool::Builder(device)
            .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();
    loadGameObjects();
}

App::~App() {}

void App::run() {
    std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < uboBuffers.size(); i++) {
        uboBuffers[i] = std::make_unique<Buffer>(
            device,
            sizeof(GlobalUbo),
            1,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        uboBuffers[i]->map();
    }
    std::unique_ptr<DescriptorSetLayout> globalSetLayout = 
        DescriptorSetLayout::Builder(device)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .build();

    std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < globalDescriptorSets.size(); i++) {
        VkDescriptorBufferInfo bufferInfo = uboBuffers[i]->createDescriptorBufferInfo();
        DescriptorWriter(*globalSetLayout, *globalPool)
            .writeBuffer(0, &bufferInfo)
            .build(globalDescriptorSets[i]);
    }

    RenderSystem renderSystem{
        device,
        renderer.getSwapChainRenderPass(),
        globalSetLayout->getDescriptorSetLayout()
    };

    PointLightSystem pointLightSystem{
        device,
        renderer.getSwapChainRenderPass(),
        globalSetLayout->getDescriptorSetLayout()
    };
    
    Camera camera{};

    GameObject viewerObject = GameObject::createGameObject();
    viewerObject.transform.translation.z = -2.5f;
    KeyboardMovementController cameraController{};

    std::chrono::time_point currentTime = std::chrono::high_resolution_clock::now();

    double lastMouseX = window.getCursorX();
    double lastMouseY = window.getCursorY();

    while (!window.shouldClose()) {
        glfwPollEvents();

        std::chrono::time_point newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        // Get current mouse position
        double mouseX = window.getCursorX();
        double mouseY = window.getCursorY();

        // Calculate delta (change) from last frame
        float cursor_dx = static_cast<float>(mouseX - lastMouseX);
        float cursor_dy = static_cast<float>(mouseY - lastMouseY);

        // Update last position for next frame
        lastMouseX = mouseX;
        lastMouseY = mouseY;

        cameraController.moveInPlaneXZ(window.getGLFWwindow(), frameTime, viewerObject, cursor_dx, cursor_dy);
        camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

        float aspect = renderer.getAspectRatio();
        camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

        if (VkCommandBuffer commandBuffer = renderer.beginFrame()) {
            int frameIndex = renderer.getFrameIndex();
            FrameInfo frameInfo{
                frameIndex,
                frameTime,
                commandBuffer,
                camera,
                globalDescriptorSets[frameIndex],
                gameObjects
            };

            // update
            GlobalUbo ubo{};
            ubo.projection  = camera.getProjection();
            ubo.view        = camera.getView();
            ubo.inverseView = camera.getInverseView();
            pointLightSystem.update(frameInfo, ubo);
            uboBuffers[frameIndex]->writeToBuffer(&ubo);
            uboBuffers[frameIndex]->flush();

            // render
            renderer.beginSwapChainRenderPass(commandBuffer);

            // order here matters
            renderSystem.renderGameObjects(frameInfo);
            pointLightSystem.render(frameInfo);

            renderer.endSwapChainRenderPass(commandBuffer);
            renderer.endFrame();
        }
    }
    vkDeviceWaitIdle(device.getLogicalDevice());
}

void App::loadGameObjects() {
    std::shared_ptr<Model> model = Model::createModelFromFile(device, "models/smooth_vase.obj");
    GameObject smoothVase = GameObject::createGameObject();
    smoothVase.model = model;
    smoothVase.transform.translation = {.5f, .5f, 0.f};
    smoothVase.transform.scale = {3.f, 1.5f, 3.f};

    smoothVase.transform.rotation = {
        glm::mod(smoothVase.transform.rotation.x + 0.0f, glm::two_pi<float>()),
        glm::mod(smoothVase.transform.rotation.y + 0.0f, glm::two_pi<float>()),
        glm::mod(smoothVase.transform.rotation.z + 0.001f, glm::two_pi<float>()),
    };

    gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

    model = Model::createModelFromFile(device, "models/colored_cube.obj");
    GameObject cube = GameObject::createGameObject();
    cube.model = model;
    cube.transform.translation = {-4.f, -4.f, 4.f};
    cube.transform.scale = {1.f, 1.f, 1.f};

    cube.transform.rotation = {
        glm::mod(cube.transform.rotation.x + 0.0f, glm::two_pi<float>()),
        glm::mod(cube.transform.rotation.y + 0.0f, glm::two_pi<float>()),
        glm::mod(cube.transform.rotation.z + 0.001f, glm::two_pi<float>()),
    };

    gameObjects.emplace(cube.getId(), std::move(cube));

    std::vector<glm::vec3> lightColors{
        {1.f, 1.f, 1.f},
    };

    for (int i = 0; i < lightColors.size(); i++) {
        GameObject pointLight = GameObject::makePointLight(1.f, 1.f);
        pointLight.color = lightColors[i];
        auto rotateLight = glm::rotate(
            glm::mat4(1.f),
            (i * glm::two_pi<float>()) / lightColors.size(),
            {0.f, -1.f, 0.f});
        pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
        gameObjects.emplace(pointLight.getId(), std::move(pointLight));
    }
}

} // namespace engine