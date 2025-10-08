# Minimum required CMake version
cmake_minimum_required(VERSION 3.28)

# Define the project name
project(opengl-cmake-starter-project LANGUAGES C CXX)

# Define GLEW as static library
add_definitions(-DGLEW_STATIC)

# Set C++ standard and options
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined -g")

# Fetch external dependencies using FetchContent
include(FetchContent)

# Fetch GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG "3.4"
)
FetchContent_MakeAvailable(glfw)

# Fetch GLEW (using omniavinco/glew-cmake fork)
FetchContent_Declare(
  glew
  GIT_REPOSITORY https://github.com/omniavinco/glew-cmake.git
  GIT_TAG "glew-cmake-2.2.0"
)
FetchContent_MakeAvailable(glew)

# Fetch GLM
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG "1.0.1"
)
FetchContent_MakeAvailable(glm)

# Fetch ImGui
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG "v1.92.3-docking"
)
FetchContent_MakeAvailable(imgui)

# Add ImGui as a static library with specific source files including backends
add_library(imgui STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends ${glfw_SOURCE_DIR}/include)

# Add the main executable with unique source files
add_executable(opengl-cmake-starter-project
  src/Application.cpp
  src/MyApplication.cpp
  src/glError.cpp
  src/main.cpp
  src/Shader.cpp
)

# Set C++23 standard and enable all warnings
set_property(TARGET opengl-cmake-starter-project PROPERTY CXX_STANDARD 23)
target_compile_options(opengl-cmake-starter-project PRIVATE -Wall)

# Add GLM experimental extension definition
target_compile_definitions(opengl-cmake-starter-project PRIVATE GLM_ENABLE_EXPERIMENTAL)

# Link the executable with required libraries
target_link_libraries(opengl-cmake-starter-project
  PRIVATE glfw
  PRIVATE libglew_static
  PRIVATE glm
  PRIVATE imgui
)

# Configure the asset header file with CMake variables
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/asset.hpp.in
  ${CMAKE_CURRENT_BINARY_DIR}/src/asset.hpp
)

# Include directories for project sources and generated headers
target_include_directories(opengl-cmake-starter-project
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src
  PRIVATE ${imgui_SOURCE_DIR}
  PRIVATE ${imgui_SOURCE_DIR}/backends
  PRIVATE ${glew_SOURCE_DIR}/include
)