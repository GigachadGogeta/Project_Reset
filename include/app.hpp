#ifndef __APP_HPP__
#define __APP_HPP__

#include <window.hpp>
#include <device.hpp>
#include <swap_chain.hpp>
#include <pipeline.hpp>

#include <memory>
#include <cstdint>

namespace engine{

class App {
public:
    static constexpr uint32_t WIDTH = 800;
    static constexpr uint32_t HEIGHT = 600;

    App();
    ~App();

    // Not copyable
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    void run();
private:
    void mainLoop();
    void createPipelineLayout();
    void createPipeline();

    void recreateSwapChain();
    void recordCommandBuffer(int imageIndex);

    Window window{WIDTH, HEIGHT, "Hello Vulkan Window"};
    Device device{window};
    std::unique_ptr<SwapChain> swapChain;
    std::unique_ptr<Pipeline> pipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<VkCommandBuffer> commandBuffers;
};

} // namespace engine

#endif