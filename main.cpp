#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>             
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>  // For std::max


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// --------------------- Global Settings ---------------------
const unsigned int SCR_WIDTH  = 1000;
const unsigned int SCR_HEIGHT = 800;

// Camera settings
glm::vec3 cameraPos   = glm::vec3(0.0f, 1.0f, 8.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
float yaw   = -90.0f;
float pitch =  0.0f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT/ 2.0f;
bool  firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// --------------------- Global Variables for Object Transformations ---------------------
// 1 = cube, 2 = pyramid, 3 = sphere
int selectedObject = 1;

// Cube transformation variables
glm::vec3 cubePos(-2.0f, 0.5f, 0.0f);
float cubeRotY = 0.0f;   // Y-axis rotation (horizontal)
float cubeRotX = 0.0f;   // X-axis rotation (tilt up/down)
float cubeRotZ = 0.0f;
float cubeScale = 1.5f;

// Pyramid transformation variables
glm::vec3 pyramidPos(0.0f, 0.0f, 2.0f);
float pyramidRotY = 0.0f;
float pyramidRotX = 0.0f;
float pyramidRotZ = 0.0f;
float pyramidScale = 1.0f;
// New flag for pyramid auto-rotation
bool pyramidAutoRotate = false;

// Sphere transformation variables
glm::vec3 spherePos(2.0f, 0.5f, 0.0f);
float sphereRotY = 0.0f;
float sphereRotX = 0.0f;
float sphereRotZ = 0.0f;
float sphereScale = 1.0f;

// --------------------- Callback Functions ---------------------
// Resize viewport when window size changes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Mouse callback: update camera direction from mouse movement
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) { 
        lastX = (float)xpos; 
        lastY = (float)ypos; 
        firstMouse = false; 
    }
    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos; // reversed y
    lastX = (float)xpos;
    lastY = (float)ypos;
    
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    
    yaw   += xoffset;
    pitch += yoffset;
    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
    
    glm::vec3 front;
    front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    front.y = sin(glm::radians(pitch));
    front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    cameraFront = glm::normalize(front);
}

// Process keyboard input for camera and object transformation
void processInput(GLFWwindow* window) {
    // Camera movement (WASD)
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float camSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += camSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= camSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * camSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * camSpeed;

    // --- Object Selection: keys 1,2,3 ---
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        selectedObject = 1;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        selectedObject = 2;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        selectedObject = 3;

    // --- Object Translation Controls ---
    float objSpeed = 2.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        // Move forward (decrease Z)
        if (selectedObject == 1)
            cubePos.z -= objSpeed;
        else if (selectedObject == 2)
            pyramidPos.z -= objSpeed;
        else if (selectedObject == 3)
            spherePos.z -= objSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        // Move backward (increase Z)
        if (selectedObject == 1)
            cubePos.z += objSpeed;
        else if (selectedObject == 2)
            pyramidPos.z += objSpeed;
        else if (selectedObject == 3)
            spherePos.z += objSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        // Move left (decrease X)
        if (selectedObject == 1)
            cubePos.x -= objSpeed;
        else if (selectedObject == 2)
            pyramidPos.x -= objSpeed;
        else if (selectedObject == 3)
            spherePos.x -= objSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        // Move right (increase X)
        if (selectedObject == 1)
            cubePos.x += objSpeed;
        else if (selectedObject == 2)
            pyramidPos.x += objSpeed;
        else if (selectedObject == 3)
            spherePos.x += objSpeed;
    }
    
    // --- New: Object Translation Along Y Axis using Z and X keys ---
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        // Move up (increase Y)
        if (selectedObject == 1)
            cubePos.y += objSpeed;
        else if (selectedObject == 2)
            pyramidPos.y += objSpeed;
        else if (selectedObject == 3)
            spherePos.y += objSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        // Move down (decrease Y)
        if (selectedObject == 1)
            cubePos.y -= objSpeed;
        else if (selectedObject == 2)
            pyramidPos.y -= objSpeed;
        else if (selectedObject == 3)
            spherePos.y -= objSpeed;
    }
    
    // --- Object Rotation Controls ---
    float rotSpeed = glm::radians(90.0f) * deltaTime; // 90 degrees per second
    // Rotate around Y-axis (horizontal)
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        if (selectedObject == 1)
            cubeRotY += rotSpeed;
        else if (selectedObject == 2)
            pyramidRotY += rotSpeed;
        else if (selectedObject == 3)
            sphereRotY += rotSpeed;
    }

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        if (selectedObject == 1)
            cubeRotY -= rotSpeed;
        else if (selectedObject == 2)
            pyramidRotY -= rotSpeed;
        else if (selectedObject == 3)
            sphereRotY -= rotSpeed;
    }
    // Rotate around X-axis (tilt up/down)
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        if (selectedObject == 1)
            cubeRotX += rotSpeed;
        else if (selectedObject == 2)
            pyramidRotX += rotSpeed;
        else if (selectedObject == 3)
            sphereRotX += rotSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        if (selectedObject == 1)
            cubeRotX -= rotSpeed;
        else if (selectedObject == 2)
            pyramidRotX -= rotSpeed;
        else if (selectedObject == 3)
            sphereRotX -= rotSpeed;
    }
 
    // --- Object Scaling Controls ---
    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) { // '+' key
        if (selectedObject == 1)
            cubeScale += 0.5f * deltaTime;
        else if (selectedObject == 2)
            pyramidScale += 0.5f * deltaTime;
        else if (selectedObject == 3)
            sphereScale += 0.5f * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
        if (selectedObject == 1)
            cubeScale = std::max(0.1f, cubeScale - 0.5f * deltaTime);
        else if (selectedObject == 2)
            pyramidScale = std::max(0.1f, pyramidScale - 0.5f * deltaTime);
        else if (selectedObject == 3)
            sphereScale = std::max(0.1f, sphereScale - 0.5f * deltaTime);
    }
     
    // --- Toggle Pyramid Rotation Mode ---
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        pyramidAutoRotate = true;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        pyramidAutoRotate = false;
}
 
// --------------------- Utility Shader Functions ---------------------
unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION_ERROR:\n" << infoLog << std::endl;
    }
    return shader;
}
 
unsigned int createShaderProgram(const char* vertSrc, const char* fragSrc) {
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertSrc);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragSrc);
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cout << "ERROR::PROGRAM_LINKING_ERROR:\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}
 
// --------------------- Texture Loading Functions ---------------------
unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
 
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = GL_RGB;
        if (nrChannels == 1)       format = GL_RED;
        else if (nrChannels == 4)  format = GL_RGBA;
 
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, (GLint)format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
        stbi_image_free(data);
    } else {
        std::cout << "Failed to load texture: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}
 
unsigned int loadCubemap(std::vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
 
    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = GL_RGB;
            if (nrChannels == 4) format = GL_RGBA;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, (GLint)format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
 
    return textureID;
}
 
// --------------------- Geometry Data ---------------------
// Cube vertices: positions, normals, texcoords (36 vertices)
float cubeVertices[] = {
    // Back face
    -0.5f, -0.5f, -0.5f,   0, 0, -1,   0, 0,
     0.5f, -0.5f, -0.5f,   0, 0, -1,   1, 0,
     0.5f,  0.5f, -0.5f,   0, 0, -1,   1, 1,
     0.5f,  0.5f, -0.5f,   0, 0, -1,   1, 1,
    -0.5f,  0.5f, -0.5f,   0, 0, -1,   0, 1,
    -0.5f, -0.5f, -0.5f,   0, 0, -1,   0, 0,
    // Front face
    -0.5f, -0.5f,  0.5f,   0, 0,  1,   0, 0,
     0.5f, -0.5f,  0.5f,   0, 0,  1,   1, 0,
     0.5f,  0.5f,  0.5f,   0, 0,  1,   1, 1,
     0.5f,  0.5f,  0.5f,   0, 0,  1,   1, 1,
    -0.5f,  0.5f,  0.5f,   0, 0,  1,   0, 1,
    -0.5f, -0.5f,  0.5f,   0, 0,  1,   0, 0,
    // Left face
    -0.5f,  0.5f,  0.5f,  -1, 0,  0,   1, 0,
    -0.5f,  0.5f, -0.5f,  -1, 0,  0,   1, 1,
    -0.5f, -0.5f, -0.5f,  -1, 0,  0,   0, 1,
    -0.5f, -0.5f, -0.5f,  -1, 0,  0,   0, 1,
    -0.5f, -0.5f,  0.5f,  -1, 0,  0,   0, 0,
    -0.5f,  0.5f,  0.5f,  -1, 0,  0,   1, 0,
    // Right face
     0.5f,  0.5f,  0.5f,   1, 0,  0,   1, 0,
     0.5f,  0.5f, -0.5f,   1, 0,  0,   1, 1,
     0.5f, -0.5f, -0.5f,   1, 0,  0,   0, 1,
     0.5f, -0.5f, -0.5f,   1, 0,  0,   0, 1,
     0.5f, -0.5f,  0.5f,   1, 0,  0,   0, 0,
     0.5f,  0.5f,  0.5f,   1, 0,  0,   1, 0,
    // Bottom face
    -0.5f, -0.5f, -0.5f,   0, -1,  0,   0, 1,
     0.5f, -0.5f, -0.5f,   0, -1,  0,   1, 1,
     0.5f, -0.5f,  0.5f,   0, -1,  0,   1, 0,
     0.5f, -0.5f,  0.5f,   0, -1,  0,   1, 0,
    -0.5f, -0.5f,  0.5f,   0, -1,  0,   0, 0,
    -0.5f, -0.5f, -0.5f,   0, -1,  0,   0, 1,
    // Top face
    -0.5f,  0.5f, -0.5f,   0, 1,  0,    0, 1,
     0.5f,  0.5f, -0.5f,   0, 1,  0,    1, 1,
     0.5f,  0.5f,  0.5f,   0, 1,  0,    1, 0,
     0.5f,  0.5f,  0.5f,   0, 1,  0,    1, 0,
    -0.5f,  0.5f,  0.5f,   0, 1,  0,    0, 0,
    -0.5f,  0.5f, -0.5f,   0, 1,  0,    0, 1
};
 
// Ground: a large quad (positions, normals, texcoords)
float groundVertices[] = {
    -50.0f, 0.0f, -50.0f,   0, 1, 0,  0, 0,
     50.0f, 0.0f, -50.0f,   0, 1, 0,  50, 0,
     50.0f, 0.0f,  50.0f,   0, 1, 0,  50, 50,
     
     50.0f, 0.0f,  50.0f,   0, 1, 0,  50, 50,
    -50.0f, 0.0f,  50.0f,   0, 1, 0,  0, 50,
    -50.0f, 0.0f, -50.0f,   0, 1, 0,  0, 0
};
 
// Skybox: a cube (positions only)
float skyboxVertices[] = {
    -1.0f,  1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,   1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,   1.0f,  1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,  -1.0f, -1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,   1.0f, -1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,   1.0f,  1.0f, -1.0f,   1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,   1.0f, -1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,   1.0f,  1.0f, -1.0f,   1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,   1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,   1.0f, -1.0f, -1.0f,  -1.0f, -1.0f, -1.0f
};
 
// Pyramid: square base (2 triangles) and 4 side triangles (18 vertices total)
// Format: position (3), normal (3), texcoord (2)
float pyramidVertices[] = {
    // Base
    -0.5f, 0.0f, -0.5f,   0, -1, 0,   0, 0,
     0.5f, 0.0f, -0.5f,   0, -1, 0,   1, 0,
     0.5f, 0.0f,  0.5f,   0, -1, 0,   1, 1,
     
     0.5f, 0.0f,  0.5f,   0, -1, 0,   1, 1,
    -0.5f, 0.0f,  0.5f,   0, -1, 0,   0, 1,
    -0.5f, 0.0f, -0.5f,   0, -1, 0,   0, 0,
     
    // Side 1 (back)
    -0.5f, 0.0f, -0.5f,   0, 0.4472f, -0.8944f,   0, 0,
     0.0f, 1.0f,  0.0f,   0, 0.4472f, -0.8944f,   0.5f, 1,
     0.5f, 0.0f, -0.5f,   0, 0.4472f, -0.8944f,   1, 0,
     
    // Side 2 (right)
     0.5f, 0.0f, -0.5f,   0.8944f, 0.4472f, 0,   0, 0,
     0.0f, 1.0f,  0.0f,   0.8944f, 0.4472f, 0,   0.5f, 1,
     0.5f, 0.0f,  0.5f,   0.8944f, 0.4472f, 0,   1, 0,
     
    // Side 3 (front)
     0.5f, 0.0f,  0.5f,   0, 0.4472f, 0.8944f,   0, 0,
     0.0f, 1.0f,  0.0f,   0, 0.4472f, 0.8944f,   0.5f, 1,
    -0.5f, 0.0f,  0.5f,   0, 0.4472f, 0.8944f,   1, 0,
     
    // Side 4 (left)
    -0.5f, 0.0f,  0.5f,  -0.8944f, 0.4472f, 0,   0, 0,
     0.0f, 1.0f,  0.0f,  -0.8944f, 0.4472f, 0,   0.5f, 1,
    -0.5f, 0.0f, -0.5f,  -0.8944f, 0.4472f, 0,   1, 0
};
 
// --------------------- Sphere Generation ---------------------
void generateSphere(std::vector<float>& vertices, std::vector<unsigned int>& indices, float radius, unsigned int sectorCount, unsigned int stackCount) {
    const float PI = 3.14159265359f;
    for (unsigned int i = 0; i <= stackCount; ++i) {
        float stackAngle = PI / 2 - i * (PI / stackCount);
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);
        for (unsigned int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = j * (2 * PI / sectorCount);
            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            glm::vec3 norm = glm::normalize(glm::vec3(x, y, z));
            vertices.push_back(norm.x);
            vertices.push_back(norm.y);
            vertices.push_back(norm.z);
            float s = (float)j / sectorCount;
            float t = (float)i / stackCount;
            vertices.push_back(s);
            vertices.push_back(t);
        }
    }
    for (unsigned int i = 0; i < stackCount; ++i) {
        unsigned int k1 = i * (sectorCount + 1);
        unsigned int k2 = k1 + sectorCount + 1;
        for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            if (i != (stackCount - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
}
 
// --------------------- Shader Sources ---------------------
// Object Shader
const char* objVertexShaderSrc = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
 
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
 
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
 
void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";
   
const char* objFragmentShaderSrc = R"(
#version 330 core
out vec4 FragColor;
 
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
 
uniform sampler2D texture1;
uniform bool useTexture;
uniform vec3 objectColor;
 
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 viewPos;
 
void main() {
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;
    vec3 norm = normalize(Normal);
    vec3 invLight = normalize(-lightDir);
    float diff = max(dot(norm, invLight), 0.0);
    vec3 diffuse = diff * lightColor;
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
     
    vec3 texColor = useTexture ? texture(texture1, TexCoord).rgb : objectColor;
    vec3 result = (ambient + diffuse + specular) * texColor;
    FragColor = vec4(result, 1.0);
}
)";
   
// Skybox Shader
const char* skyboxVertexShaderSrc = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
out vec3 TexCoords;
uniform mat4 view;
uniform mat4 projection;
void main() {
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
)";
   
const char* skyboxFragmentShaderSrc = R"(
#version 330 core
out vec4 FragColor;
in vec3 TexCoords;
uniform samplerCube skybox;
void main() {
    FragColor = texture(skybox, TexCoords);
}
)";
   
// --------------------- Main Function ---------------------
int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
 
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Interactive Scene", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
 
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
 
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
 
    // Build shader programs
    unsigned int objShader = createShaderProgram(objVertexShaderSrc, objFragmentShaderSrc);
    unsigned int skyboxShader = createShaderProgram(skyboxVertexShaderSrc, skyboxFragmentShaderSrc);
 
    // --------------------- Setup Geometry ---------------------
    // Cube
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
      glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
      glEnableVertexAttribArray(2);
    glBindVertexArray(0);
 
    // Ground
    unsigned int groundVAO, groundVBO;
    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);
    glBindVertexArray(groundVAO);
      glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
      glEnableVertexAttribArray(2);
    glBindVertexArray(0);
 
    // Pyramid
    unsigned int pyramidVAO, pyramidVBO;
    glGenVertexArrays(1, &pyramidVAO);
    glGenBuffers(1, &pyramidVBO);
    glBindVertexArray(pyramidVAO);
      glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
      glEnableVertexAttribArray(2);
    glBindVertexArray(0);
 
    // Sphere (generated)
    std::vector<float> sphereVerts;
    std::vector<unsigned int> sphereIndices;
    generateSphere(sphereVerts, sphereIndices, 0.5f, 32, 16);
    unsigned int sphereVAO, sphereVBO, sphereEBO;
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);
    glBindVertexArray(sphereVAO);
      glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
      glBufferData(GL_ARRAY_BUFFER, sphereVerts.size() * sizeof(float), &sphereVerts[0], GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), &sphereIndices[0], GL_STATIC_DRAW);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
      glEnableVertexAttribArray(2);
    glBindVertexArray(0);
 
    // Skybox
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
      glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(0);
    glBindVertexArray(0);
 
    // --------------------- Load Textures ---------------------
    unsigned int cubeTexture   = loadTexture("textures/texture.jpg");
    unsigned int groundTexture = loadTexture("textures/stone-texture.jpg");
    std::vector<std::string> faces {
        "skybox/posx.jpg",
        "skybox/negx.jpg",
        "skybox/posy.jpg",
        "skybox/negy.jpg",
        "skybox/posz.jpg",
        "skybox/negz.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(faces);
 
    // --------------------- Render Loop ---------------------
    while (!glfwWindowShouldClose(window)) {
        float currentTime = (float)glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;
 
        processInput(window);
 
        glClearColor(0.1f, 0.12f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
        // Setup view and projection matrices
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);
 
        // Use object shader for ground, cube, pyramid, sphere
        glUseProgram(objShader);
        glUniformMatrix4fv(glGetUniformLocation(objShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(objShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(glGetUniformLocation(objShader, "lightDir"), 1, glm::value_ptr(glm::vec3(-0.2f, -1.0f, -0.3f)));
        glUniform3fv(glGetUniformLocation(objShader, "lightColor"), 1, glm::value_ptr(glm::vec3(1.0f)));
        glUniform3fv(glGetUniformLocation(objShader, "viewPos"), 1, glm::value_ptr(cameraPos));
 
        // --- Draw Ground ---
        {
            glm::mat4 model = glm::mat4(1.0f);
            glUniformMatrix4fv(glGetUniformLocation(objShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(glGetUniformLocation(objShader, "useTexture"), true);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, groundTexture);
            glUniform1i(glGetUniformLocation(objShader, "texture1"), 0);
            glBindVertexArray(groundVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
 
        // --- Draw Cube ---
        {
            glm::mat4 model = glm::mat4(1.0f);
            // Apply translation, then rotation about Y, then rotation about X, then scale.
            model = glm::translate(model, cubePos);
            model = glm::rotate(model, cubeRotY, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, cubeRotX, glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(cubeScale));
            glUniformMatrix4fv(glGetUniformLocation(objShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(glGetUniformLocation(objShader, "useTexture"), true);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, cubeTexture);
            glUniform1i(glGetUniformLocation(objShader, "texture1"), 0);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
        }
 
        // --- Draw Pyramid (sky blue color with auto/manual rotation) ---
        {
            // Auto-rotate if enabled
            if (pyramidAutoRotate) {
                pyramidRotX += glm::radians(30.0f) * deltaTime;
            }
 
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pyramidPos);
            model = glm::rotate(model, pyramidRotY, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, pyramidRotX, glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, pyramidRotZ, glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(pyramidScale));
            glUniformMatrix4fv(glGetUniformLocation(objShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(glGetUniformLocation(objShader, "useTexture"), false);
            // Set sky blue color
            glUniform3f(glGetUniformLocation(objShader, "objectColor"), 0.53f, 0.81f, 0.92f);
            glBindVertexArray(pyramidVAO);
            glDrawArrays(GL_TRIANGLES, 0, 18);
            glBindVertexArray(0);
        }
 
        // --- Draw Sphere (solid color) ---
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, spherePos);
            model = glm::rotate(model, sphereRotY, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, sphereRotX, glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(sphereScale));
            glUniformMatrix4fv(glGetUniformLocation(objShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(glGetUniformLocation(objShader, "useTexture"), false);
            glUniform3f(glGetUniformLocation(objShader, "objectColor"), 0.8f, 0.4f, 0.2f);
            glBindVertexArray(sphereVAO);
            glDrawElements(GL_TRIANGLES, (GLsizei)sphereIndices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
 
        // --- Draw Skybox ---
        glDepthFunc(GL_LEQUAL);
        glUseProgram(skyboxShader);
        glm::mat4 skyboxView = glm::mat4(glm::mat3(view));
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "view"), 1, GL_FALSE, glm::value_ptr(skyboxView));
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glUniform1i(glGetUniformLocation(skyboxShader, "skybox"), 0);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);
 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    // --------------------- Cleanup ---------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &groundVAO);
    glDeleteBuffers(1, &groundVBO);
    glDeleteVertexArrays(1, &pyramidVAO);
    glDeleteBuffers(1, &pyramidVBO);
    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteBuffers(1, &sphereVBO);
    glDeleteBuffers(1, &sphereEBO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteProgram(objShader);
    glDeleteProgram(skyboxShader);
 
    glfwTerminate();
    return 0;
}
