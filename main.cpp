#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "classes/stb_image.h"

#include <vector>
#include <filesystem>
#include <iostream>
#include <sstream>
#include "classes/Scene.h"

#include "classes/Util.h"
#include "classes/GLFW_Util.h"

//Declare callback functions
void framebuffer_size_callback(GLFWwindow* w, int width, int height);
void mouse_callback(GLFWwindow* w, double xpos, double ypos);
void scroll_callback(GLFWwindow* w, double xoffset, double yoffset);

// Window dimensions
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//Global object references
GLFWwindow* window;
Camera* camera;
Scene scene;

//Set up GLFW
int initGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    scene = Scene(window);
    scene.addCamera("main");
    camera = scene.getCamera("main");

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    return 1;
}

//Main function
int main() {

    initGLFW();

    //Load shaders and models from file
    GLFW_Util::loadShaders(&scene, "config/shader_list.txt");
    GLFW_Util::loadModels(&scene, "config/model_list.txt");
    GLFW_Util::loadPointLights(&scene, "config/pointlight_list.txt");
    GLFW_Util::loadDirectionalLights(&scene, "config/dirlight_list.txt");

    //Assign reference vars to shaders for convenience
    Shader* shader_standard = scene.getShader("standard");
    Shader* shader_quad = scene.getShader("quad");
    Shader* shader_depth = scene.getShader("depth");

    //Assign shaders to models
    scene.assignShader("floor", "standard");
    scene.assignShader("wall", "standard");

    std::vector buffer_objects = GLFW_Util::createFrameBuffer(SCR_WIDTH, SCR_HEIGHT);
    unsigned int FBO = buffer_objects[0];
    unsigned int TBO = buffer_objects[1];
    unsigned int RBO = buffer_objects[2];

    std::vector<unsigned int> quad_objects = GLFW_Util::createRenderQuad();
    unsigned int quadVAO = quad_objects[0];
    unsigned int quadVBO = quad_objects[1];

    float angle = 0.0f;

    // render loop
    while (!glfwWindowShouldClose(window)) {

        angle += 0.1f;

        //Initial render to texture buffer object (TBO) using frame buffer object (FBO)
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene.getModel("floor")->setRotation(angle, glm::vec3(0.0f, 1.0f, 0.0f));

        scene.prepareShaders();
        scene.prepareLights("standard");
        scene.renderScene();

        //Final render to output quad
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader_quad->use();
        glBindVertexArray(quadVAO);

        shader_quad->setInt("TBO", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TBO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        GLFW_Util::processInput(window, camera);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteFramebuffers(1, &FBO);
    scene.clearAll();

    glfwTerminate();
    return 0;
}

//Callback functions

void framebuffer_size_callback(GLFWwindow* w, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* w, double xpos, double ypos) {
    camera->mouseCallback(xpos, ypos);
}

void scroll_callback(GLFWwindow* w, double xoffset, double yoffset) {
    camera->scrollCallback(xoffset, yoffset);
}