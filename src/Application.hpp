#pragma once

#include <string>

struct GLFWwindow;

// Manages OpenGL initialization and window handling, providing utilities for
// window dimensions, timing, and a customizable render loop.
class Application {
 public:
  // Initializes GLFW, OpenGL context, and window
  Application();

  // Returns the singleton instance of the Application
  static Application& getInstance();

  // Gets the GLFW window handle
  GLFWwindow* getWindow() const { return window; }

  // Terminates the application
  void exit();

  // Returns the time between frames
  float getFrameDeltaTime() const { return deltaTime; }

  // Returns the elapsed time since application start
  float getTime() const { return time; }

  // Runs the main application loop
  void run();

  // Gets the current window width
  int getWidth() const { return width; }

  // Gets the current window height
  int getHeight() const { return height; }

  // Gets the window aspect ratio (width/height)
  float getWindowRatio() const { return static_cast<float>(width) / height; }

  // Checks if window dimensions have changed since last frame
  bool windowDimensionChanged() const { return dimensionChanged; }

 protected:
  Application(const Application&) = delete;             // Prevent copying
  Application& operator=(const Application&) = delete;  // Prevent assignment

  std::string title = "Application";  // Window title
  virtual void loop() {}              // Virtual render loop for derived classes

 private:
  enum class State { Ready, Run, Exit };  // Application state

  // Updates window dimensions and viewport if changed
  void detectWindowDimensionChange();

  State state = State::Ready;     // Current application state
  GLFWwindow* window = nullptr;   // GLFW window handle
  float time = 0.0f;              // Time since application start
  float deltaTime = 0.0f;         // Time between frames
  int width = 640;                // Window width
  int height = 480;               // Window height
  bool dimensionChanged = false;  // Flag for window size changes
};
