#include <app.hpp>

namespace engine {

App::App() {
// nothing YET
}

App::~App() {
    // destroy pipeline layout soon
}

void App::run() {
    initVulkan(); // placeholder (see declaraion)
    mainLoop();

    //vkDeviceWaitIdle( device here );
}

void App::initVulkan() {
    
}

void App::mainLoop() {
    while (!window.shouldClose()) {
        glfwPollEvents();
    }
}

} // namespace engine