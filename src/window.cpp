#include <window.hpp>

#include <stdexcept>
#include <cstdint>

namespace engine {

void Window::framebufferResizedCallback(GLFWwindow *glfwWindow, int width, int height) {
    Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
    window->framebufferResized = true;
    window->width = width;
    window->height = height;
}

Window::Window(uint32_t w, uint32_t h, std::string name) : width{w}, height{h}, windowName{name} {
    initWindow();
}

Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizedCallback);
}

void Window::createSurface(VkInstance& instance, VkSurfaceKHR& surface) {
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface!");
    }
}

} // namespace engine