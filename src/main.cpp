#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <fstream>

#include "scene.h"
#include "object.h"
#include "loadobject.h"
#include "airplane.h"

using namespace std;
 
bool ortho_per = true;
GLint SCREEN_WIDTH = 800,SCREEN_HEIGHT = 800; 

int worldx=-5, worldy=175, worldz=0;
// glm::vec3 camera(40.0f,0.0f,40.0f); // side view
// glm::vec3 initCamera(40.0f,0.0f,40.0f); // side view
glm::vec3 camera(-30.0f, 5.0f, 0.0f); // rear view
glm::vec3 initCamera(-30.0f, 5.0f, 0.0f); // rear view

float camerax=0.0,cameray=0.0,cameraz=5.0;

bool dynamicCamera = true; 

AirplaneInput airplaneInput;

static void error_callback(int error, const char* description){
    fprintf(stderr, "Error: %s\n", description);
}
 
static bool evalInput(bool input, int action) {
    return action == GLFW_PRESS ? true : action == GLFW_RELEASE ? false : input;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if ( key == GLFW_KEY_P && action == GLFW_PRESS ) 
        ortho_per = true;
    if ( key == GLFW_KEY_O && action == GLFW_PRESS ) 
       ortho_per = false;
    if(action == GLFW_PRESS){
        switch (key) {
        case GLFW_KEY_Q:
            if (mods == GLFW_MOD_SHIFT) {
                worldx = (worldx + 5) % 360;
            }
            else {
                worldx = (worldx - 5) % 360;
            }
            break;
        case GLFW_KEY_W:
            if (mods == GLFW_MOD_SHIFT) {
                worldy = (worldy + 5) % 360;
            }
            else {
                worldy = (worldy - 5) % 360;
            }
            break;
        case GLFW_KEY_E:
            if (mods == GLFW_MOD_SHIFT) {
                worldz = (worldz + 5) % 360;
            }
            else {
                worldz = (worldz - 5) % 360;
            }
            break;
        case GLFW_KEY_A:
            if (mods == GLFW_MOD_SHIFT) {
                cameraz = cameraz + 0.1;
            }
            else {
                cameraz = cameraz - 0.1;
            }
            break;
        default:
            break;
        }
    }
    
    // identify holding the key down, GLFW_PRESS vs GLFW_RELEASE (both generate callbacks)
    switch (key) {
        printf("key\n");
        case GLFW_KEY_LEFT:
            airplaneInput.mLeft = evalInput(airplaneInput.mLeft, action);
            break;
        case GLFW_KEY_RIGHT:
            airplaneInput.mRight = evalInput(airplaneInput.mRight, action);
            break;
        case GLFW_KEY_UP:
            airplaneInput.mUp = evalInput(airplaneInput.mUp, action);
            break;
        case GLFW_KEY_DOWN:
            airplaneInput.mDown = evalInput(airplaneInput.mDown, action);
            break;
        case GLFW_KEY_Z:
            airplaneInput.mTakeOff = evalInput(airplaneInput.mTakeOff, action);
            break;
        case GLFW_KEY_X:
            airplaneInput.mLand = evalInput(airplaneInput.mLand, action);
            break;
        default:
            break;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
    }
}

int main(void){
    GLFWwindow* window; 
    glfwSetErrorCallback(error_callback);
    if(!glfwInit())
        exit(EXIT_FAILURE);
 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
 
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Simple example", NULL, NULL);
    if(!window){
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
 
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    
    glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
  
    glEnable(GL_DEPTH_TEST);
    Scene my_scene;

    Airplane airplane;

    Object* plane = read_obj_file("models/plane.obj");
    plane->LoadTexture2DSimpleBmp("models/uvmap.bmp", 0, 1000, 1000);

    my_scene.PushBackObjects(airplane.GetObjects());
    my_scene.PushBackObject(plane);

    // my_scene.SetWireframe(true);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    while (!glfwWindowShouldClose(window)){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (ortho_per) {
            my_scene.Perspective(0.75, 1.0, 0.1, 200.0);
        }else{
            my_scene.Ortho3D(-2.0, 2.0, -2.0, 2.0, 0.0, 200.0);
        }

        //printf("%d %d %d\n", worldx, worldy, worldz);

        airplane.Input(airplaneInput);
        airplane.Update();

        if (!dynamicCamera) {
            my_scene.LookAt(worldx, worldy, worldz, 0, 10, 0, 0.0, 10.0, 0.0);
        }
        else {
            glm::vec3 planePosition = glm::vec3(airplane.GetModelMatrix()*glm::vec4(0.0f,0.0f,0.0f,1.0f));

            glm::vec3 nextCameraView = glm::vec3(airplane.GetModelMatrix() * glm::vec4(initCamera,1.0f));

            nextCameraView = camera*0.9f + nextCameraView*0.1f;
            camera = nextCameraView;

            my_scene.LookAt(camera[0], camera[1], camera[2], planePosition[0], planePosition[1], planePosition[2], 0.0f, 1.0f, 0.0f);
        }


        my_scene.Render();


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}