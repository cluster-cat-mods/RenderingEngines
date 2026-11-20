#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <nfd.h>
#include <nfd_glfw3.h>

#include "imgui_internal.h"
#include "core/mesh.h"
#include "core/assimpLoader.h"
#include "core/Camera.h"
#include "core/Input.h"
#include "core/Scene.h"
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

int winWidth = 1280;
int winHeight = 720;

GLenum drawMode = GL_TRIANGLES;

void error_callback(int error_code, const char* description) {
    printf("GLFW call failed (code %d): %s\n", error_code, description);
}

void set_native_window(GLFWwindow* glfwWindow, nfdwindowhandle_t* nativeWindow) {
    if (!NFD_GetNativeWindowFromGLFWWindow(glfwWindow, nativeWindow)) {
        printf("NFD_GetNativeWindowFromGLFWWindow failed\n");
    }
}

std::string readFilePath(GLFWwindow* window)
{
    std::string pathOut;
    char* path;
    nfdopendialogu8args_t args = {0};
    set_native_window(window, &args.parentWindow);
    const nfdresult_t res = NFD_OpenDialogU8_With(&path, &args);
    switch (res) {
        case NFD_OKAY:
            printf("NFD_OpenDialogU8_With success: %s\n", path);
            pathOut = path;
            NFD_FreePathU8(path);
            break;
        case NFD_CANCEL:
            printf("NFD_OpenDialogU8_With cancelled\n");
            break;
        case NFD_ERROR:
            printf("NFD_OpenDialogU8_With error: %s\n", NFD_GetError());
            break;
        default:
            break;
    }
    return pathOut;
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    winWidth = width;
    winHeight = height;
    glViewport(0, 0, width, height);
}

std::string readFileToString(const std::string &filePath)
{
    std::ifstream fileStream(filePath, std::ios::in);
    if (!fileStream.is_open())
    {
        printf("Could not open file: %s\n", filePath.c_str());
        return "";
    }
    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    return buffer.str();
}

GLuint generateShader(const std::string &shaderPath, GLuint shaderType)
{
    printf("Loading shader: %s\n", shaderPath.c_str());
    const std::string shaderText = readFileToString(shaderPath);
    const GLuint shader = glCreateShader(shaderType);
    const char *s_str = shaderText.c_str();
    glShaderSource(shader, 1, &s_str, nullptr);
    glCompileShader(shader);
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("Error! Shader issue [%s]: %s\n", shaderPath.c_str(), infoLog);
    }
    return shader;
}

//region ProgVars
std::vector<core::Scene> scenes;
core::Scene* ActiveScene{};
//endregion ProgVars

//region ImGuiMethods
void GenerateImGuiObject(core::Model *model, GLFWwindow* window)
{
    if (ImGui::CollapsingHeader(model->GetName().c_str()))
    {
        if (ImGui::TreeNode("Transform"))
        {
            glm::vec3 newPos = model->transform.GetPosition();
            if (ImGui::InputFloat3("Position", &newPos[0]))
            {
                model->transform.SetPosition(newPos);
            }
            glm::vec3 newRot = model->transform.GetRotation();
            if (ImGui::InputFloat3("Rotation", &newRot[0]))
            {
                model->transform.SetRotation(newRot);
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Material"))
        {
            if (ImGui::Button("...##1"))
            {
                std::string path = readFilePath(window);
                if (!path.empty())
                {
                    model->material.SetVertexShader(path);
                    model->material.CreateShaderProgram();
                }
            } ImGui::SameLine();
            ImGui::Text(model->material.GetVertexShaderPath().c_str());
            if (ImGui::Button("...##2"))
            {
                std::string path = readFilePath(window);
                if (!path.empty())
                {
                    model->material.SetFragmentShader(path);
                    model->material.CreateShaderProgram();
                }
            } ImGui::SameLine();
            ImGui::Text(model->material.GetFragmentShaderPath().c_str());

            glm::vec4 newColor = model->material.GetColor();
            if (ImGui::ColorEdit4("Color", &newColor[0]))
            {
                model->material.SetColor(newColor);
            }
            float newSpecularStrength = model->material.GetSpecularStrength();
            if (ImGui::DragFloat("Specular Strength", &newSpecularStrength, 0.01f, 0.0f, 1.0f))
            {
                model->material.SetSpecularStrength(newSpecularStrength);
            }

            if (ImGui::ImageButton("Select TextureMap", model->material.GetTextureMap().getId(), ImVec2(40.0f, 40.0f)))
            {
                std::string path = readFilePath(window);
                if (!path.empty()) model->material.SetTextureMap(path);
            } ImGui::SameLine();
            if (ImGui::ImageButton("Select NormalMap", model->material.GetNormalMap().getId(), ImVec2(40.0f, 40.0f)))
            {
                std::string path = readFilePath(window);
                if (!path.empty()) model->material.SetNormalMap(path);
            } ImGui::SameLine();
            if (ImGui::ImageButton("Select SpecularMap", model->material.GetSpecularMap().getId(), ImVec2(40.0f, 40.0f)))
            {
                std::string path = readFilePath(window);
                if (!path.empty()) model->material.SetSpecularMap(path);
            }
            ImGui::TreePop();
        }

    }
}
void generateImGuiObject(core::Camera *camera)
{
    if (ImGui::CollapsingHeader(camera->GetName().c_str()))
    {
        glm::vec3 newPos = camera->transform.GetPosition();
        if (ImGui::InputFloat3("Position", &newPos[0]))
        {
            camera->transform.SetPosition(newPos);
        }
        glm::vec3 newRot = camera->transform.GetRotation();
        if (ImGui::InputFloat3("Rotation", &newRot[0]))
        {
            camera->transform.SetRotation(newRot);
        }
        float newFOV = camera->GetFOV();
        if (ImGui::SliderFloat("fov", &newFOV, 45.0f, 135.0f))
        {
            camera->SetFOV(newFOV);
        }
        if (camera != ActiveScene->GetActiveCamera())
        {
            ImGui::Text("Inactive");
            if (ImGui::Button("Set Active Camera"))
            {
                ActiveScene->SetActiveCamera(camera);
            }
        }
        else
        {
            ImGui::Text("Active");
        }
    }
}
void generateImGuiObject(core::Light *light)
{
    if (ImGui::CollapsingHeader(light->GetName().c_str()))
    {
        glm::vec3 newPos = light->transform.GetPosition();
        if (ImGui::InputFloat3("Position", &newPos[0]))
        {
            light->transform.SetPosition(newPos);
        }
        glm::vec3 newRot = light->transform.GetRotation();
        if (ImGui::InputFloat3("Rotation", &newRot[0]))
        {
            light->transform.SetRotation(newRot);
        }
        glm::vec3 newCol = light->GetColor();
        if (ImGui::ColorEdit3("Color", &newCol[0]))
        {
            light->SetColor(glm::vec4(newCol, 1));
        }
    }
}

//endregion ImGuiMethods

float* deltaPtr{};

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

    //region GLADInit
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }
    //endregion GLADInit

    //region ImGUIInit
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 400");
    //endregion ImGUIInit

    //region NFDInit
    NFD_Init();
    //endregion NFDInit

    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    //region shaderProgram
    const GLuint lightVertexShader = generateShader("defaultAssets/shaders/model_default.vs", GL_VERTEX_SHADER);
    const GLuint lightFragmentShader = generateShader("defaultAssets/shaders/light_default.fs", GL_FRAGMENT_SHADER);

    const GLuint frameBufferVertexShader = generateShader("defaultAssets/shaders/framebuffer_default.vs", GL_VERTEX_SHADER);
    const GLuint frameBufferFragmentShader = generateShader("defaultAssets/shaders/framebuffer_default.fs", GL_FRAGMENT_SHADER);

    int success;
    char infoLog[512];
    const unsigned int gizmoShaderProgram = glCreateProgram();
    glAttachShader(gizmoShaderProgram, lightVertexShader);
    glAttachShader(gizmoShaderProgram, lightFragmentShader);
    glLinkProgram(gizmoShaderProgram);
    glGetProgramiv(gizmoShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(gizmoShaderProgram, 512, NULL, infoLog);
        printf("Error! Making Shader Program: %s\n", infoLog);
    }

    glDeleteShader(lightVertexShader);
    glDeleteShader(lightFragmentShader);

    GLint gizmo_mvpMatrixUniform = glGetUniformLocation(gizmoShaderProgram, "mvpMatrix");
    GLint gizmo_mMatrixUniform = glGetUniformLocation(gizmoShaderProgram, "mMatrix");

    const unsigned int frameBufferShaderProgram = glCreateProgram();
    glAttachShader(frameBufferShaderProgram, frameBufferVertexShader);
    glAttachShader(frameBufferShaderProgram, frameBufferFragmentShader);
    glLinkProgram(frameBufferShaderProgram);
    glGetProgramiv(frameBufferShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(frameBufferShaderProgram, 512, NULL, infoLog);
        printf("Error! Making Shader Program: %s\n", infoLog);
    }

    glDeleteShader(frameBufferVertexShader);
    glDeleteShader(frameBufferFragmentShader);

    GLuint sceneTextureUniform = glGetUniformLocation(frameBufferShaderProgram, "sceneTexture");
    //endregion shaderProgram

    //region frameBuffers
    //generate buffer
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    //bind buffer
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // generate texture
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, winWidth, winHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    //attach it to currently bound framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    //generate render buffer attachment
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, winWidth, winHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    //attach rbo to depth and stencil attachment of the frame render buffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    //debug log
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    //rebind screen buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //endregion frameBuffers


    //region Instantiating
    glm::vec4 clearColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    glClearColor(clearColor.r,clearColor.g, clearColor.b, clearColor.a);

    core::Scene mainScene = core::Scene();
    mainScene.SetName("mainScene");
    ActiveScene = &mainScene;

    core::Model suzanne = core::AssimpLoader::loadModel("models/nonormalmonkey.obj");
    suzanne.SetName("nonnormalmonkey");
    suzanne.transform.SetPosition(glm::vec3(0,0,0));
    core::Model* suzanneOb = ActiveScene->Instantiate(suzanne);
    suzanneOb->material.SetTextureMap("../textures/CMGaTo_crop.png");

    core::Model suzanne2 = core::AssimpLoader::loadModel("models/nonormalmonkey.obj");
    suzanne2.SetName("nonnormalmonkey 2");
    suzanne2.transform.SetPosition(glm::vec3(0,2,0));
    core::Model* suzanne2Ob = ActiveScene->Instantiate(suzanne2);


    core::Model sphere = core::AssimpLoader::loadModel("models/sphere.obj");
    sphere.SetName("sphere");
    sphere.transform.SetPosition(glm::vec3(0,4,0));
    core::Model* sphereOb = ActiveScene->Instantiate(sphere);
    //sphereOb->material.SetTextureMap("../textures/CMGaTo_crop.png");

    core::Model beer = core::AssimpLoader::loadModel("models/beer.obj");
    beer.SetName("beer");
    beer.transform.SetPosition(glm::vec3(5.0f, 2.0f, 0.0f));
    beer.transform.Scale(glm::vec3(5.0f, 5.0f, 5.0f));
    beer.transform.SetRotation(glm::vec3(0.0f, 0.0f, 20.0f));
    core::Model* beerOb = ActiveScene->Instantiate(beer);

    core::Mesh screenQuad = core::Mesh::generateQuad();

    core::Light light1(core::Transform(), glm::vec4(1.0f, 0.97f, 0.83f, 1.0f));
    core::Light* mainLight = ActiveScene->Instantiate(light1);
    mainLight->SetName("mainLight");
    mainLight->transform.SetPosition(glm::vec3(2.0f, 0.5f, 0.0f));

    core::Light light2(core::Transform(), glm::vec4(1.0f, 0.60f, 0.75f, 1.0f));
    core::Light* otherLight = ActiveScene->Instantiate(light2);
    otherLight->SetName("otherLight");
    otherLight->transform.SetPosition(glm::vec3(-3.5f, 1.5f, 0.0f));

    core::Camera cam1(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), 10.0f, 45.0f, 0.1f, 100.0f, "cam1");
    core::Camera* mainCam = ActiveScene->Instantiate(cam1);
    ActiveScene->SetActiveCamera(mainCam);

    //endregion Instantiating

    //region GameLoopTime
    double currentTime = glfwGetTime();
    double finishFrameTime = 0.0;
    float deltaTime = 0.0f;
    //endregion GameLoopTime

    //region BehaviourVariables
    float rotationStrength = 100.0f;
    //endregion BehaviourVariables

    //region input
    core::Input input;
    float cameraMoveSpeed = 1.0f;
    float cameraRotateSpeed = 10.0;
    float cameraZoomSpeed = 1.0f;
    deltaPtr = &deltaTime;
    input.AddKeyMap(GLFW_KEY_0, GLFW_PRESS, false, [](){std::cout << *deltaPtr << std::endl;});
    input.AddKeyMap(GLFW_KEY_ESCAPE, GLFW_PRESS, false, [window](){glfwSetWindowShouldClose(window, GLFW_TRUE);});
    input.AddKeyMap(GLFW_KEY_W, GLFW_PRESS, true, [cameraMoveSpeed](){ActiveScene->GetActiveCamera()->transform.Translate(glm::vec3(0.0f, 0.0f, -cameraMoveSpeed * *deltaPtr));});
    input.AddKeyMap(GLFW_KEY_S, GLFW_PRESS, true, [cameraMoveSpeed](){ActiveScene->GetActiveCamera()->transform.Translate(glm::vec3(0.0f, 0.0f, cameraMoveSpeed  * *deltaPtr));});
    input.AddKeyMap(GLFW_KEY_A, GLFW_PRESS, true, [cameraMoveSpeed](){ActiveScene->GetActiveCamera()->transform.Translate(glm::vec3(-cameraMoveSpeed  * *deltaPtr, 0.0f, 0.0f));});
    input.AddKeyMap(GLFW_KEY_D, GLFW_PRESS, true, [cameraMoveSpeed](){ActiveScene->GetActiveCamera()->transform.Translate(glm::vec3(cameraMoveSpeed  * *deltaPtr, 0.0f, 0.0f));});
    input.AddKeyMap(GLFW_KEY_Q, GLFW_PRESS, true, [cameraMoveSpeed](){ActiveScene->GetActiveCamera()->transform.Translate(glm::vec3(0.0f, -cameraMoveSpeed  * *deltaPtr, 0.0f));});
    input.AddKeyMap(GLFW_KEY_E, GLFW_PRESS, true, [cameraMoveSpeed](){ActiveScene->GetActiveCamera()->transform.Translate(glm::vec3(0.0f, cameraMoveSpeed  * *deltaPtr, 0.0f));});
    input.AddKeyMap(GLFW_KEY_LEFT, GLFW_PRESS, true, [cameraRotateSpeed](){ActiveScene->GetActiveCamera()->transform.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(cameraRotateSpeed  * *deltaPtr));});
    input.AddKeyMap(GLFW_KEY_RIGHT, GLFW_PRESS, true, [cameraRotateSpeed](){ActiveScene->GetActiveCamera()->transform.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(-cameraRotateSpeed  * *deltaPtr));});
    input.AddKeyMap(GLFW_KEY_Z, GLFW_PRESS, true, [cameraZoomSpeed](){ActiveScene->GetActiveCamera()->ChangeFOV(-cameraZoomSpeed * *deltaPtr);});
    input.AddKeyMap(GLFW_KEY_X, GLFW_PRESS, true, [cameraZoomSpeed](){ActiveScene->GetActiveCamera()->ChangeFOV(cameraZoomSpeed * *deltaPtr);});

    input.StartInputCallback(window);
    //endregion input

    //region ImGuiVariables
    std::string label = "Triangles";

    //endregion ImGuiVariables

    while (!glfwWindowShouldClose(window))
    {

        // first pass
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
        glEnable(GL_DEPTH_TEST);


        //region ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Engine settings");
        ImGui::BeginTabBar("SettingTabBar");
        if (ImGui::BeginTabItem("Rendering"))
        {
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
            ImGui::PopID();
        }
        if (ImGui::BeginTabItem("Active camera"))
        {
            ImGui::Text("vars not being fucking updated grr");
            float& newCamMoveSpeed = cameraMoveSpeed;
            if (ImGui::InputFloat("Move speed", &newCamMoveSpeed))
            {
                cameraMoveSpeed = newCamMoveSpeed;
            }
            float& newCamRotateSpeed = cameraRotateSpeed;
            if (ImGui::InputFloat("Rotate speed", &newCamRotateSpeed))
            {
                cameraRotateSpeed = newCamRotateSpeed;
            }
            float newCamZoomSpeed = cameraZoomSpeed;
            if (ImGui::InputFloat("Zoom speed", &newCamZoomSpeed))
            {
                cameraZoomSpeed = newCamZoomSpeed;
            }
            ImGui::PopID();
        }
        ImGui::EndTabBar();

        ImGui::End();

        ImGui::Begin(ActiveScene->GetName().c_str());
        ImGui::BeginTabBar("SceneSetting");
        if (ImGui::BeginTabItem("Lighting"))
        {
            glm::vec3 ambientCol = ActiveScene->GetAmbientColor();
            if (ImGui::ColorEdit3("AmbientColor", &ambientCol[0]))
            {
                ActiveScene->SetAmbientColor(glm::vec4(ambientCol, 1.0f));
            }
            float ambientIntensity = ActiveScene->GetAmbientIntensity();
            if (ImGui::InputFloat("AmbientIntensity", &ambientIntensity))
            {
                ActiveScene->SetAmbientIntensity(ambientIntensity);
            }
        }
        ImGui::EndTabItem();
        ImGui::EndTabBar();

        ImGui::BeginTabBar("Hierarchy");
        if (ImGui::BeginTabItem("Models"))
        {
            int i = 0;
            for (core::Model& model : *ActiveScene->GetModels())
            {
                ImGui::PushID(i);
                GenerateImGuiObject(&model, window);
                ImGui::PopID();
                i++;
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Cameras"))
        {
            int i = 0;
            for (core::Camera& camera : *ActiveScene->GetCameras())
            {
                ImGui::PushID(i);
                generateImGuiObject(&camera);
                ImGui::PopID();
                i++;
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Lights"))
        {
            int i = 0;
            for (core::Light& light : *ActiveScene->GetLights())
            {
                ImGui::PushID(i);
                generateImGuiObject(&light);
                ImGui::PopID();
                i++;
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
        ImGui::End();

        //endregion ImGui

        //process intput
        input.ProccesHeldKeys();

        //region modelBehaviour
        suzanneOb->transform.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(rotationStrength * 2) * static_cast<float>(deltaTime));
        suzanne2Ob->transform.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(-rotationStrength * 1) * static_cast<float>(deltaTime));
        sphereOb->transform.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(rotationStrength * 0.5f) * static_cast<float>(deltaTime));

        beerOb->transform.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(rotationStrength * 0.2f) * static_cast<float>(deltaTime));
        //endregion modelBehaviour
        for (core::Model& model : *ActiveScene->GetModels())
        {
            const unsigned int shaderProgram = model.material.GetShaderProgram();
            GLint mvpMatrixUniform = glGetUniformLocation(shaderProgram, "mvpMatrix");
            GLint mMatrixUniform = glGetUniformLocation(shaderProgram, "mMatrix");
            GLint lightAC0Uniform = glGetUniformLocation(shaderProgram, "ambientColor");
            GLint lightAI0Uniform = glGetUniformLocation(shaderProgram, "ambientIntensity");
            GLint lightCountUniform = glGetUniformLocation(shaderProgram, "lightCount");
            GLint cameraPositionUniform = glGetUniformLocation(shaderProgram, "cameraPosition");

            glUseProgram(shaderProgram);
            glUniform4fv(lightAC0Uniform, 1, &ActiveScene->GetAmbientColor()[0]);
            glUniform1f(lightAI0Uniform, ActiveScene->GetAmbientIntensity());
            glUniform1i(lightCountUniform, ActiveScene->GetLights()->size());
            glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, glm::value_ptr(ActiveScene->GetActiveCamera()->PM(winWidth, winHeight) * ActiveScene->GetActiveCamera()->VM() * model.transform.GetModelMatrix()));
            glUniformMatrix4fv(mMatrixUniform, 1, GL_FALSE, glm::value_ptr(model.transform.GetModelMatrix()));
            glUniform3fv(cameraPositionUniform, 1, &ActiveScene->GetActiveCamera()->transform.GetPosition()[0]);
            glBindVertexArray(0);

            int i = 0;
            for (core::Light& light : *ActiveScene->GetLights())
            {
                glUseProgram(shaderProgram);
                std::string lightUniformString = "lights[" + std::to_string(i) + "].";
                GLint lightColorUniform = glGetUniformLocation(shaderProgram, (lightUniformString + "color").c_str());
                GLint lightPositionUniform = glGetUniformLocation(shaderProgram, (lightUniformString + "position").c_str());

                glUniform4fv(lightColorUniform, 1, &light.GetColor()[0]);
                glUniform3fv(lightPositionUniform, 1, &light.transform.GetPosition()[0]);
                i++;
                glBindVertexArray(0);
            }

            glUseProgram(shaderProgram);
            model.render(GL_TRIANGLES);
            glBindVertexArray(0);
        }


        for (core::Light& light : *ActiveScene->GetLights())
        {
            core::Model lightGizmo = core::AssimpLoader::loadModel("models/sphere.obj");
            lightGizmo.transform.SetPosition(light.transform.GetPosition());
            lightGizmo.transform.Scale(glm::vec3(0.2f));
            glUseProgram(gizmoShaderProgram);
            glUniformMatrix4fv(gizmo_mvpMatrixUniform, 1, GL_FALSE, glm::value_ptr(ActiveScene->GetActiveCamera()->PM(winWidth, winHeight) * ActiveScene->GetActiveCamera()->VM() * lightGizmo.transform.GetModelMatrix()));
            glUniformMatrix4fv(gizmo_mMatrixUniform, 1, GL_FALSE, glm::value_ptr(lightGizmo.transform.GetModelMatrix()));
            lightGizmo.render(GL_LINES);
            glBindVertexArray(0);
        }

        // second pass
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(frameBufferShaderProgram);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(sceneTextureUniform, 0);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glDisable(GL_DEPTH_TEST);
        screenQuad.render(GL_TRIANGLES);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
        finishFrameTime = glfwGetTime();
        deltaTime = static_cast<float>(finishFrameTime - currentTime);
        currentTime = finishFrameTime;
    }

    glDeleteProgram(gizmoShaderProgram);
    for (core::Model& model : *ActiveScene->GetModels())
    {
        model.material.DeleteShaderProgram();
    }


    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();

    NFD_Quit();

    return 0;
}