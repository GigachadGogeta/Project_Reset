#ifndef __KEYBOARD_MOVEMENT_CONTROLLER_HPP__
#define __KEYBOARD_MOVEMENT_CONTROLLER_HPP__

#include <game_object.hpp>
#include <window.hpp>

namespace engine {

class KeyboardMovementController {
public:
    struct KeyMappings {
        int moveLeft = GLFW_KEY_A;
        int moveRight = GLFW_KEY_D;
        int moveForward = GLFW_KEY_W;
        int moveBackward = GLFW_KEY_S;
        int moveUp = GLFW_KEY_SPACE;
        int moveDown = GLFW_KEY_LEFT_SHIFT;
    };

    void moveInPlaneXZ(GLFWwindow* glfwWindow, float dt, GameObject& gameObject, float cursor_dx, float cursor_dy);

    KeyMappings keys{};
    float moveSpeed{3.f};

    float lookSpeed{0.005f};
};

} // namespace engine

#endif