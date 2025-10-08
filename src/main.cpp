#include <iostream>
#include "MyApplication.hpp"

/**
 * Program entry point.
 * Initializes and runs the MyApplication instance.
 * @param argc Number of command-line arguments
 * @param argv Array of command-line argument strings
 * @return Exit status (0 for success, non-zero for failure)
 */
int main(int argc, const char* argv[]) {
  try {
    MyApplication app;
    std::cout << "Starting MyApplication..." << std::endl;
    app.run();
    std::cout << "MyApplication terminated successfully." << std::endl;
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}