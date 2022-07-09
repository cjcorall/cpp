

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "classes/stb_image.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <filesystem>
#include <iostream>
#include <sstream>
#include "classes/Scene.h"

void framebuffer_size_callback(GLFWwindow* w, int width, int height);
void processInput(GLFWwindow* w);
void unbindVertexArrays();
void mouse_callback(GLFWwindow* w, double xpos, double ypos);
void scroll_callback(GLFWwindow* w, double xoffset, double yoffset);

unsigned int loadTexture(std::string filename);
unsigned int loadCubemap(std::vector<std::string> faces);
void renderQuad();
void renderCube();
void renderScene(Shader* shader);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

GLFWwindow* window;
Camera* camera;

Scene scene;

unsigned int planeVAO;

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

int main() {
    // glfw: initialize and configure
    initGLFW();

    // build and compile our shader program
    
    scene.addShader("standard", "shaders/vertex_standard.glsl", "shaders/fragment_standard.glsl");
    Shader* shader_standard = scene.getShader("standard");
    //scene.addShader("light", "shaders/vertex_light.glsl", "shaders/fragment_light.glsl");
    //scene.addShader("flat", "shaders/vertex_flat.glsl", "shaders/fragment_flat.glsl");
    scene.addShader("quad", "shaders/vertex_quad.glsl", "shaders/fragment_quad.glsl");
    scene.addShader("skybox", "shaders/vertex_skybox.glsl", "shaders/fragment_skybox.glsl");
    scene.addShader("depth", "shaders/vertex_depth.glsl", "shaders/fragment_depth.glsl");
    Shader* shader_depthcube = new Shader("shaders/vertex_depthcube.glsl", "shaders/fragment_depthcube.glsl", "shaders/geometry_depthcube.glsl");
    Shader* shader_quad = scene.getShader("quad");
    Shader* shader_skybox = scene.getShader("skybox");
    Shader* shader_depth = scene.getShader("depth");

    //set up vertex data(and buffer(s)) and configure vertex attributes
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };
    /*
    scene.addModel("gun", "obj/cube.obj");
    scene.getModel("gun")->setPosition(glm::vec3(0.0f, 0.35f, 0.0f));
    scene.getModel("gun")->setScale(glm::vec3(0.25f));
    scene.getModel("gun")->setColor(glm::vec3(0.4f));
    //scene.assignShader("gun", "depth");

    scene.addModel("floor", "obj/plane.obj");
    scene.getModel("floor")->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    scene.getModel("floor")->setScale(glm::vec3(5.0f));
    scene.getModel("floor")->setColor(glm::vec3(1.0f));
    //scene.assignShader("floor", "depth");
    
    
    scene.addModel("plight", "obj/cube.obj");
    Model* plight = scene.getModel("plight");
    plight->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
    plight->setPosition(glm::vec3(-0.05f, 0.8f, 0.0f));
    plight->setScale(glm::vec3(0.1f));
    //scene.assignShader("plight", "depth");
    */

    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    //Positions Array
    glm::vec3 obj_positions[] = {
        glm::vec3(0.0f,0.0f,0.0f),
        glm::vec3(1.0f,0.0f,-1.0f),
        glm::vec3(-1.0f,0.0f,-1.0f),
        glm::vec3(0.0f,1.0f,0.0f),
        glm::vec3(0.0f,-1.0f,-0.5f)
    };

    //Point Light Values
    glm::vec3 plight_positions[] = {
        glm::vec3(0.0f,0.0f,0.0f),
        glm::vec3(100.0f,0.0f,-3.0f),
        glm::vec3(-100.0f,3.5f,1.0f),
        glm::vec3(100.0f,0.0f,1.5f)
    };

    glm::vec3 plight_diffuse[] = {
        glm::vec3(0.65f,0.35f,0.35f),
        glm::vec3(0.15f,0.15f,0.05f),
        glm::vec3(0.05f,0.05f,0.05f),
        glm::vec3(0.15f,0.15f,0.15f)
    };


    //Light inputs
    scene.addDirectionalLight("main", glm::vec3(0.1f, -1.0f, -0.5f), glm::vec3(0.15f), glm::vec3(0.05f), glm::vec3(1.0f));

    for (int i = 0; i < 4; i++) {
        std::stringstream ss;
        ss << i;
        std::string pname = "plight_" + ss.str();
        scene.addPointLight(pname, plight_positions[i], glm::vec3(0.075f), plight_diffuse[i], glm::vec3(1.0f), 1.0f, 0.09f, 0.032f);
    }

    float light_phi = glm::cos(glm::radians(15.0f));
    float light_gamma = glm::cos(glm::radians(30.0f));

    //Material inputs
    glm::vec3 material_ambient = glm::vec3(0.4f, 0.9f, 0.7f);
    glm::vec3 material_diffuse = glm::vec3(0.4f, 0.9f, 0.7f);
    glm::vec3 material_specular = glm::vec3(0.5f, 0.5f, 0.5f);

    //glDepthMask(GL_FALSE);


    unsigned int FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    unsigned int TBO;
    glGenTextures(1, &TBO);
    glBindTexture(GL_TEXTURE_2D, TBO);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TBO, 0);

    unsigned int RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    float quad_vertices[] = {
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };

    glBindVertexArray(0);

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    unsigned int depthMapCube;
    glGenTextures(1, &depthMapCube);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthMapCube);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMapCube, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glm::vec3 light_pos = glm::vec3(-0.0000001f, 0.0f, 0.0f);
    float near_plane = 1.0f, far_plane = 25.0f;

    Model cube1("obj/cube.obj");
    cube1.setPosition(glm::vec3(4.3f, 4.0f, 4.3f));
    cube1.setScale(glm::vec3(0.15f));

    Model cube2("obj/cube.obj");
    cube2.setPosition(glm::vec3(4.30f, 4.60f, 4.30f));
    cube2.setScale(glm::vec3(0.1f));

    Model cube3("obj/cube.obj");
    cube3.setPosition(glm::vec3(4.20f, 4.10f, 4.20f));
    cube3.setScale(glm::vec3(0.23f));

    Model cube4("obj/cube.obj");
    cube4.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    cube4.setScale(glm::vec3(5.0f));

    glm::mat4 model = glm::mat4(1.0f);
    float angle = 0.0f;
    float dx = 0.0f;
    float dz = 0.0f;
    float radius = 0.45f;
    
    float aspect = ((float)SHADOW_WIDTH / (float)SHADOW_HEIGHT);
    float near_clip = 1.0f;
    float far_clip = 25.0f;
    glm::mat4 shadow_proj = glm::perspective(glm::radians(90.0f), aspect, near_clip, far_clip);

    std::vector<glm::mat4> shadow_transforms;

    shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
    shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

    


    // render looping
    while (!glfwWindowShouldClose(window))
    {
        // input
        
        dx = cos(angle * (float)M_PI / 180.0f) * radius;
        dz = sin(angle * (float)M_PI / 180.0f) * radius;
        angle += 0.08f;
        cube1.setPosition(glm::vec3(4.3f + dx, 4.5f, 4.3f + dz));
        //cube.setScale(glm::vec3(std::abs(dx * 1.2f)));

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 light_proj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        glm::mat4 light_view = glm::lookAt(light_pos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 mat_light = light_proj * light_view;

        //Capture point lights' cube shadow map
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        
        shader_depthcube->use();
        glCullFace(GL_FRONT);

        for (unsigned int i = 0; i < 6; ++i) {
            std::stringstream face_name("mat_shadow[", std::ios_base::ate | std::ios_base::in | std::ios_base::out);
            face_name << i << "]";
            //std::cout << face_name.str() << std::endl;
            shader_depthcube->setMatrix(face_name.str().c_str(), shadow_transforms[i]);
            face_name.str("");
        }

        shader_depthcube->setFloat("far_plane", far_plane);
        shader_depthcube->setVector("lightPos", light_pos);

        shader_depthcube->use();
        glDisable(GL_CULL_FACE);
        model = glm::mat4(1.0f);
        model = glm::translate(model, cube4.getPosition());
        model = glm::scale(model, cube4.getScale());
        shader_depthcube->setMatrix("mat_model", model);
        cube4.draw(*shader_depthcube);
        glEnable(GL_CULL_FACE);

        shader_depthcube->use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, cube1.getPosition());
        model = glm::scale(model, cube1.getScale());
        shader_depthcube->setMatrix("mat_model", model);
        cube1.draw(*shader_depthcube);

        shader_depthcube->use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, cube2.getPosition());
        model = glm::scale(model, cube2.getScale());
        shader_depthcube->setMatrix("mat_model", model);
        cube2.draw(*shader_depthcube);

        shader_depthcube->use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, cube3.getPosition());
        model = glm::scale(model, cube3.getScale());
        shader_depthcube->setMatrix("mat_model", model);
        cube3.draw(*shader_depthcube);

        glCullFace(GL_BACK);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        /*
        //Capture directional light shadow map
        glCullFace(GL_FRONT);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        shader_depth->use();

        shader_depth->setInt("depthMap", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);

        shader_depth->setMatrix("mat_light", mat_light);
        model = glm::mat4(1.0f);
        model = glm::translate(model, floor.getPosition());
        model = glm::scale(model, floor.getScale());
        shader_depth->setMatrix("mat_model", model);
        floor.draw(*shader_depth);

        shader_depth->use();
        shader_depth->setMatrix("mat_light", mat_light);
        model = glm::translate(model, gun.getPosition());
        model = glm::scale(model, gun.getScale());
        shader_depth->setMatrix("mat_model", model);
        gun.draw(*shader_depth);

        shader_depth->use();
        shader_depth->setMatrix("mat_light", mat_light);
        model = glm::mat4(1.0f);
        model = glm::translate(model, cube.getPosition());
        model = glm::scale(model, cube.getScale());
        shader_depth->setMatrix("mat_model", model);
        cube.draw(*shader_depth);

        glCullFace(GL_BACK);
        //scene.renderScene();
        */

        //Set up standard render
            
        
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        processInput(window);

        shader_standard->use();
        shader_standard->setFloat("near_plane", near_plane);
        shader_standard->setFloat("far_plane", far_plane);
        shader_standard->setInt("depthMap", 3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthMapCube);
        shader_standard->setVector("cameraPos", camera->getPosition());
        shader_standard->setMatrix("mat_view", camera->getView());
        shader_standard->setMatrix("mat_proj", camera->getProjection());
        shader_standard->setVector("lightPos", light_pos);
        shader_standard->setInt("material.diffuse", 0);
        shader_standard->setInt("material.specular", 1);
        shader_standard->setFloat("material.shininess", 256.0f);
        scene.prepareLights("standard");

        shader_standard->use();
        glDisable(GL_CULL_FACE);
        model = glm::mat4(1.0f);
        model = glm::translate(model, cube4.getPosition());
        model = glm::scale(model, cube4.getScale());
        shader_standard->setInt("reverse_normals", 1);
        shader_standard->setMatrix("mat_model", model);
        shader_standard->setInt("has_diffuse", (int)cube4.hasDiffuse());
        shader_standard->setInt("has_specular", (int)cube4.hasSpecular());
        shader_standard->setVector("output_color", cube4.getColor());
        cube4.draw(*shader_standard);
        shader_standard->setInt("reverse_normals", 0);
        glEnable(GL_CULL_FACE);

        shader_standard->use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, cube1.getPosition());
        model = glm::scale(model, cube1.getScale());
        shader_standard->setMatrix("mat_model", model);
        shader_standard->setInt("has_diffuse", (int)cube1.hasDiffuse());
        shader_standard->setInt("has_specular", (int)cube1.hasSpecular());
        shader_standard->setVector("output_color", cube1.getColor());
        cube1.draw(*shader_standard);

        shader_standard->use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, cube2.getPosition());
        model = glm::scale(model, cube2.getScale());
        shader_standard->setMatrix("mat_model", model);
        shader_standard->setInt("has_diffuse", (int)cube2.hasDiffuse());
        shader_standard->setInt("has_specular", (int)cube2.hasSpecular());
        shader_standard->setVector("output_color", cube2.getColor());
        cube2.draw(*shader_standard);

        shader_standard->use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, cube3.getPosition());
        model = glm::scale(model, cube3.getScale());
        shader_standard->setMatrix("mat_model", model);
        shader_standard->setInt("has_diffuse", (int)cube3.hasDiffuse());
        shader_standard->setInt("has_specular", (int)cube3.hasSpecular());
        shader_standard->setVector("output_color", cube3.getColor());
        cube3.draw(*shader_standard);

        scene.renderScene();
        
        unbindVertexArrays();

        //Final render to output quad
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader_quad->use();
        glBindVertexArray(quadVAO);
        
        shader_quad->setFloat("near_plane", near_plane);
        shader_quad->setFloat("far_plane", far_plane);
        shader_quad->setInt("TBO", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TBO);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteFramebuffers(1, &FBO);
    glDeleteFramebuffers(1, &depthMapFBO);
    scene.clearAll();
    delete shader_depthcube;

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* w)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera->moveForward();
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera->moveBackward();
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera->moveLeft();
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera->moveRight();
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera->moveDown();
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera->moveUp();
    }
}

void framebuffer_size_callback(GLFWwindow* w, int width, int height)
{
    glViewport(0, 0, width, height);
}

void unbindVertexArrays() {
    glBindVertexArray(0);
}

void mouse_callback(GLFWwindow* w, double xpos, double ypos) {
    camera->mouseCallback(xpos, ypos);
}

void scroll_callback(GLFWwindow* w, double xoffset, double yoffset) {
    camera->scrollCallback(xoffset, yoffset);
}

unsigned int loadCubemap(std::vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, channels;
    std::vector<unsigned int> sides = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };
    for (unsigned int i = 0; i < faces.size(); i++) {
        stbi_set_flip_vertically_on_load(0);
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);

        if (data) {
            glTexImage2D(sides[i], 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Failed to load skybox texture!" << std::endl;
            stbi_image_free(data);
            return 0;
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
     
    }
    return textureID;
}

unsigned int loadTexture(std::string filename){
    unsigned int texture_id;
	glGenTextures(1, &texture_id);

	int width, height, num_components;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &num_components, 0);
	if (data) {
		GLenum format = GL_RGB;
		if (num_components == 1) {
			format = GL_RED;
		} else if (num_components == 3) {
			format = GL_RGB;
		} else if (num_components == 4) {
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);

	} else {
		std::cout << "Texture load failed." << std::endl;
		stbi_image_free(data);
	}

	return texture_id;
}
