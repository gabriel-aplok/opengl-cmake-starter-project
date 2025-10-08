#pragma once

#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <initializer_list>
#include <map>
#include <string>

// Forward declaration
class ShaderProgram;

// Manages an OpenGL shader (vertex, fragment, etc.).
class Shader {
 public:
  // Loads and compiles a shader from a file
  Shader(const std::string& filename, GLenum type);

  // Returns the OpenGL shader handle
  GLuint getHandle() const { return handle; }

  // Cleans up shader resources
  ~Shader();

 private:
  GLuint handle = 0;  // OpenGL shader handle
  friend class ShaderProgram;
};

// Manages an OpenGL shader program, combining multiple shaders and providing
// interfaces for setting uniforms and attributes using GLM types.
class ShaderProgram {
 public:
  // Creates a program from a list of shaders
  ShaderProgram(std::initializer_list<Shader> shaderList);

  // Binds/unbinds the shader program
  void use() const;
  void unuse() const { glUseProgram(0); }

  // Returns the OpenGL program handle
  GLuint getHandle() const { return handle; }

  // Sets vertex attribute parameters
  void setAttribute(const std::string& name,
                    GLint size,
                    GLsizei stride,
                    GLuint offset,
                    GLboolean normalize = GL_FALSE,
                    GLenum type = GL_FLOAT);

  // Retrieves uniform location
  GLint uniform(const std::string& name);

  // Sets uniform values
  void setUniform(const std::string& name, float x, float y, float z);
  void setUniform(const std::string& name, const glm::vec3& v);
  void setUniform(const std::string& name, const glm::dvec3& v);
  void setUniform(const std::string& name, const glm::vec4& v);
  void setUniform(const std::string& name, const glm::dvec4& v);
  void setUniform(const std::string& name, const glm::mat4& m);
  void setUniform(const std::string& name, const glm::mat3& m);
  void setUniform(const std::string& name, float val);
  void setUniform(const std::string& name, int val);

  // Cleans up program resources
  ~ShaderProgram();

 private:
  ShaderProgram();  // Private constructor for initialization
  void link();      // Links the shader program

  GLuint handle = 0;                        // OpenGL program handle
  std::map<std::string, GLint> uniforms;    // Cache for uniform locations
  std::map<std::string, GLint> attributes;  // Cache for attribute locations
};
