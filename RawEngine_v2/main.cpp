#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "core/mesh.h"
#include "core/assimpLoader.h"
#include "core/Camera.h"
#include "core/texture.h"

//#define MAC_CLION
#define VSTUDIO

#ifdef MAC_CLION
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#endif

#ifdef VSTUDIO
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#endif

// Source - https://stackoverflow.com/a/39047129
// Posted by Christopher Oezbek, modified by community. See post 'Timeline' for change history
// Retrieved 2026-10-09, License - CC BY-SA 3.0

extern "C"
{
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

int winWidth = 800;
int winHeight = 600;

GLenum drawMode = GL_TRIANGLES;

core::Camera* ActiveCamera;

void processInput(GLFWwindow *window) {
    float translationOffset = 0.01f;
    float fovOffset = 0.1f;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        ActiveCamera->Translate(glm::vec3(0.0f, 0.0f, -translationOffset));
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        ActiveCamera->Translate(glm::vec3(0.0f, 0.0f, translationOffset));
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        ActiveCamera->Translate(glm::vec3(-translationOffset, 0.0f, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        ActiveCamera->Translate(glm::vec3(translationOffset, 0.0f, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        ActiveCamera->Translate(glm::vec3(0.0f, -translationOffset, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        ActiveCamera->Translate(glm::vec3(0.0f, translationOffset, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        ActiveCamera->ChangeFOV(-fovOffset);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        ActiveCamera->ChangeFOV(fovOffset);


}

void framebufferSizeCallback(GLFWwindow *window,
                             int width, int height) {
    winWidth = width;
    winHeight = height;
    glViewport(0, 0, width, height);
}

std::string readFileToString(const std::string &filePath) {
    std::ifstream fileStream(filePath, std::ios::in);
    if (!fileStream.is_open()) {
        printf("Could not open file: %s\n", filePath.c_str());
        return "";
    }
    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    return buffer.str();
}

GLuint generateShader(const std::string &shaderPath, GLuint shaderType) {
    printf("Loading shader: %s\n", shaderPath.c_str());
    const std::string shaderText = readFileToString(shaderPath);
    const GLuint shader = glCreateShader(shaderType);
    const char *s_str = shaderText.c_str();
    glShaderSource(shader, 1, &s_str, nullptr);
    glCompileShader(shader);
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("Error! Shader issue [%s]: %s\n", shaderPath.c_str(), infoLog);
    }
    return shader;
}

int main()
{
    //region glfwInit
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(winWidth, winHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    //endregion glfwInit

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    //region ImGUIInit
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 400");
    //endregion ImGUIInit

    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //region shaderProgram
    const GLuint modelVertexShader = generateShader("shaders/modelVertex.vs", GL_VERTEX_SHADER);
    const GLuint fragmentShader = generateShader("shaders/fragment.fs", GL_FRAGMENT_SHADER);
    const GLuint textureShader = generateShader("shaders/texture.fs", GL_FRAGMENT_SHADER);

    int success;
    char infoLog[512];
    const unsigned int modelShaderProgram = glCreateProgram();
    glAttachShader(modelShaderProgram, modelVertexShader);
    glAttachShader(modelShaderProgram, fragmentShader);
    glLinkProgram(modelShaderProgram);
    glGetProgramiv(modelShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(modelShaderProgram, 512, NULL, infoLog);
        printf("Error! Making Shader Program: %s\n", infoLog);
    }
    const unsigned int textureShaderProgram = glCreateProgram();
    glAttachShader(textureShaderProgram, modelVertexShader);
    glAttachShader(textureShaderProgram, textureShader);
    glLinkProgram(textureShaderProgram);
    glGetProgramiv(textureShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(textureShaderProgram, 512, NULL, infoLog);
        printf("Error! Making Shader Program: %s\n", infoLog);
    }

    glDeleteShader(modelVertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(textureShader);
    //endregion shaderProgram

    core::Mesh quad = core::Mesh::generateQuad();
    core::Model quadModel({quad});
    quadModel.translate(glm::vec3(0,0,-2.5));
    quadModel.scale(glm::vec3(5, 5, 1));

    core::Model suzanne = core::AssimpLoader::loadModel("models/nonormalmonkey.obj");
    core::Texture cmgtGatoTexture("textures/CMGaTo_crop.png");

    glm::vec4 clearColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    glClearColor(clearColor.r,clearColor.g, clearColor.b, clearColor.a);

    core::Camera cam(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), 45.0f, 0.1f, 100.0f);
    ActiveCamera = &cam;

    GLint mvpMatrixUniform = glGetUniformLocation(modelShaderProgram, "mvpMatrix");
    GLint textureModelUniform = glGetUniformLocation(textureShaderProgram, "mvpMatrix");
    GLint textureUniform = glGetUniformLocation(textureShaderProgram, "text");

    double currentTime = glfwGetTime();
    double finishFrameTime = 0.0;
    float deltaTime = 0.0f;
    float rotationStrength = 100.0f;
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //region ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Raw Engine v2");
        ImGui::Text("Hello :)");
        std::string label = "Triangles";
        if (ImGui::Button(label.c_str()))
        {
            if (drawMode == GL_TRIANGLES)
            {
                drawMode = GL_LINES;
                label = "Wireframe";
            }
            else if (drawMode == GL_LINES)
            {
                drawMode = GL_POINTS;
                label = "Points";
            }
            else if (drawMode == GL_POINTS)
            {
                drawMode = GL_TRIANGLES;
                label = "Triangles";
            }
        }
        ImGui::End();
        //endregion ImGui

        processInput(window);

        suzanne.rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(rotationStrength) * static_cast<float>(deltaTime));

        glUseProgram(textureShaderProgram);
        glUniformMatrix4fv(textureModelUniform, 1, GL_FALSE, glm::value_ptr(cam.PM(winWidth, winHeight) * cam.VM() * quadModel.getModelMatrix()));
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(textureUniform, 0);
        glBindTexture(GL_TEXTURE_2D, cmgtGatoTexture.getId());
        quadModel.render(drawMode);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);

        glUseProgram(modelShaderProgram);
        glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, glm::value_ptr(cam.PM(winWidth, winHeight) * cam.VM() * suzanne.getModelMatrix()));
        suzanne.render(drawMode);
        glBindVertexArray(0);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
        finishFrameTime = glfwGetTime();
        deltaTime = static_cast<float>(finishFrameTime - currentTime);
        currentTime = finishFrameTime;
    }

    glDeleteProgram(modelShaderProgram);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}