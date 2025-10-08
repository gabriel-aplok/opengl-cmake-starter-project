#include "Application.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>

Application* currentApplication = nullptr;

Application& Application::getInstance() {
  if (!currentApplication) {
    throw std::runtime_error("No Application instance exists");
  }
  return *currentApplication;
}

Application::Application()
    : state(State::Ready), width(640), height(480), title("My GLFW/GLEW/GLM/ImGui App") {
  currentApplication = this;

  std::cout << "[Info] Initializing GLFW" << std::endl;

  // Initialize GLFW
  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW");
  }

  // Configure OpenGL context (version 3.2, core profile)
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create window
  window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    throw std::runtime_error("Failed to create GLFW window");
  }

  // Set OpenGL context
  glfwMakeContextCurrent(window);

  // Initialize GLEW
  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if (err != GLEW_OK) {
    glfwTerminate();
    throw std::runtime_error(
        "Failed to initialize GLEW: " +
        std::string(reinterpret_cast<const char*>(glewGetErrorString(err))));
  }

  // Log renderer and OpenGL version
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n"
            << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

  // Configure OpenGL
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // Set initial viewport
  glViewport(0, 0, width, height);
}

void Application::exit() {
  state = State::Exit;
}

void Application::run() {
  if (state != State::Ready) {
    throw std::runtime_error("Application is not in Ready state");
  }

  state = State::Run;
  time = static_cast<float>(glfwGetTime());

  while (state == State::Run) {
    // Update timing
    float currentTime = static_cast<float>(glfwGetTime());
    deltaTime = currentTime - time;
    time = currentTime;

    // Check for window size changes
    detectWindowDimensionChange();

    // Execute user-defined render loop
    loop();

    // Swap buffers and process events
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clean up GLFW
  glfwTerminate();
  currentApplication = nullptr;
}

void Application::detectWindowDimensionChange() {
  int w, h;
  glfwGetWindowSize(window, &w, &h);
  dimensionChanged = (w != width || h != height);
  if (dimensionChanged) {
    width = w;
    height = h;
    glViewport(0, 0, width, height);
    std::cout << "[Info] Window resized to " << width << "x" << height
              << std::endl;
  }
}