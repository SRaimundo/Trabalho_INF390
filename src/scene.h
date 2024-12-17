#ifndef SCENE_H__
#define SCENE_H__

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "utils.h"
using namespace std;
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "object.h"

class Scene{
public:
    Scene();
    ~Scene();
    void Render();
    void SetColor(float r, float g, float b);
    void Ortho3D(float WL, float WR, float WB, float WT, float zNear, float zFar);
    void Perspective(float fovy, float aspect, float zNear, float zFar);
    void LookAt(float eyex, float eyey, float eyez,
                float centerx, float centery, float centerz,
                float upx, float upy, float upz);
    void Model(glm::mat4 model_matrix);
    void push_back_object(Object *new_object);
    void push_back_objects(vector<Object *> new_objects);
    void SetWireframe(bool on_wireframe);

private:
    GLuint mShaderProgram;
    glm::mat4 mProjectionMatrix;
    glm::mat4 mView;
    vector<Object *> mObjects;
};

Scene::~Scene(){
    for (auto it = mObjects.begin(); it != mObjects.end(); ++it){
        delete (*it);
    }
}

Scene::Scene(){
    const char *pVSFileName = "../src/shader.vs";
    const char *pFSFileName = "../src/shader.fs";
    mShaderProgram = CompileShaders(pVSFileName, pFSFileName);
    mProjectionMatrix = glm::mat4(1.0);
    mView = glm::mat4(1.0);
}

void Scene::push_back_object(Object *new_object){
    mObjects.push_back(new_object);
}

void Scene::Render(){
    glUseProgram(mShaderProgram);
    GLint color_u = glGetUniformLocation(mShaderProgram, "Color");
    GLint projection_u = glGetUniformLocation(mShaderProgram, "Projection");
    GLint model_u = glGetUniformLocation(mShaderProgram, "Model");
    GLint view_u = glGetUniformLocation(mShaderProgram, "View");
    GLint position = glGetAttribLocation(mShaderProgram, "Position");
    glEnableVertexAttribArray(position);

    for(auto it = mObjects.begin(); it != mObjects.end(); ++it){
        glUniform3fv(color_u, 1, (**it).mColor);
        glUniformMatrix4fv(projection_u, 1, GL_FALSE, glm::value_ptr(mProjectionMatrix));
        glUniformMatrix4fv(view_u, 1, GL_FALSE, glm::value_ptr(mView));
        glUniformMatrix4fv(model_u, 1, GL_FALSE, glm::value_ptr(((**it).mModelMatrix)));
        (**it).Render(position);
    }
}

void Scene::SetColor(float r, float g, float b){
    for (auto it = mObjects.begin(); it != mObjects.end(); ++it){
        (**it).SetColor(r, g, b);
    }
    return;
}

void Scene::SetWireframe(bool on_wireframe){
    for (auto it = mObjects.begin(); it != mObjects.end(); ++it){
        (**it).SetWireframe(true);
    }
    return;
}

void Scene::Ortho3D(float WL, float WR, float WB, float WT, float zNear, float zFar){
    mProjectionMatrix = glm::ortho(WL, WR,  WB, WT,  zNear, zFar);
    return;
}

void Scene::Model(glm::mat4 model_matrix){
    for(auto it = mObjects.begin(); it != mObjects.end(); ++it){
        (**it).push_left_matrix(model_matrix);
    }
    return;
}

void Scene::LookAt(float eyex, float eyey, float eyez,
                   float centerx, float centery, float centerz,
                   float upx, float upy, float upz){

    mView = glm::lookAt(
        glm::vec3(eyex, eyey, eyez),
        glm::vec3(centerx, centery, centerz),
        glm::vec3(upx, upy, upz));
}

void Scene::Perspective(float fovy, float aspect, float zNear, float zFar){
    mProjectionMatrix = glm::perspective(fovy, aspect, zNear, zFar);
}

void Scene::push_back_objects(vector<Object *> new_objects){
    for(auto it = new_objects.begin(); it != new_objects.end(); ++it){
        mObjects.push_back(*it);
    }
}
#endif