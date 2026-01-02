#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdint>
#include <string>

namespace engine {

class Window {
public:
    Window(uint32_t w, uint32_t h, std::string name);
    ~Window();
    
    // Not copyable
    Window() = delete;
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool shouldClose() {return glfwWindowShouldClose(window);}

    VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }
    GLFWwindow* getGLFWwindow() { return window; }

    bool wasWindowResized() {return framebufferResized;}
    void resetWindowResizedFlag() {framebufferResized = false;}

    void createSurface(VkInstance& instance, VkSurfaceKHR& surface);

    double getCursorX() const { return cursorX; }
    double getCursorY() const { return cursorY; }
private:
    static void framebufferResizedCallback(GLFWwindow* glfwWindow, int width, int height);

    static void cursor_position_callback(GLFWwindow* glfwWindow, double xpos, double ypos);

    void initWindow();
    
    uint32_t width;
    uint32_t height;

    double cursorX {0};
    double cursorY {0};

    bool framebufferResized {false};
    
    std::string windowName;
    GLFWwindow* window;
};

} // namespace engine

#endif