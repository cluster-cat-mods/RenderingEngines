#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

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

int winWidth = 800;
int winHeight = 600;

GLenum drawMode = GL_TRIANGLES;

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
void GenerateImGuiObject(core::Model *model)
{
    if (ImGui::CollapsingHeader(model->GetName().c_str()))
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
        glm::vec3 newCol = light->GetColour();
        if (ImGui::ColorEdit3("Colour", &newCol[0]))
        {
            light->SetColour(glm::vec4(newCol, 1));
        }
    }
}

//endregion ImGuiMethods

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
    const GLuint fragmentShader = generateShader("shaders/adsLight.fs", GL_FRAGMENT_SHADER);
    const GLuint colourFragmentShader = generateShader("shaders/colour.fs", GL_FRAGMENT_SHADER);

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

    const unsigned int gizmoShaderProgram = glCreateProgram();
    glAttachShader(gizmoShaderProgram, modelVertexShader);
    glAttachShader(gizmoShaderProgram, colourFragmentShader);
    glLinkProgram(gizmoShaderProgram);
    glGetProgramiv(gizmoShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(gizmoShaderProgram, 512, NULL, infoLog);
        printf("Error! Making Shader Program: %s\n", infoLog);
    }

    glDeleteShader(modelVertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(textureShader);
    glDeleteShader(colourFragmentShader);

    GLint mvpMatrixUniform = glGetUniformLocation(modelShaderProgram, "mvpMatrix");
    GLint mMatrixUniform = glGetUniformLocation(modelShaderProgram, "mMatrix");
    GLint textUniform = glGetUniformLocation(modelShaderProgram, "text");
    GLint textureModelUniform = glGetUniformLocation(textureShaderProgram, "mvpMatrix");
    GLint textureUniform = glGetUniformLocation(textureShaderProgram, "text");

    GLint lightAC0Uniform = glGetUniformLocation(modelShaderProgram, "ambientColour");
    GLint lightAI0Uniform = glGetUniformLocation(modelShaderProgram, "ambientIntensity");
    GLint lightCountUniform = glGetUniformLocation(modelShaderProgram, "lightCount");

    GLint gizmo_mvpMatrixUniform = glGetUniformLocation(gizmoShaderProgram, "mvpMatrix");
    GLint gizmo_mMatrixUniform = glGetUniformLocation(gizmoShaderProgram, "mMatrix");

    //endregion shaderProgram

    //region Instantiating
    core::Scene mainScene = core::Scene();
    mainScene.SetName("mainScene");
    ActiveScene = &mainScene;

    core::Model suzanne = core::AssimpLoader::loadModel("models/nonormalmonkey.obj");
    suzanne.SetName("nonnormalmonkey");
    suzanne.transform.SetPosition(glm::vec3(0,0,0));
    core::Model* suzanneOb = ActiveScene->Instantiate(suzanne);

    core::Model suzanne2 = core::AssimpLoader::loadModel("models/nonormalmonkey.obj");
    suzanne2.SetName("nonnormalmonkey 2");
    suzanne2.transform.SetPosition(glm::vec3(0,2,0));
    core::Model* suzanne2Ob = ActiveScene->Instantiate(suzanne2);


    core::Model sphere = core::AssimpLoader::loadModel("models/sphere.obj");
    sphere.SetName("sphere");
    sphere.transform.SetPosition(glm::vec3(0,4,0));
    core::Model* sphereOb = ActiveScene->Instantiate(sphere);

    core::Model beer = core::AssimpLoader::loadModel("models/beer.obj");
    beer.SetName("beer");
    beer.transform.SetPosition(glm::vec3(5.0f, 2.0f, 0.0f));
    beer.transform.Scale(glm::vec3(5.0f, 5.0f, 5.0f));
    beer.transform.SetRotation(glm::vec3(0.0f, 0.0f, 20.0f));
    core::Model* beerOb = ActiveScene->Instantiate(beer);

    core::Texture cmgtGatoTexture("textures/CMGaTo_crop.png");
    core::Texture hexTexture("textures/Metal_Plate.png");

    glm::vec4 clearColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    glClearColor(clearColor.r,clearColor.g, clearColor.b, clearColor.a);

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
    float translationOffset = 0.01f;
    float rotationOffset = 0.1f;
    float fovOffset = 0.025f;
    input.AddKeyMap(GLFW_KEY_ESCAPE, GLFW_PRESS, false, [window](){glfwSetWindowShouldClose(window, GLFW_TRUE);});
    input.AddKeyMap(GLFW_KEY_W, GLFW_PRESS, true, [translationOffset](){ActiveScene->GetActiveCamera()->transform.Translate(glm::vec3(0.0f, 0.0f, -translationOffset));});
    input.AddKeyMap(GLFW_KEY_S, GLFW_PRESS, true, [translationOffset](){ActiveScene->GetActiveCamera()->transform.Translate(glm::vec3(0.0f, 0.0f, translationOffset));});
    input.AddKeyMap(GLFW_KEY_A, GLFW_PRESS, true, [translationOffset](){ActiveScene->GetActiveCamera()->transform.Translate(glm::vec3(-translationOffset, 0.0f, 0.0f));});
    input.AddKeyMap(GLFW_KEY_D, GLFW_PRESS, true, [translationOffset](){ActiveScene->GetActiveCamera()->transform.Translate(glm::vec3(translationOffset, 0.0f, 0.0f));});
    input.AddKeyMap(GLFW_KEY_Q, GLFW_PRESS, true, [translationOffset](){ActiveScene->GetActiveCamera()->transform.Translate(glm::vec3(0.0f, -translationOffset, 0.0f));});
    input.AddKeyMap(GLFW_KEY_E, GLFW_PRESS, true, [translationOffset](){ActiveScene->GetActiveCamera()->transform.Translate(glm::vec3(0.0f, translationOffset, 0.0f));});
    input.AddKeyMap(GLFW_KEY_LEFT, GLFW_PRESS, true, [rotationOffset](){ActiveScene->GetActiveCamera()->transform.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(rotationOffset));});
    input.AddKeyMap(GLFW_KEY_RIGHT, GLFW_PRESS, true, [rotationOffset](){ActiveScene->GetActiveCamera()->transform.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(-rotationOffset));});
    input.AddKeyMap(GLFW_KEY_DOWN, GLFW_PRESS, true, [fovOffset](){ActiveScene->GetActiveCamera()->ChangeFOV(-fovOffset);});
    input.AddKeyMap(GLFW_KEY_UP, GLFW_PRESS, true, [fovOffset](){ActiveScene->GetActiveCamera()->ChangeFOV(fovOffset);});

    input.StartInputCallback(window);
    //endregion input

    //region ImGuiVariables
    std::string label = "Triangles";

    //endregion ImGuiVariables

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //region ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("No rendering, too much engine");
        ImGui::Text("Moin!");
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

        ImGui::Begin(ActiveScene->GetName().c_str());
        glm::vec3 ambientCol = ActiveScene->GetAmbientColour();
        if (ImGui::ColorEdit3("AmbientColour", &ambientCol[0]))
        {
            ActiveScene->SetAmbientColour(glm::vec4(ambientCol, 1.0f));
        }
        float ambientIntensity = ActiveScene->GetAmbientIntensity();
        if (ImGui::InputFloat("AmbientIntensity", &ambientIntensity))
        {
            ActiveScene->SetAmbientIntensity(ambientIntensity);
        }

        ImGui::BeginTabBar("Objects");
        if (ImGui::BeginTabItem("Models"))
        {
            int i = 0;
            for (core::Model& model : *ActiveScene->GetModels())
            {
                ImGui::PushID(i);
                GenerateImGuiObject(&model);
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

        /*
        glUseProgram(textureShaderProgram);
        glUniformMatrix4fv(textureModelUniform, 1, GL_FALSE, glm::value_ptr(ActiveScene->GetActiveCamera()->PM(winWidth, winHeight) * ActiveScene->GetActiveCamera()->VM() * quadOb->transform.GetModelMatrix()));
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(textureUniform, 0);
        glBindTexture(GL_TEXTURE_2D, cmgtGatoTexture.getId());
        quadModel.render(drawMode);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
        */

        for (core::Model& model : *ActiveScene->GetModels())
        {
            int i = 0;
            for (core::Light& light : *ActiveScene->GetLights())
            {
                std::string lightUniformString = "lights[" + std::to_string(i) + "].";
                glUseProgram(modelShaderProgram);
                GLint lightColourUniform = glGetUniformLocation(modelShaderProgram, (lightUniformString + "colour").c_str());
                GLint lightPositionUniform = glGetUniformLocation(modelShaderProgram, (lightUniformString + "position").c_str());

                glUniform4fv(lightColourUniform, 1, &light.GetColour()[0]);
                glUniform3fv(lightPositionUniform, 1, &light.transform.GetPosition()[0]);
                i++;
            }

            glUseProgram(modelShaderProgram);
            glUniform4fv(lightAC0Uniform, 1, &ActiveScene->GetAmbientColour()[0]);
            glUniform1f(lightAI0Uniform, ActiveScene->GetAmbientIntensity());
            glUniform1i(lightCountUniform, ActiveScene->GetLights()->size());
            glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, glm::value_ptr(ActiveScene->GetActiveCamera()->PM(winWidth, winHeight) * ActiveScene->GetActiveCamera()->VM() * model.transform.GetModelMatrix()));
            glUniformMatrix4fv(mMatrixUniform, 1, GL_FALSE, glm::value_ptr(model.transform.GetModelMatrix()));
            glActiveTexture(GL_TEXTURE0);
            glUniform1i(textureUniform, 0);
            glBindTexture(GL_TEXTURE_2D, hexTexture.getId());
            model.render(drawMode);
            glBindVertexArray(0);
            glActiveTexture(GL_TEXTURE0);
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