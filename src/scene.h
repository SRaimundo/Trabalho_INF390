#ifndef SCENE_H__
#define SCENE_H__

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "texture.h"
#include "utils.h"
using namespace std;
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "object.h"

struct LightProperties{
    bool isCameraCoordinate;
    bool isEnabled;
    bool isLocal;
    bool isSpot;
    glm::vec3 ambient;
    glm::vec3 color;
    glm::vec3 position;
    glm::vec3 coneDirection;
    float spotCosCutoff;
    float spotExponent;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};

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
    void SetLight(int light_number, LightProperties my_light);
    void push_back_object(Object *new_object);
    void push_back_objects(vector<Object *> new_objects);

private:
    GLuint mShaderProgram;
    glm::mat4 mProjectionMatrix;
    glm::mat4 mView;
    vector<Object *> mObjects;
    LightProperties mLights[8];
    const int mMaxLights = 8;
};

Scene::~Scene(){
    for(auto it = mObjects.begin(); it != mObjects.end(); ++it){
        delete (*it);
    }
}

Scene::Scene(){
    const char *pVSFileName = "shaders/shader.vs";
    const char *pFSFileName = "shaders/shader.fs";
    mShaderProgram = CompileShaders(pVSFileName, pFSFileName);
    mProjectionMatrix = glm::mat4(1.0);
    mView = glm::mat4(1.0);

    for(int i = 1; i < mMaxLights; i++)
        mLights[i].isEnabled = false;

    mLights[0].isCameraCoordinate = true;
    mLights[0].isEnabled = true;
    mLights[0].isLocal = true;
    mLights[0].isSpot = false;

    mLights[0].ambient[0] = mLights[0].ambient[1] = mLights[0].ambient[2] = 0.3;
    mLights[0].color[0] = mLights[0].color[1] = mLights[0].color[2] = 1.0;
    mLights[0].position[0] = 0.0;
    mLights[0].position[1] = 5.0;
    mLights[0].position[2] = 0.0;

    mLights[0].constantAttenuation = 1.0;
    mLights[0].linearAttenuation = 0.0;
    mLights[0].quadraticAttenuation = 0.0;
}

void Scene::push_back_object(Object *new_object){
    mObjects.push_back(new_object);
}

void Scene::Render(){
    glUseProgram(mShaderProgram);
    GLint color_u = glGetUniformLocation(mShaderProgram, "Color");
    GLint mvp_u = glGetUniformLocation(mShaderProgram, "MVPMatrix");
    GLint mv_u = glGetUniformLocation(mShaderProgram, "MVMatrix");
    GLint normal_u = glGetUniformLocation(mShaderProgram, "NormalMatrix");
    GLint texture_combiner_u = glGetUniformLocation(mShaderProgram,"Texture_combiner");
    GLint gSampler = glGetUniformLocation(mShaderProgram, "gSampler");
    glUniform1i(gSampler, 0);

    GLint position = glGetAttribLocation(mShaderProgram, "Position");

    glEnableVertexAttribArray(position);

    GLint normal = glGetAttribLocation(mShaderProgram, "Normal");
    glEnableVertexAttribArray(normal);

    GLint texcoord = glGetAttribLocation(mShaderProgram,"TexCoord");
    glEnableVertexAttribArray(texcoord);

    // Light
    for(int i = 0; i < mMaxLights; i++){
        string light_name = "Lights[" + std::to_string(i) + "]";
        GLint isEnabled_u = glGetUniformLocation(mShaderProgram, (light_name + ".isEnabled").c_str());
        GLint isLocal_u = glGetUniformLocation(mShaderProgram, (light_name + ".isLocal").c_str());
        GLint isSpot_u = glGetUniformLocation(mShaderProgram, (light_name + ".isSpot").c_str());
        GLint ambient_u = glGetUniformLocation(mShaderProgram, (light_name + ".ambient").c_str());
        GLint lightcolor_u = glGetUniformLocation(mShaderProgram, (light_name + ".color").c_str());
        GLint lightposition_u = glGetUniformLocation(mShaderProgram, (light_name + ".position").c_str());

        GLint constantAttenuation_u = glGetUniformLocation(mShaderProgram, (light_name + ".constantAttenuation").c_str());
        GLint linearAttenuation_u = glGetUniformLocation(mShaderProgram, (light_name + ".linearAttenuation").c_str());
        GLint quadraticAttenuation_u = glGetUniformLocation(mShaderProgram, (light_name + ".quadraticAttenuation").c_str());

        glUniform1i(isEnabled_u, mLights[i].isEnabled);
        glUniform1i(isLocal_u, mLights[i].isLocal);
        glUniform1i(isSpot_u, mLights[i].isSpot);
        glUniform3fv(ambient_u, 1, glm::value_ptr(mLights[i].ambient));
        glUniform3fv(lightcolor_u, 1, glm::value_ptr(mLights[i].color));

        glUniform3fv(lightposition_u, 1, glm::value_ptr(mLights[i].position));

        glUniform1f(constantAttenuation_u, mLights[i].constantAttenuation);
        glUniform1f(linearAttenuation_u, mLights[i].linearAttenuation);
        glUniform1f(quadraticAttenuation_u, mLights[i].quadraticAttenuation);
    }

    // Material
    GLint shininess_u = glGetUniformLocation(mShaderProgram, "Shininess");
    GLint strength_u = glGetUniformLocation(mShaderProgram, "Strength");

    for(auto it = mObjects.begin(); it != mObjects.end(); ++it){

        glUniform1f(shininess_u, (**it).mShininess);
        glUniform1f(strength_u, (**it).mStrength);
        glUniform3fv(color_u, 1, (**it).mColor);

        glUniformMatrix4fv(mvp_u, 1, GL_FALSE, glm::value_ptr(mProjectionMatrix * mView * ((**it).mModelMatrix)));
        glUniformMatrix4fv(mv_u, 1, GL_FALSE, glm::value_ptr(mView * ((**it).mModelMatrix)));
        glUniformMatrix3fv(normal_u, 1, GL_FALSE, glm::value_ptr(glm::inverseTranspose(glm::mat3(mView * ((**it).mModelMatrix)))));
        
        glUniform1i(texture_combiner_u,0);
        (**it).mTextures->Bind(GL_TEXTURE0);
        
        (**it).Render(position,normal,texcoord);
    }
}

void Scene::SetColor(float r, float g, float b){
    for(auto it = mObjects.begin(); it != mObjects.end(); ++it){
        (**it).SetColor(r, g, b);
    }
    return;
}

void Scene::Ortho3D(float WL, float WR, float WB, float WT, float zNear, float zFar){

    mProjectionMatrix = glm::ortho(
        WL, WR,
        WB, WT,
        zNear, zFar);

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
    mView = glm::lookAt(glm::vec3(eyex, eyey, eyez),glm::vec3(centerx, centery, centerz),glm::vec3(upx, upy, upz));
}

void Scene::Perspective(float fovy, float aspect, float zNear, float zFar){
    mProjectionMatrix = glm::perspective(fovy, aspect, zNear, zFar);
}

void Scene::push_back_objects(vector<Object *> new_objects){
    for(auto it = new_objects.begin(); it != new_objects.end(); ++it){
        mObjects.push_back(*it);
    }
}

void Scene::SetLight(int light_number, LightProperties my_light){
    if(light_number >= mMaxLights || light_number < 0)
        return;
    mLights[light_number] = my_light;
    return;
}

#endif