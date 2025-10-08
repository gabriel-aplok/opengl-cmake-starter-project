#include "MyApplication.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include "imgui.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "asset.hpp"
#include "glError.hpp"

// Vertex structure for heightmap mesh
struct VertexType {
    glm::vec3 position;  // 3D position of the vertex
    glm::vec3 normal;    // Surface normal for lighting
    glm::vec4 color;     // RGBA color of the vertex
};

// Computes height for a given 2D position using a sine-based function
float heightMap(const glm::vec2& position) {
    return 2.0f * std::sin(position.x) * std::sin(position.y);
}

// Generates a vertex for the heightmap at the given 2D position
VertexType getHeightMap(const glm::vec2& position) {
    const glm::vec2 dx(0.01f, 0.0f);  // Small offset for x-derivative
    const glm::vec2 dy(0.0f, 0.01f);  // Small offset for y-derivative

    VertexType vertex;
    float h = heightMap(position);
    // Approximate partial derivatives for normal calculation
    float hx = 100.0f * (heightMap(position + dx) - h);
    float hy = 100.0f * (heightMap(position + dy) - h);

    vertex.position = glm::vec3(position, h);
    vertex.normal = glm::normalize(glm::vec3(-hx, -hy, 1.0f));
    // Color based on height for visual variation
    float c = std::sin(h * 5.0f) * 0.5f + 0.5f;
    vertex.color = glm::vec4(c, 1.0f - c, 0.5f, 1.0f);

    return vertex;
}

MyApplication::MyApplication()
    : Application(),
    vertexShader(SHADER_DIR "/vertex_shader.glsl", GL_VERTEX_SHADER),
    fragmentShader(SHADER_DIR "/fragment_shader.glsl", GL_FRAGMENT_SHADER),
    shaderProgram({ vertexShader, fragmentShader }) {
    glCheckError(__FILE__, __LINE__);

    // Generate heightmap mesh
    std::vector<VertexType> vertices;
    std::vector<GLuint> indices;

    // Create grid of vertices
    for (int y = 0; y <= size; ++y) {
        for (int x = 0; x <= size; ++x) {
            float xx = (x - size / 2) * 0.1f;
            float yy = (y - size / 2) * 0.1f;
            vertices.push_back(getHeightMap({ xx, yy }));
        }
    }

    // Generate indices for triangle mesh
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            int base = x + (size + 1) * y;
            // First triangle
            indices.push_back(base);
            indices.push_back(base + 1);
            indices.push_back(base + size + 2);
            // Second triangle
            indices.push_back(base + size + 2);
            indices.push_back(base + size + 1);
            indices.push_back(base);
        }
    }

    // Log mesh statistics
    std::cout << "Vertices: " << vertices.size() << "\n";
    std::cout << "Indices: " << indices.size() << "\n";

    // Set up Vertex Buffer Object (VBO)
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(VertexType)),
        vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Set up Index Buffer Object (IBO)
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(GLuint)),
        indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Set up Vertex Array Object (VAO)
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Map vertex attributes to shader inputs
    shaderProgram.setAttribute("position", 3, sizeof(VertexType),
        offsetof(VertexType, position));
    shaderProgram.setAttribute("normal", 3, sizeof(VertexType),
        offsetof(VertexType, normal));
    shaderProgram.setAttribute("color", 4, sizeof(VertexType),
        offsetof(VertexType, color));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBindVertexArray(0);

    // Enable depth testing for proper rendering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Initialize ImGui
    initImGui(getWindow());
}

void MyApplication::initImGui(GLFWwindow* windowParam) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(windowParam, true);
    const char* glsl_version = "#version 150";
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Copy initial values
    clearColor[0] = style.Colors[ImGuiCol_WindowBg].x;
    clearColor[1] = style.Colors[ImGuiCol_WindowBg].y;
    clearColor[2] = style.Colors[ImGuiCol_WindowBg].z;
    lightPosArray[0] = lightPos.x;
    lightPosArray[1] = lightPos.y;
    lightPosArray[2] = lightPos.z;
}

void MyApplication::loop() {
    // Exit if window is closed
    if (glfwWindowShouldClose(getWindow())) {
        exit();
    }

    float t = getTime();
    // Configure camera and transformation matrices
    projection = glm::perspective(glm::radians(45.0f), getWindowRatio(), 0.1f, 100.0f);
    view = glm::lookAt(glm::vec3(20.0f * std::sin(t), 20.0f * std::cos(t), 20.0),
        glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::mat4(1.0f); // No additional model transformations
    lightPos = glm::vec3(lightPosArray[0], lightPosArray[1], lightPosArray[2]);

    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Main ImGui windows with docking, fuck this shit for real, i need someone to fix this, or  i will fix later
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

        ImGuiWindowFlags host_window_flags = 0;
        host_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
        host_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar;
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            host_window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpaceViewport", NULL, host_window_flags);
        ImGui::PopStyleVar(3);

        ImGuiID dockspace_id = ImGui::GetID("DockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags, nullptr);
        
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New Scene")) {
                    std::cout << "New Scene clicked\n";
                }
                if (ImGui::MenuItem("Open Scene")) {
                    std::cout << "Open Scene clicked\n";
                }
                if (ImGui::MenuItem("Save Scene")) {
                    std::cout << "Save Scene clicked\n";
                }
                if (ImGui::MenuItem("Save As")) {
                    std::cout << "Save As clicked\n";
                }
                if (ImGui::MenuItem("Exit")) {
                    std::cout << "Exit clicked\n";
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Undo")) {
                    std::cout << "Undo clicked\n";
                }
                if (ImGui::MenuItem("Redo")) {
                    std::cout << "Redo clicked\n";
                }
                if (ImGui::MenuItem("Preferences")) {
                    std::cout << "Preferences clicked\n";
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                if (ImGui::MenuItem("Toggle Grid")) {
                    std::cout << "Toggle Grid clicked\n";
                }
                if (ImGui::MenuItem("Camera Settings")) {
                    std::cout << "Camera Settings clicked\n";
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("GameObject")) {
                if (ImGui::MenuItem("Add Cube")) {
                    std::cout << "Add Cube clicked\n";
                }
                if (ImGui::MenuItem("Add Sphere")) {
                    std::cout << "Add Sphere clicked\n";
                }
                if (ImGui::MenuItem("Delete")) {
                    std::cout << "Delete clicked\n";
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help")) {
                if (ImGui::MenuItem("Documentation")) {
                    std::cout << "Documentation clicked\n";
                }
                if (ImGui::MenuItem("About")) {
                    std::cout << "About clicked\n";
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::End();
    }

    // Control Panel Dock
    {
        ImGui::Begin("Control Panel");

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        if (ImGui::Checkbox("Demo Window", &showDemoWindow))
            showMetrics = false;
        if (ImGui::Checkbox("Metrics", &showMetrics))
            showDemoWindow = false;

        ImGui::Separator();

        // Light Position Controls
        ImGui::Text("Light Position:");
        ImGui::SliderFloat("X", &lightPosArray[0], -20.0f, 20.0f);
        ImGui::SliderFloat("Y", &lightPosArray[1], -20.0f, 20.0f);
        ImGui::SliderFloat("Z", &lightPosArray[2], -20.0f, 20.0f);

        // Clear Color
        ImGui::ColorEdit3("Clear Color", clearColor);

        // Heightmap controls
        static float heightScale = 2.0f;
        ImGui::SliderFloat("Height Scale", &heightScale, 0.1f, 5.0f);

        ImGui::End();
    }

    // Show demo window
    if (showDemoWindow)
        ImGui::ShowDemoWindow(&showDemoWindow);

    // Show metrics window
    if (showMetrics)
        ImGui::ShowMetricsWindow(&showMetrics);

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(getWindow(), &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render 3D scene
    shaderProgram.use();
    shaderProgram.setUniform("projection", projection);
    shaderProgram.setUniform("view", view);
    shaderProgram.setUniform("model", model);
    shaderProgram.setUniform("lightPos", lightPos);

    glCheckError(__FILE__, __LINE__);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, size * size * 2 * 3, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    shaderProgram.unuse();

    // Render ImGui
    renderImGui();

    // Update and Render additional Platform Windows
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void MyApplication::renderImGui() {
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

MyApplication::~MyApplication() {
    shutdownImGui();
}

void MyApplication::shutdownImGui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}