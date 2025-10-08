#pragma once

#include <GL/glew.h>

/**
 * Checks for OpenGL errors and prints details to stderr.
 * Usage: glCheckError(__FILE__, __LINE__);
 * @param file Source file name (typically __FILE__)
 * @param line Line number in the source file (typically __LINE__)
 */
void glCheckError(const char* file, unsigned int line);