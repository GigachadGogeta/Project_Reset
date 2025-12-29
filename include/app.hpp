#ifndef __APP_HPP__
#define __APP_HPP__

#include <swap_chain.hpp>
#include <window.hpp>
#include <device.hpp>

#include <memory>
#include <cstdint>

namespace engine{

class App {
public:
    static constexpr uint32_t WIDTH = 800;
    static constexpr uint32_t HEIGHT = 600;

    App();
    ~App();

    App(const App&) = delete;
    App& operator=(const App&) = delete;

    void run();
private:
    void mainLoop();

    void recreateSwapChain();

    Window window{WIDTH, HEIGHT, "Hello Vulkan Window"};
    Device device{window};
    std::unique_ptr<SwapChain> swapChain;
};

} // namespace engine

#endif