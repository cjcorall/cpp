#include "GLFW_Util.h"

void GLFW_Util::processInput(GLFWwindow* w, Camera* c)
{
    if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(w, true);

    if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS) {
        c->moveForward();
    }
    if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS) {
        c->moveBackward();
    }
    if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS) {
        c->moveLeft();
    }
    if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS) {
        c->moveRight();
    }
    if (glfwGetKey(w, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        c->moveDown();
    }
    if (glfwGetKey(w, GLFW_KEY_SPACE) == GLFW_PRESS) {
        c->moveUp();
    }
}

unsigned int GLFW_Util::loadCubemap(std::vector<std::string> faces) {
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


unsigned int GLFW_Util::loadTexture(std::string filename){
    unsigned int texture_id;
	glGenTextures(1, &texture_id);

	int width, height, num_components;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &num_components, 0);
	if (data) {
		GLenum format;
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

void GLFW_Util::loadShaders(Scene* s, const char* file_path){
    std::ifstream infile(file_path);
    std::string line;
    std::string name;
    std::string vertex_path;
    std::string fragment_path;
	while (std::getline(infile, line)) {
        name = line;
        vertex_path = "shaders/vertex_" + name + ".glsl";
        fragment_path = "shaders/fragment_" + name + ".glsl";
        std::cout << name.c_str() << " :: " << vertex_path.c_str() << " :: " << fragment_path.c_str() << std::endl;
        s->addShader(name.c_str(), vertex_path.c_str(), fragment_path.c_str());
	}
}

void GLFW_Util::loadModels(Scene* s, const char* file_path){
    std::ifstream infile(file_path);
    
    std::string line;
    std::string segment;
    bool first_line = true;

    std::string model_name;
    std::string obj_path;
    glm::vec3 position;
    float angle;
    glm::vec3 axis;
    glm::vec3 scale;

    enum headers{
        NAME,
        PATH,
        POSITION,
        ROTATION,
        SCALE
    };

	while (std::getline(infile, line)) {
        if(!first_line){
            std::stringstream components(line);
            int header_index = 0;
            while(std::getline(components,segment,'\t')){
                std::string digit;
                if(header_index == headers::NAME){
                    model_name = segment;
                }else if(header_index == headers::PATH){
                    obj_path = segment;
                }else if(header_index == headers::POSITION){
                    std::stringstream pos_ss(segment);
                    std::vector<float> pos_xyz;
                    while(std::getline(pos_ss,digit,' ')){
                        pos_xyz.push_back(std::stof(digit));
                    }
                    position = glm::vec3(pos_xyz[0], pos_xyz[1], pos_xyz[2]);
                }else if(header_index == headers::ROTATION){
                    std::stringstream rot_ss(segment);
                    std::vector<float> rot_wxyz;
                    while(std::getline(rot_ss,digit,' ')){
                        rot_wxyz.push_back(std::stof(digit));
                    }
                    angle = rot_wxyz[0];
                    axis = glm::vec3(rot_wxyz[1], rot_wxyz[2], rot_wxyz[3]);
                }else if(header_index == headers::SCALE){
                    std::stringstream scale_ss(segment);
                    std::vector<float> scale_xyz;
                    while(std::getline(scale_ss,digit,' ')){
                        scale_xyz.push_back(std::stof(digit));
                    }
                    scale = glm::vec3(scale_xyz[0], scale_xyz[1], scale_xyz[2]);
                }
                header_index++;
            }

            s->addModel(model_name.c_str(), obj_path.c_str());
            Model* m = s->getModel(model_name.c_str());
            m->setPosition(position);
            m->setRotation(angle, axis);
            m->setScale(scale);
        }else{
            first_line = false;
        }
	}
}

void GLFW_Util::loadPointLights(Scene* s, const char* file_path){
    std::ifstream infile(file_path);
    
    std::string line;
    std::string segment;
    bool first_line = true;

    std::string light_name;
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float kc;
    float kl;
    float kq;

    enum headers{
        NAME,
        POSITION,
        AMBIENT,
        DIFFUSE,
        SPECULAR,
        KC,
        KL,
        KQ
    };

    int light_num = 0;

	while (std::getline(infile, line)) {
        if(!first_line){
            std::stringstream components(line);
            int header_index = 0;
            while(std::getline(components,segment,'\t')){
                std::string digit;
                if(header_index == headers::NAME){
                    std::stringstream name_ss(segment);
                    name_ss << light_num;
                    light_num++;
                    light_name = name_ss.str();
                }else if(header_index == headers::POSITION){
                    std::stringstream pos_ss(segment);
                    std::vector<float> pos_xyz;
                    while(std::getline(pos_ss,digit,' ')){
                        pos_xyz.push_back(std::stof(digit));
                    }
                    position = glm::vec3(pos_xyz[0], pos_xyz[1], pos_xyz[2]);
                }else if(header_index == headers::AMBIENT){
                    std::stringstream amb_ss(segment);
                    std::vector<float> amb_xyz;
                    while(std::getline(amb_ss,digit,' ')){
                        amb_xyz.push_back(std::stof(digit));
                    }
                    ambient = glm::vec3(amb_xyz[0], amb_xyz[1], amb_xyz[2]);
                }else if(header_index == headers::DIFFUSE){
                    std::stringstream diff_ss(segment);
                    std::vector<float> diff_xyz;
                    while(std::getline(diff_ss,digit,' ')){
                        diff_xyz.push_back(std::stof(digit));
                    }
                    diffuse = glm::vec3(diff_xyz[0], diff_xyz[1], diff_xyz[2]);
                } else if (header_index == headers::SPECULAR) {
                    std::stringstream spec_ss(segment);
                    std::vector<float> spec_xyz;
                    while (std::getline(spec_ss, digit, ' ')) {
                        spec_xyz.push_back(std::stof(digit));
                    }
                    specular = glm::vec3(spec_xyz[0], spec_xyz[1], spec_xyz[2]);
                } else if(header_index == headers::KC){
                    std::stringstream kc_ss(segment);
                    kc = std::stof(kc_ss.str());
                }else if(header_index == headers::KL){
                    std::stringstream kl_ss(segment);
                    kl = std::stof(kl_ss.str());
                }else if(header_index == headers::KQ){
                    std::stringstream kq_ss(segment);
                    kq = std::stof(kq_ss.str());
                }
                header_index++;
            }

            s->addPointLight(light_name, position, ambient, diffuse, specular, kc, kl, kq);
        }else{
            first_line = false;
        }
	}
}

void GLFW_Util::loadDirectionalLights(Scene* s, const char* file_path){
    std::ifstream infile(file_path);
    
    std::string line;
    std::string segment;
    bool first_line = true;

    std::string light_name;
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    enum headers{
        NAME,
        DIRECTION,
        AMBIENT,
        DIFFUSE,
        SPECULAR
    };

    int light_num = 0;

	while (std::getline(infile, line)) {
        if(!first_line){
            std::stringstream components(line);
            int header_index = 0;
            while(std::getline(components,segment,'\t')){
                std::string digit;
                if(header_index == headers::NAME){
                    std::stringstream name_ss(segment);
                    name_ss << light_num;
                    light_num++;
                    light_name = name_ss.str();
                }else if(header_index == headers::DIRECTION){
                    std::stringstream dir_ss(segment);
                    std::vector<float> dir_xyz;
                    while(std::getline(dir_ss,digit,' ')){
                        dir_xyz.push_back(std::stof(digit));
                    }
                    direction = glm::vec3(dir_xyz[0], dir_xyz[1], dir_xyz[2]);
                }else if(header_index == headers::AMBIENT){
                    std::stringstream amb_ss(segment);
                    std::vector<float> amb_xyz;
                    while(std::getline(amb_ss,digit,' ')){
                        amb_xyz.push_back(std::stof(digit));
                    }
                    ambient = glm::vec3(amb_xyz[0], amb_xyz[1], amb_xyz[2]);
                }else if(header_index == headers::DIFFUSE){
                    std::stringstream diff_ss(segment);
                    std::vector<float> diff_xyz;
                    while(std::getline(diff_ss,digit,' ')){
                        diff_xyz.push_back(std::stof(digit));
                    }
                    diffuse = glm::vec3(diff_xyz[0], diff_xyz[1], diff_xyz[2]);
                }else if (header_index == headers::SPECULAR) {
                    std::stringstream spec_ss(segment);
                    std::vector<float> spec_xyz;
                    while (std::getline(spec_ss, digit, ' ')) {
                        spec_xyz.push_back(std::stof(digit));
                    }
                    specular = glm::vec3(spec_xyz[0], spec_xyz[1], spec_xyz[2]);
                }
                header_index++;
            }

            s->addDirectionalLight(light_name, direction, ambient, diffuse, specular);
        }else{
            first_line = false;
        }
	}
}

std::vector<unsigned int> GLFW_Util::createFrameBuffer(const unsigned int w, const unsigned int h){
    unsigned int FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    unsigned int TBO;
    glGenTextures(1, &TBO);
    glBindTexture(GL_TEXTURE_2D, TBO);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TBO, 0);

    unsigned int RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    std::vector<unsigned int> buffer_objects;
    buffer_objects.push_back(FBO);
    buffer_objects.push_back(TBO);
    buffer_objects.push_back(RBO);

    return buffer_objects;
}

std::vector<unsigned int> GLFW_Util::createRenderQuad(){
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

    std::vector<unsigned int> quad_objects;
    quad_objects.push_back(quadVAO);
    quad_objects.push_back(quadVBO); 

    return quad_objects;
}