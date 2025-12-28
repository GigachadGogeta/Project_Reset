#ifndef __APP_HPP__
#define __APP_HPP__

#include <window.hpp>
#include <device.hpp>

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
    void initVulkan(); // placeholder that might be removed if pipeline components can be objectized
    void mainLoop();

    Window window{WIDTH, HEIGHT, "Hello Vulkan Window"};
    Device device{};
};

} // namespace engine

#endif