cmake_minimum_required(VERSION 4.2)
project (Minecraft
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

set(glm_VERSION 1.0.3)
set(glm_URL_HASH SHA256=3a8815d60cce4bd0a40b3a7380dc23c2831470bdcdaea8089ba86a28510d6ee4)
FetchContent_Declare(
    glm
    URL https://github.com/g-truc/glm/releases/download/${glm_VERSION}/glm-${glm_VERSION}.zip
    URL_HASH ${glm_URL_HASH}
    FIND_PACKAGE_ARGS ${glm_VERSION}
)

set(glfw_VERSION 3.4)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    glfw
    URL https://github.com/glfw/glfw/releases/download/${glfw_VERSION}/glfw-${glfw_VERSION}.zip
    FIND_PACKAGE_ARGS ${glfw_VERSION}
)

FetchContent_MakeAvailable(glm glfw)

find_package(Vulkan REQUIRED)
if(Vulkan_FOUND)
    message(STATUS "Found Vulkan: ${Vulkan_INCLUDE_DIRS}")
    include_directories(${Vulkan_INCLUDE_DIRS})
    link_directories(${Vulkan_LIBRARY_DIRS})
endif()

find_package(Threads REQUIRED)

set(LIBRARIES
    glm
    glfw
    Vulkan::Vulkan
    Threads::Threads
)

set(SOURCE_FILES
    src/app.cpp
    src/buffer.cpp
    src/camera.cpp
    src/descriptors.cpp
    src/device.cpp
    src/game_object.cpp
    src/keyboard_movement_controller.cpp
    src/main.cpp
    src/model.cpp
    src/pipeline.cpp
    src/point_light_system.cpp
    src/render_system.cpp
    src/renderer.cpp
    src/swap_chain.cpp
    src/window.cpp
)

set(INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_compile_options(${PROJECT_NAME} PUBLIC -std=c++17)

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRECTORIES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBRARIES})

set(SHADERS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(SHADERS_DEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")

set(MODELS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/models")
set(MODELS_DEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/models")

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND cmd /c "${CMAKE_CURRENT_SOURCE_DIR}/compile_shaders.bat"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Executing post-build batch script to compile shaders..."
)

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${SHADERS_SOURCE_DIR}"
    "${SHADERS_DEST_DIR}"
    COMMENT "Copying 'shaders' to build directory"
)
add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${MODELS_SOURCE_DIR}"
    "${MODELS_DEST_DIR}"
    COMMENT "Copying 'models' to build directory"
)

### For distribution
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
install(DIRECTORY
    ${CMAKE_SOURCE_DIR}/shaders/ DESTINATION bin/shaders
)
install(DIRECTORY
    ${CMAKE_SOURCE_DIR}/models/ DESTINATION bin/models
)