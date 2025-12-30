cmake_minimum_required(VERSION 4.2)
project (Minecraft
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

set(glm_VERSION 1.0.2)
set(glm_URL_HASH SHA256=a41adf423315e35489c1e67689c7bb74c7edd4d207a477b7b19c45cd440c2f67)
FetchContent_Declare(
    glm
    URL https://github.com/g-truc/glm/releases/download/${glm_VERSION}/glm-${glm_VERSION}.zip
    URL_HASH ${glm_URL_HASH}
    FIND_PACKAGE_ARGS ${glm_VERSION}
)

set(glfw_VERSION 3.4)
FetchContent_Declare(
    glfw
    URL https://github.com/glfw/glfw/releases/download/${glfw_VERSION}/glfw-${glfw_VERSION}.zip
    FIND_PACKAGE_ARGS ${glfw_VERSION}
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glm glfw)

find_package(Vulkan REQUIRED)
if(Vulkan_FOUND)
    message(STATUS "Found Vulkan: ${Vulkan_INCLUDE_DIRS}")
    include_directories(${Vulkan_INCLUDE_DIRS})
    link_directories(${Vulkan_LIBRARY_DIRS})
endif()

find_package(Threads)

set(LIBRARIES
    glm 
    glfw 
    Vulkan::Vulkan 
    Threads::Threads
)

set(SOURCE_FILES
    src/main.cpp
    src/app.cpp
    src/window.cpp
    src/device.cpp
    src/swap_chain.cpp
    src/pipeline.cpp
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

add_custom_command(
    TARGET ${PROJECT_NAME} # Replace 'your_target_name' with your actual executable/library name
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${SHADERS_SOURCE_DIR}"
    "${SHADERS_DEST_DIR}"
    COMMENT "Copying 'shaders' to build directory"
)

### For distribution
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
install(DIRECTORY
    ${CMAKE_SOURCE_DIR}/shaders/ DESTINATION bin/shaders
)