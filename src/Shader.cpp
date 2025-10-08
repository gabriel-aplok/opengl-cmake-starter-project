#include "Shader.hpp"
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
// Reads file contents into a vector, appending a null terminator
void readFile(const std::string& filename, std::vector<char>& buffer) {
  std::ifstream file(filename, std::ios::binary);
  if (!file) {
    throw std::runtime_error("Failed to open file: " + filename);
  }
  file.seekg(0, std::ios::end);
  auto size = file.tellg();
  if (size <= 0) {
    throw std::runtime_error("File is empty: " + filename);
  }
  file.seekg(0, std::ios::beg);
  buffer.resize(static_cast<size_t>(size) + 1);
  file.read(buffer.data(), size);
  buffer.back() = '\0';
}
}  // namespace

Shader::Shader(const std::string& filename, GLenum type) {
  // Load shader source
  std::vector<char> source;
  readFile(filename, source);

  // Create and compile shader
  handle = glCreateShader(type);
  if (!handle) {
    throw std::runtime_error("Failed to create shader for: " + filename);
  }

  const char* sourcePtr = source.data();
  glShaderSource(handle, 1, &sourcePtr, nullptr);
  glCompileShader(handle);

  // Check compilation status
  GLint status;
  glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE) {
    GLint logSize = 0;
    glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logSize);
    std::vector<char> log(logSize + 1);
    glGetShaderInfoLog(handle, logSize, nullptr, log.data());
    throw std::runtime_error("Shader compilation failed: " + filename + "\n" +
                             log.data());
  }
  std::cout << "Shader compiled: " << filename << std::endl;
}

Shader::~Shader() {
  if (handle) {
    glDeleteShader(handle);
  }
}

ShaderProgram::ShaderProgram() {
  handle = glCreateProgram();
  if (!handle) {
    throw std::runtime_error("Failed to create shader program");
  }
}

ShaderProgram::ShaderProgram(std::initializer_list<Shader> shaderList)
    : ShaderProgram() {
  for (const auto& shader : shaderList) {
    glAttachShader(handle, shader.getHandle());
  }
  link();
}

void ShaderProgram::link() {
  glLinkProgram(handle);
  GLint status;
  glGetProgramiv(handle, GL_LINK_STATUS, &status);
  if (status != GL_TRUE) {
    GLint logSize = 0;
    glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &logSize);
    std::vector<char> log(logSize + 1);
    glGetProgramInfoLog(handle, logSize, nullptr, log.data());
    throw std::runtime_error("Shader program linking failed:\n" +
                             std::string(log.data()));
  }
}

GLint ShaderProgram::uniform(const std::string& name) {
  auto it = uniforms.find(name);
  if (it != uniforms.end()) {
    return it->second;
  }
  GLint loc = glGetUniformLocation(handle, name.c_str());
  if (loc < 0) {
    std::cerr << "Warning: Uniform '" << name << "' not found in program"
              << std::endl;
  }
  uniforms[name] = loc;
  return loc;
}

void ShaderProgram::setAttribute(const std::string& name,
                                 GLint size,
                                 GLsizei stride,
                                 GLuint offset,
                                 GLboolean normalize,
                                 GLenum type) {
  GLint loc = glGetAttribLocation(handle, name.c_str());
  if (loc < 0) {
    std::cerr << "Warning: Attribute '" << name << "' not found in program"
              << std::endl;
    return;
  }
  attributes[name] = loc;
  glEnableVertexAttribArray(loc);
  glVertexAttribPointer(
      loc, size, type, normalize, stride,
      reinterpret_cast<void*>(static_cast<uintptr_t>(offset)));
}

void ShaderProgram::setUniform(const std::string& name,
                               float x,
                               float y,
                               float z) {
  glUniform3f(uniform(name), x, y, z);
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec3& v) {
  glUniform3fv(uniform(name), 1, glm::value_ptr(v));
}

void ShaderProgram::setUniform(const std::string& name, const glm::dvec3& v) {
  glUniform3dv(uniform(name), 1, glm::value_ptr(v));
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec4& v) {
  glUniform4fv(uniform(name), 1, glm::value_ptr(v));
}

void ShaderProgram::setUniform(const std::string& name, const glm::dvec4& v) {
  glUniform4dv(uniform(name), 1, glm::value_ptr(v));
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat4& m) {
  glUniformMatrix4fv(uniform(name), 1, GL_FALSE, glm::value_ptr(m));
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat3& m) {
  glUniformMatrix3fv(uniform(name), 1, GL_FALSE, glm::value_ptr(m));
}

void ShaderProgram::setUniform(const std::string& name, float val) {
  glUniform1f(uniform(name), val);
}

void ShaderProgram::setUniform(const std::string& name, int val) {
  glUniform1i(uniform(name), val);
}

ShaderProgram::~ShaderProgram() {
  if (handle) {
    glDeleteProgram(handle);
  }
}

void ShaderProgram::use() const {
  glUseProgram(handle);
}
