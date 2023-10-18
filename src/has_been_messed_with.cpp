#include <iostream>

#include <GL/freeglut.h>

#include <GLES2/gl2.h>
#include <EGL/egl.h>

// https://github.com/g-truc/glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/component_wise.hpp>

#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>

#include "shader.h"
#include "mesh.h"
#include "camera.h"


bool keys[256];

int width = 1600, height = 900;
float lastTime;
Camera *camera = nullptr;

void mouse(int button, int state, int x, int y ){}


int main() {
    plog::init(plog::warning);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // create window
    GLFWwindow *window = glfwCreateWindow(width, height, "Mesh Viewer", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // init glad
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glutMouseWheelFunc(scroll_callback);

    ShaderProgram program("../src/shader/common.vert", "../src/shader/phong.frag");
    Model lumine("../resource/lumine/Lumine.obj", &program);
    camera = new ModelRotationCamera({0.0f, 10.0f, 0.0f}, 20.0f);
    lastTime = glfwGetTime();

    glm::vec3 lightPos(0.0f, 10.0f, 5.0f),
            lightColor(3.0f, 3.0f, 1.0f),
            lightAmbient(1.0f, 1.0f, 1.0f);

    glViewport(0, 0, width, height);
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float nowTime = glfwGetTime();
        float deltaTime = nowTime - lastTime;
        lastTime = nowTime;
        handle_keyboard(window, deltaTime);

        glm::mat4 model = glm::identity<glm::mat4>();
        glm::mat4 view = camera->getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera->getFOV()),
                                                (float) width / height, 0.1f, 100.0f);

        program.setMVPMatrices(model, view, projection);
        program.setVec3("eyePos", camera->getPosition());
        program.setVec3("light.position", lightPos);
        program.setVec3("light.ambient", lightAmbient);
        program.setVec3("light.diffuse", lightColor);
        program.setVec3("light.specular", lightColor);
        lumine.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete camera;
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    ::width = width;
    ::height = height;
    glViewport(0, 0, width, height);
}

//void cursor_position_callback(GLFWwindow *window, double x, double y) {
//    bool pressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
//    if (camera != nullptr) camera->handleMouseInput(x, y, pressed);
//}


void mouse(int button, int state, int x, int y ){
  if (state == GLUT_DOWN) {
    switch(button) {
    case GLUT_LEFT_BUTTON:

        if (camera != nullptr) camera->handleMouseInput((double) x, (double) y, true);
        break;
    case 3:  //mouse wheel scrolls
        if (camera != nullptr) camera->handleScrollInput( (double)(-y));
        break;
    case 4:
        if (camera != nullptr) camera->handleScrollInput((double) y);       
        break;
    default:
        break;
    }
    
    }
    else{
    if (camera != nullptr) camera->handleMouseInput(x, y, false);
    }   
    
}

void KeyboardDown( int key, int x, int y ) {
    if ( isalpha( key ) ) {
        key = toupper( key );
    }
    keys[ key ] = true;
}
void KeyboardUp( int key, int x, int y ) {
    if ( isalpha( key ) ) {
        key = toupper( key );
    }
    keys[ key ] = false;        
}
void handle_keyboard(float deltaTime) {
    if ( keys['W'] ) {
        if (camera != nullptr) camera->handleKeyboardInput('W', deltaTime);
    } 
    if ( keys['A'] ) {
        if (camera != nullptr) camera->handleKeyboardInput('A', deltaTime);
    }
    if ( keys['S'] ) {
        if (camera != nullptr) camera->handleKeyboardInput('S', deltaTime);
    }
    if ( keys['D'] ) {
        if (camera != nullptr) camera->handleKeyboardInput('D', deltaTime);
    }    
}
