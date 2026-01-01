#ifndef __APP_HPP__
#define __APP_HPP__

#include <descriptors.hpp>
#include <window.hpp>
#include <device.hpp>
#include <renderer.hpp>
#include <game_object.hpp>

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
    void loadGameObjects();

    Window window{WIDTH, HEIGHT, "Voxel Engine"};
    Device device{window};
    Renderer renderer{window, device};

    // note: order of declarations matters
    std::unique_ptr<DescriptorPool> globalPool{};
    GameObject::Map gameObjects;
};

} // namespace engine

#endif