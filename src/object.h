#ifndef OBJECT_H__
#define OBJECT_H__

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include "utils.h"
using namespace std;
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Scene;
class Object{
public:
    friend class Scene;
    Object();
    Object(int v_number, GLfloat *Vertices);
    Object(int v_number, int i_number, GLfloat *Vertices, GLushort *index);
    ~Object();
    void SetColor(float r, float g, float b);
    void Model(glm::mat4 model_matrix);
    void push_right_matrix(glm::mat4 matrix);
    void push_left_matrix(glm::mat4 matrix);
    glm::mat4 GetModelMatrix();
    void SetWireframe(bool on_wireframe) { mIsWireframe = on_wireframe; };
    friend class Scene;

private:
    void Render(GLint position);
    bool mIsIndexed;
    bool mIsWireframe;
    float mColor[3];
    glm::mat4 mModelMatrix;
    GLuint mVBO;
    GLuint mEBO;
    int mVerticesNumber;
    int mIndexNumber;
};

Object::~Object(){
    glDeleteBuffers(1, &mVBO);
    if (mIsIndexed)
        glDeleteBuffers(1, &mEBO);
}

Object::Object(){
    mIsIndexed = true;
    mIsWireframe = false;
    CreateVertexBufferTriangle(mVBO, mEBO, mVerticesNumber, mIndexNumber);
    mColor[0] = 1.0;
    mColor[1] = mColor[2] = 0.0;
    mModelMatrix = glm::mat4(1.0);
};

Object::Object(int v_number, GLfloat *Vertices) : mVerticesNumber(v_number){
    mIsIndexed = false;
    mIsWireframe = false;
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * mVerticesNumber * 3, Vertices, GL_STATIC_DRAW);
    mColor[0] = 1.0;
    mColor[1] = mColor[2] = 0.0;
    mModelMatrix = glm::mat4(1.0);
};

Object::Object(int v_number, int i_number, GLfloat *Vertices, GLushort *index) : mVerticesNumber(v_number), mIndexNumber(i_number){
    mIsIndexed = true;
    mIsWireframe = false;
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * v_number * 3, Vertices, GL_STATIC_DRAW);
    glGenBuffers(1, &mEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * mIndexNumber, index, GL_STATIC_DRAW);
    mColor[0] = 1.0;
    mColor[1] = mColor[2] = 0.0;
    mModelMatrix = glm::mat4(1.0);
}

void Object::SetColor(float r, float g, float b){
    mColor[0] = r;
    mColor[1] = g;
    mColor[2] = b;
    return;
}

void Object::Model(glm::mat4 model_matrix){
    mModelMatrix = model_matrix;
    return;
}

void Object::push_right_matrix(glm::mat4 matrix){
    mModelMatrix = mModelMatrix * matrix;
}

void Object::push_left_matrix(glm::mat4 matrix){
    mModelMatrix = matrix * mModelMatrix;
}

glm::mat4 Object::GetModelMatrix(){
    return mModelMatrix;
}

void Object::Render(GLint position){
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 0, 0);
    if (!mIsWireframe){
        if (!(mIsIndexed)){
            glDrawArrays(GL_TRIANGLES, 0, mVerticesNumber);
        }else{
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
            glDrawElements(GL_TRIANGLES, mIndexNumber, GL_UNSIGNED_SHORT, NULL);
        }
    }else{
        glPolygonMode(GL_FRONT, GL_LINE);
        glPolygonMode(GL_BACK, GL_LINE);
        if (!(mIsIndexed)){
            glDrawArrays(GL_TRIANGLES, 0, mVerticesNumber);
        }else{
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
            glDrawElements(GL_TRIANGLES, mIndexNumber, GL_UNSIGNED_SHORT, NULL);
        }
        glPolygonMode(GL_FRONT, GL_FILL);
        glPolygonMode(GL_BACK, GL_FILL);
    }
}
#endif