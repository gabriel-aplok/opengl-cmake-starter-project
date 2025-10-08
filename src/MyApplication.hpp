#pragma once

#include <glm/glm.hpp>
#include "Application.hpp"
#include "Shader.hpp"

// Forward declarations
struct GLFWwindow;

// Application class for rendering a heightmap mesh with custom shaders
class MyApplication : public Application {
public:
	MyApplication();
	~MyApplication();

protected:
	// Main render loop
	virtual void loop();

private:
	static const int size = 100;  // Grid size for heightmap

	// Shader resources
	Shader vertexShader;
	Shader fragmentShader;
	ShaderProgram shaderProgram;

	// Transformation matrices and light position
	glm::mat4 projection = glm::mat4(1.0f);               // Projection matrix
	glm::mat4 view = glm::mat4(1.0f);                     // View matrix
	glm::mat4 model = glm::mat4(1.0f);                    // Model matrix
	glm::vec3 lightPos = glm::vec3(10.0f, 10.0f, 10.0f);  // Light position

	// OpenGL buffer objects
	GLuint vao;  // Vertex Array Object
	GLuint vbo;  // Vertex Buffer Object
	GLuint ibo;  // Index Buffer Object

	// ImGui resources
	bool showDemoWindow = true;
	bool showMetrics = false;
	float clearColor[4] = { 0.1f, 0.1f, 0.2f, 1.0f };
	float lightPosArray[3] = { 10.0f, 10.0f, 10.0f };

	// ImGui initialization and rendering
	void initImGui(GLFWwindow* windowParam);
	void renderImGui();
	void shutdownImGui();
};