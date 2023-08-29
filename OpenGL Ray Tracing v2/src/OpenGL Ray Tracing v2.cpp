// OpenGL Ray Tracing v2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "computeShader.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
bool TestRayOBBIntersection(glm::vec3 ray_origin, glm::vec3 ray_direction, glm::vec3 aabb_min, glm::vec3 aabb_max, glm::mat4 ModelMatrix, float& intersection_distance);
void renderQuad();

const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;

const unsigned int TEXTURE_WIDTH = 1000, TEXTURE_HEIGHT = 1000;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool spacePress = false;
bool firstPress = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
    std::cout << "Hello World!\n";

    if (!glfwInit())
    {
        throw std::runtime_error("GLFW failed to initialize.");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Ray Tracing in OpenGL", NULL, NULL);
    //if (window == NULL);
    //{
    //    std::cout << "Failed to create GLFW window" << std::endl;
    //    glfwTerminate();
    //    return -1;
    //}
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to intialize GLAD" << std::endl;
        return -1;
    }

    //glEnable(GL_DEPTH_TEST);
   //glEnable(GL_MULTISAMPLE);

#pragma region Standard Renderer

    //Shader lightingShader("Shaders\\colors.vs", "Shaders\\colors.fs");
    //Shader lightCubeShader("Shaders\\light_cube.vs", "Shaders\\light_cube.fs");
    //Shader planeShader("Shaders\\plane.vs", "Shaders\\plane.fs");
    //Shader lineShader("Shaders\\line.vs", "Shaders\\line.fs");

    //float vertices[] = {
    //    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
    //     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
    //     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    //     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    //    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
    //    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

    //    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
    //     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
    //     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    //     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    //    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
    //    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

    //    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    //    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    //    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    //    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    //    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    //    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    //     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    //     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    //     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    //     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    //     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    //     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    //    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
    //     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
    //     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    //     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    //    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
    //    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

    //    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
    //     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
    //     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    //     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    //    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
    //    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    //};

    //glm::vec3 cubePositions[] = {
    //    glm::vec3(0.0f,  0.0f,  0.0f),
    //    glm::vec3(2.0f,  5.0f, -15.0f),
    //    glm::vec3(-1.5f, -2.2f, -2.5f),
    //    glm::vec3(-3.8f, -2.0f, -12.3f),
    //    glm::vec3(2.4f, -0.4f, -3.5f),
    //    glm::vec3(-1.7f,  3.0f, -7.5f),
    //    glm::vec3(1.3f, -2.0f, -2.5f),
    //    glm::vec3(1.5f,  2.0f, -2.5f),
    //    glm::vec3(1.5f,  0.2f, -1.5f),
    //    glm::vec3(-1.3f,  1.0f, -1.5f)
    //};

    //glm::vec3 pointLightPositions[] = {
    //    glm::vec3(0.7f,  0.2f,  2.0f),
    //    glm::vec3(2.3f, -3.3f, -4.0f),
    //    glm::vec3(-4.0f,  2.0f, -12.0f),
    //    glm::vec3(0.0f,  0.0f, -3.0f)
    //};

    //float planeVertices[] = {
    //     5.0f, -0.5f,  5.0f, 0.0f, 1.0f, 0.0f, 2.0f, 0.0f,
    //    -5.0f, -0.5f,  5.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
    //    -5.0f, -0.5f, -5.0f, 0.0f, 1.0f, 0.0f,  0.0f, 2.0f,

    //     5.0f, -0.5f,  5.0f, 0.0f, 1.0f, 0.0f,  2.0f, 0.0f,
    //    -5.0f, -0.5f, -5.0f, 0.0f, 1.0f, 0.0f,  0.0f, 2.0f,
    //     5.0f, -0.5f, -5.0f, 0.0f, 1.0f, 0.0f,  2.0f, 2.0f
    //};

    //float line[] = {
    //    0.0, 0.0, 0.0,
    //    0.0, 1.0, 0.0
    //};

    //unsigned int cubeVBO, cubeVAO;
    //glGenVertexArrays(1, &cubeVAO);
    //glGenBuffers(1, &cubeVBO);

    //glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //glBindVertexArray(cubeVAO);

    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    //glEnableVertexAttribArray(2);

    //unsigned int lightCubeVAO;
    //glGenVertexArrays(1, &lightCubeVAO);
    //glBindVertexArray(lightCubeVAO);

    //glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(0);

    //unsigned int planeVAO, planeVBO;
    //glGenVertexArrays(1, &planeVAO);
    //glGenBuffers(1, &planeVBO);

    //glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

    //glBindVertexArray(planeVAO);

    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    //glEnableVertexAttribArray(2);

    //unsigned int lineVBO, lineVAO;
    //glGenVertexArrays(1, &lineVAO);
    //glGenBuffers(1, &lineVBO);

    //glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

    //glBindVertexArray(lineVAO);

    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    //glEnableVertexAttribArray(0);

    //unsigned int diffuseMap = loadTexture("Resources\\Textures\\container2.png");
    //unsigned int specularMap = loadTexture("Resources\\Textures\\container2_specular.png");
    //unsigned int emissionMap = loadTexture("Resources\\Textures\\matrix.png");
    //unsigned int planeDiffuseMap = loadTexture("Resources\\Textures\\planeDiffuse.jpg");
    //unsigned int planeSpecularMap = loadTexture("Resources\\Textures\\planeSpecular.jpg");

    //lightingShader.use();
    //lightingShader.setInt("material.diffuse", 0);
    //lightingShader.setInt("material.specular", 1);
    //lightingShader.setInt("material.emission", 2);

    //planeShader.use();
    //planeShader.setInt("material.diffuse", 0);
    //planeShader.setInt("material.specular", 1);

    //glm::mat4 lineModel = glm::mat4(1.0f);

    //while (!glfwWindowShouldClose(window))
    //{
    //    float currentFrame = static_cast<float>(glfwGetTime());
    //    deltaTime = currentFrame - lastFrame;
    //    lastFrame = currentFrame;

    //    processInput(window);

    //    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //    lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
    //    lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;

    //    lightingShader.use();
    //    lightingShader.setVec3("viewPos", camera.Position);
    //    lightingShader.setFloat("material.shininess", 32.0f);

    //    lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    //    lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    //    lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    //    lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

    //    lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
    //    lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    //    lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    //    lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    //    lightingShader.setFloat("pointLights[0].constant", 1.0f);
    //    lightingShader.setFloat("pointLights[0].linear", 0.09f);
    //    lightingShader.setFloat("pointLights[0].quadratic", 0.032f);

    //    lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
    //    lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
    //    lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
    //    lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
    //    lightingShader.setFloat("pointLights[1].constant", 1.0f);
    //    lightingShader.setFloat("pointLights[1].linear", 0.09f);
    //    lightingShader.setFloat("pointLights[1].quadratic", 0.032f);

    //    lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
    //    lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
    //    lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
    //    lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
    //    lightingShader.setFloat("pointLights[2].constant", 1.0f);
    //    lightingShader.setFloat("pointLights[2].linear", 0.09f);
    //    lightingShader.setFloat("pointLights[2].quadratic", 0.032f);

    //    lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
    //    lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
    //    lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
    //    lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
    //    lightingShader.setFloat("pointLights[3].constant", 1.0f);
    //    lightingShader.setFloat("pointLights[3].linear", 0.09f);
    //    lightingShader.setFloat("pointLights[3].quadratic", 0.032f);

    //    lightingShader.setVec3("pointLights[4].position", lightPos);
    //    lightingShader.setVec3("pointLights[4].ambient", 0.05f, 0.05f, 0.05f);
    //    lightingShader.setVec3("pointLights[4].diffuse", 0.8f, 0.8f, 0.8f);
    //    lightingShader.setVec3("pointLights[4].specular", 1.0f, 1.0f, 1.0f);
    //    lightingShader.setFloat("pointLights[4].constant", 1.0f);
    //    lightingShader.setFloat("pointLights[4].linear", 0.09f);
    //    lightingShader.setFloat("pointLights[4].quadratic", 0.032f);

    //    lightingShader.setVec3("spotLight.position", camera.Position);
    //    lightingShader.setVec3("spotLight.direction", camera.Front);
    //    lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    //    lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    //    lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    //    lightingShader.setFloat("spotLight.constant", 1.0f);
    //    lightingShader.setFloat("spotLight.linear", 0.09f);
    //    lightingShader.setFloat("spotLight.quadratic", 0.032f);
    //    lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    //    lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

    //    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    //    glm::mat4 view = camera.GetViewMatrix();
    //    lightingShader.setMat4("projection", projection);
    //    lightingShader.setMat4("view", view);

    //    glm::mat4 model = glm::mat4(1.0f);
    //    lightingShader.setMat4("model", model);

    //    glActiveTexture(GL_TEXTURE0);
    //    glBindTexture(GL_TEXTURE_2D, diffuseMap);

    //    glActiveTexture(GL_TEXTURE1);
    //    glBindTexture(GL_TEXTURE_2D, specularMap);

    //    glActiveTexture(GL_TEXTURE2);
    //    glBindTexture(GL_TEXTURE_2D, emissionMap);

    //    glBindVertexArray(cubeVAO);

    //    for (unsigned int i = 0; i < 10; i++)
    //    {
    //        glm::mat4 model = glm::mat4(1.0f);
    //        model = glm::translate(model, cubePositions[i]);
    //        float angle = 20.0f * i;
    //        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
    //        lightingShader.setMat4("model", model);
    //        glDrawArrays(GL_TRIANGLES, 0, 36);
    //    }

    //    planeShader.use();
    //    planeShader.setVec3("viewPos", camera.Position);
    //    planeShader.setFloat("material.shininess", 32.0f);

    //    planeShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    //    planeShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    //    planeShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    //    planeShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

    //    planeShader.setVec3("pointLights[0].position", pointLightPositions[0]);
    //    planeShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    //    planeShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    //    planeShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    //    planeShader.setFloat("pointLights[0].constant", 1.0f);
    //    planeShader.setFloat("pointLights[0].linear", 0.09f);
    //    planeShader.setFloat("pointLights[0].quadratic", 0.032f);

    //    planeShader.setVec3("pointLights[1].position", pointLightPositions[1]);
    //    planeShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
    //    planeShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
    //    planeShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
    //    planeShader.setFloat("pointLights[1].constant", 1.0f);
    //    planeShader.setFloat("pointLights[1].linear", 0.09f);
    //    planeShader.setFloat("pointLights[1].quadratic", 0.032f);

    //    planeShader.setVec3("pointLights[2].position", pointLightPositions[2]);
    //    planeShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
    //    planeShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
    //    planeShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
    //    planeShader.setFloat("pointLights[2].constant", 1.0f);
    //    planeShader.setFloat("pointLights[2].linear", 0.09f);
    //    planeShader.setFloat("pointLights[2].quadratic", 0.032f);

    //    planeShader.setVec3("pointLights[3].position", pointLightPositions[3]);
    //    planeShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
    //    planeShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
    //    planeShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
    //    planeShader.setFloat("pointLights[3].constant", 1.0f);
    //    planeShader.setFloat("pointLights[3].linear", 0.09f);
    //    planeShader.setFloat("pointLights[3].quadratic", 0.032f);

    //    planeShader.setVec3("pointLights[4].position", lightPos);
    //    planeShader.setVec3("pointLights[4].ambient", 0.05f, 0.05f, 0.05f);
    //    planeShader.setVec3("pointLights[4].diffuse", 0.8f, 0.8f, 0.8f);
    //    planeShader.setVec3("pointLights[4].specular", 1.0f, 1.0f, 1.0f);
    //    planeShader.setFloat("pointLights[4].constant", 1.0f);
    //    planeShader.setFloat("pointLights[4].linear", 0.09f);
    //    planeShader.setFloat("pointLights[4].quadratic", 0.032f);

    //    planeShader.setVec3("spotLight.position", camera.Position);
    //    planeShader.setVec3("spotLight.direction", camera.Front);
    //    planeShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    //    planeShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    //    planeShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    //    planeShader.setFloat("spotLight.constant", 1.0f);
    //    planeShader.setFloat("spotLight.linear", 0.09f);
    //    planeShader.setFloat("spotLight.quadratic", 0.032f);
    //    planeShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    //    planeShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

    //    projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    //    view = camera.GetViewMatrix();
    //    planeShader.setMat4("projection", projection);
    //    planeShader.setMat4("view", view);

    //    model = glm::mat4(1.0f);
    //    planeShader.setMat4("model", model);

    //    glActiveTexture(GL_TEXTURE0);
    //    glBindTexture(GL_TEXTURE_2D, planeDiffuseMap);

    //    glActiveTexture(GL_TEXTURE1);
    //    glBindTexture(GL_TEXTURE_2D, planeSpecularMap);

    //    for (int i = 0; i < 7; i++)
    //    {
    //        glm::mat4 model = glm::mat4(1.0f);
    //        glBindVertexArray(planeVAO);
    //        switch (i)
    //        {
    //        case 0:
    //            model = glm::translate(model, glm::vec3(0.0f, -4.5f, 0.0f));
    //            break;
    //        case 1:
    //            model = glm::translate(model, glm::vec3(4.5f, 0.0f, 0.0f));
    //            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    //            break;
    //        case 2:
    //            model = glm::translate(model, glm::vec3(0.0f, 0.0f, -14.5f));
    //            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    //            break;
    //        case 3:
    //            model = glm::translate(model, glm::vec3(-4.5f, 0.0f, 0.0f));
    //            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    //            break;
    //        case 4:
    //            model = glm::translate(model, glm::vec3(4.5f, 0.0f, -10.0f));
    //            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    //            break;
    //        case 5:
    //            model = glm::translate(model, glm::vec3(-4.5f, 0.0f, -10.0f));
    //            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    //            break;
    //        case 6:
    //            model = glm::translate(model, glm::vec3(0.0f, -4.5f, -10.0f));
    //            break;
    //        }
    //        planeShader.setMat4("model", model);
    //        glDrawArrays(GL_TRIANGLES, 0, 6);
    //    }

    //    lightCubeShader.use();
    //    lightCubeShader.setMat4("projection", projection);
    //    lightCubeShader.setMat4("view", view);

    //    glBindVertexArray(lightCubeVAO);
    //    for (unsigned int i = 0; i < 5; i++)
    //    {
    //        model = glm::mat4(1.0f);
    //        if (i == 4)
    //        {
    //            model = glm::translate(model, lightPos);
    //        }
    //        else
    //        {
    //            model = glm::translate(model, pointLightPositions[i]);
    //        }
    //        model = glm::scale(model, glm::vec3(0.2f));
    //        lightCubeShader.setMat4("model", model);
    //        glDrawArrays(GL_TRIANGLES, 0, 36);
    //    }

    //    lineShader.use();
    //    lineShader.setMat4("projection", projection);
    //    lineShader.setMat4("view", view);

    //    glBindVertexArray(lineVAO);
    //    float intersectionDistance;
    //    glm::vec3 aabb_min(-1.0f, -1.0f, -1.0f);
    //    glm::vec3 aabb_max(1.0f, 1.0f, 1.0f);
    //    glm::vec3 rayOrigin;
    //    glm::vec3 rayDir;
    //    model = glm::mat4(1.0f);
    //    model = glm::translate(model, cubePositions[0]);
    //    rayOrigin = camera.Position;
    //    rayDir = camera.Front;
    //    glm::vec3 currentDir;
    //    if (TestRayOBBIntersection(
    //        rayOrigin,
    //        rayDir,
    //        aabb_min,
    //        aabb_max,
    //        model,
    //        intersectionDistance
    //    ))
    //    {

    //    }

    //    if (spacePress && firstPress)
    //    {
    //        lineModel = glm::inverse(camera.GetViewMatrix());
    //        lineModel = glm::rotate(lineModel, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
    //        firstPress = false;
    //        lineModel = glm::scale(lineModel, glm::vec3(intersectionDistance + 0.5f));
    //        currentDir = rayDir;
    //        currentDir = currentDir * intersectionDistance;
    //    }

    //    lineShader.setMat4("model", lineModel);

    //    glDrawArrays(GL_LINES, 0, 2);

    //    std::cout << intersectionDistance << std::endl;
    //    glm::mat4 reflectModel = lineModel;
    //    reflectModel = glm::translate(reflectModel, glm::vec3(0.0f, 1.0f, 0.0f));
    //    reflectModel = glm::rotate(reflectModel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));


    //    lineShader.setMat4("model", reflectModel);

    //    glDrawArrays(GL_LINES, 0, 2);

    //    glfwSwapBuffers(window);
    //    glfwPollEvents();
    //}

    //glDeleteVertexArrays(1, &cubeVAO);
    //glDeleteVertexArrays(1, &lightCubeVAO);
    //glDeleteBuffers(1, &cubeVBO);

#pragma endregion

    int max_compute_work_group_count[3];
    int max_compute_work_group_size[3];
    int max_compute_work_group_invocations;

    for (int idx = 0; idx < 3; idx++)
    {
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, idx, &max_compute_work_group_count[idx]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, idx, &max_compute_work_group_size[idx]);
    }
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &max_compute_work_group_invocations);

    std::cout << "OpenGL Limitations: " << std::endl;
    std::cout << "maximum number of work groups in X dimension " << max_compute_work_group_count[0] << std::endl;
    std::cout << "maximum number of work groups in Y dimension " << max_compute_work_group_count[1] << std::endl;
    std::cout << "maximum number of work groups in Z dimension " << max_compute_work_group_count[2] << std::endl;

    std::cout << "maximum size of a work group in X dimension " << max_compute_work_group_size[0] << std::endl;
    std::cout << "maximum size of a work group in Y dimension " << max_compute_work_group_size[1] << std::endl;
    std::cout << "maximum size of a work group in Z dimension " << max_compute_work_group_size[2] << std::endl;

    std::cout << "Number of invocations in a single local work group that may be dispatched to a compute shader " << max_compute_work_group_invocations << std::endl;

    Shader screenQuad("Shaders\\screenquad.vs", "Shaders\\screenquad.fs");
    ComputeShader rayShader("Shaders\\raycompute.cs");

    screenQuad.use();
    screenQuad.setInt("tex", 0);

    unsigned int texture;

    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

    glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    int fCounter = 0;
    while (!glfwWindowShouldClose(window))
    {
        // Set frame time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        if (fCounter > 60) {
            std::cout << "FPS: " << 1 / deltaTime << std::endl;
            fCounter = 0;
        }
        else {
            fCounter++;
        }

        rayShader.use();
        glDispatchCompute((unsigned int)TEXTURE_WIDTH, (unsigned int)TEXTURE_HEIGHT, 1);

        // make sure writing to image has finished before read
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // render image to quad
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        screenQuad.use();

        renderQuad();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) { glfwSetWindowShouldClose(window, 1); }
    }

    glDeleteTextures(1, &texture);
    glDeleteProgram(screenQuad.ID);
    glDeleteProgram(rayShader.ID);

    glfwTerminate();
    return 0;
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

bool TestRayOBBIntersection(
    glm::vec3 ray_origin,        // Ray origin, in world space
    glm::vec3 ray_direction,     // Ray direction (NOT target position!), in world space. Must be normalize()'d.
    glm::vec3 aabb_min,          // Minimum X,Y,Z coords of the mesh when not transformed at all.
    glm::vec3 aabb_max,          // Maximum X,Y,Z coords. Often aabb_min*-1 if your mesh is centered, but it's not always the case.
    glm::mat4 ModelMatrix,       // Transformation applied to the mesh (which will thus be also applied to its bounding box)
    float& intersection_distance // Output : distance between ray_origin and the intersection with the OBB
) {

    // Intersection method from Real-Time Rendering and Essential Mathematics for Games

    float tMin = 0.0f;
    float tMax = 100000.0f;

    glm::vec3 OBBposition_worldspace(ModelMatrix[3].x, ModelMatrix[3].y, ModelMatrix[3].z);

    glm::vec3 delta = OBBposition_worldspace - ray_origin;

    // Test intersection with the 2 planes perpendicular to the OBB's X axis
    {
        glm::vec3 xaxis(ModelMatrix[0].x, ModelMatrix[0].y, ModelMatrix[0].z);
        float e = glm::dot(xaxis, delta);
        float f = glm::dot(ray_direction, xaxis);

        if (fabs(f) > 0.001f) { // Standard case

            float t1 = (e + aabb_min.x) / f; // Intersection with the "left" plane
            float t2 = (e + aabb_max.x) / f; // Intersection with the "right" plane
            // t1 and t2 now contain distances betwen ray origin and ray-plane intersections

            // We want t1 to represent the nearest intersection, 
            // so if it's not the case, invert t1 and t2
            if (t1 > t2) {
                float w = t1; t1 = t2; t2 = w; // swap t1 and t2
            }

            // tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
            if (t2 < tMax)
                tMax = t2;
            // tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
            if (t1 > tMin)
                tMin = t1;

            // And here's the trick :
            // If "far" is closer than "near", then there is NO intersection.
            // See the images in the tutorials for the visual explanation.
            if (tMax < tMin)
                return false;

        }
        else { // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
            if (-e + aabb_min.x > 0.0f || -e + aabb_max.x < 0.0f)
                return false;
        }
    }


    // Test intersection with the 2 planes perpendicular to the OBB's Y axis
    // Exactly the same thing than above.
    {
        glm::vec3 yaxis(ModelMatrix[1].x, ModelMatrix[1].y, ModelMatrix[1].z);
        float e = glm::dot(yaxis, delta);
        float f = glm::dot(ray_direction, yaxis);

        if (fabs(f) > 0.001f) {

            float t1 = (e + aabb_min.y) / f;
            float t2 = (e + aabb_max.y) / f;

            if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }

            if (t2 < tMax)
                tMax = t2;
            if (t1 > tMin)
                tMin = t1;
            if (tMin > tMax)
                return false;

        }
        else {
            if (-e + aabb_min.y > 0.0f || -e + aabb_max.y < 0.0f)
                return false;
        }
    }


    // Test intersection with the 2 planes perpendicular to the OBB's Z axis
    // Exactly the same thing than above.
    {
        glm::vec3 zaxis(ModelMatrix[2].x, ModelMatrix[2].y, ModelMatrix[2].z);
        float e = glm::dot(zaxis, delta);
        float f = glm::dot(ray_direction, zaxis);

        if (fabs(f) > 0.001f) {

            float t1 = (e + aabb_min.z) / f;
            float t2 = (e + aabb_max.z) / f;

            if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }

            if (t2 < tMax)
                tMax = t2;
            if (t1 > tMin)
                tMin = t1;
            if (tMin > tMax)
                return false;

        }
        else {
            if (-e + aabb_min.z > 0.0f || -e + aabb_max.z < 0.0f)
                return false;
        }
    }

    intersection_distance = tMin;
    return true;

}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        spacePress = true;
    }
    else
    {
        spacePress = false;
        firstPress = true;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
        {
            format = GL_RED;
        }
        else if (nrComponents == 3)
        {
            format = GL_RGB;
        }
        else if (nrComponents == 4)
        {
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        std::cout << "Loaded texture at path: " << path << " width " << width << " id " << textureID << " format " << format << std::endl;
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
