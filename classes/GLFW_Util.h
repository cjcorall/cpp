#ifndef GLFW_UTIL_H
#define GLFW_UTIL_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"

#include <vector>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <utility>
#include "Shader.h"
#include "Camera.h"
#include "Scene.h"

class GLFW_Util{

        public:
            static void processInput(GLFWwindow* w, Camera* c);

            static unsigned int loadCubemap(std::vector<std::string> faces);
            static unsigned int loadTexture(std::string filename);
            static void loadShaders(Scene* s, const char* file_path);
            static void loadModels(Scene* s, const char* file_path);
            static void loadPointLights(Scene* s, const char* file_path);
            static void loadDirectionalLights(Scene* s, const char* file_path);

            static std::vector<unsigned int> createFrameBuffer(const unsigned int w, const unsigned int h);
            static std::vector<unsigned int> createRenderQuad();

        private:

};

#endif