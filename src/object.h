#ifndef OBJECT_H__
#define OBJECT_H__

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include "texture.h"
#include "utils.h"
using namespace std;
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Scene;
class Object{
public:
    friend class Scene;
    Object(int verticesNumber,int indexNumber, GLfloat *Vertices_information,GLushort *index);
    ~Object();
    void SetColor(float r, float g, float b);
    void SetShininess(float shininess) { mShininess = shininess; };
    void SetStrength(float strength) { mStrength = strength; };
    void Model(glm::mat4 model_matrix);
    void push_right_matrix(glm::mat4 matrix);
    void push_left_matrix(glm::mat4 matrix);
    glm::mat4 GetModelMatrix();
    void SetWireframe(bool on_wireframe) { mIsWireframe = on_wireframe; };
    void LoadTexture2DSimpleBmp(const char *name,int header_size,int Width,int Height,int BGR=0);
    friend class Scene;

private:
    void Render(GLint position,GLint normal,GLint texcoord);
    bool mIsIndexed;
    bool mIsWireframe;
    bool no_normal;
    float mColor[3];
    float mShininess;
    float mStrength;

    glm::mat4 mModelMatrix;
    GLuint mVBO;
    GLuint mEBO;
    int mVerticesNumber;
    int mIndexNumber;

    bool mIsNoTexture;
    Texture *mTextures;
};

Object::Object(int verticesNumber,int indexNumber, GLfloat *Vertices_information,GLushort *index):mVerticesNumber(verticesNumber),mIndexNumber(indexNumber){
    mIsIndexed = true;
    mIsWireframe = false;
    no_normal = false;
    mIsNoTexture = false;
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*mVerticesNumber*8,
    Vertices_information, GL_STATIC_DRAW);
    glGenBuffers(1, &mEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*mIndexNumber,
    index, GL_STATIC_DRAW);
    mColor[0] = 1.0;
    mColor[1] = mColor[2] = 0.0;
    mShininess = 50.0;
    mStrength = 1.0;
    mModelMatrix = glm::mat4(1.0);
}

Object::~Object(){
    glDeleteBuffers(1, &mVBO);
    if (mIsIndexed)
        glDeleteBuffers(1, &mEBO);
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

void Object::LoadTexture2DSimpleBmp(const char *name,int header_size,int Width,int Height,int BGR){
    if(mIsNoTexture){
        cout << "Object without uv " << endl;
        exit(1);
    }
    mTextures = new Texture(GL_TEXTURE_2D,name);
    mTextures->LoadSimpleBmp(header_size,Width,Height,BGR);
}

void Object::Render(GLint position,GLint normal,GLint texcoord){
    if (no_normal or mIsNoTexture){
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 0, 0);

        if(!mIsWireframe){
            if(!(mIsIndexed)){
                glDrawArrays(GL_TRIANGLES, 0, mVerticesNumber);
            }else{
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
                glDrawElements(GL_TRIANGLES, mIndexNumber, GL_UNSIGNED_SHORT, NULL);
            }
        }else{
            glPolygonMode(GL_FRONT, GL_LINE);
            glPolygonMode(GL_BACK, GL_LINE);
            if(!(mIsIndexed)){
                glDrawArrays(GL_TRIANGLES, 0, mVerticesNumber);
            }else{
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
                glDrawElements(GL_TRIANGLES, mIndexNumber, GL_UNSIGNED_SHORT, NULL);
            }
            glPolygonMode(GL_FRONT, GL_FILL);
            glPolygonMode(GL_BACK, GL_FILL);
        }
    }else{
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
        glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)
        (3* sizeof(float)));
        glVertexAttribPointer(texcoord, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
        (void*)(6* sizeof(float)));

        if(!mIsWireframe){
            if(!(mIsIndexed)){
                glDrawArrays(GL_TRIANGLES, 0, mVerticesNumber);
            }else{
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
                glDrawElements(GL_TRIANGLES, mIndexNumber, GL_UNSIGNED_SHORT, NULL);
            }
        }else{
            glPolygonMode(GL_FRONT, GL_LINE);
            glPolygonMode(GL_BACK, GL_LINE);
            if(!(mIsIndexed)){
                glDrawArrays(GL_TRIANGLES, 0, mVerticesNumber);
            }else{
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
                glDrawElements(GL_TRIANGLES, mIndexNumber, GL_UNSIGNED_SHORT, NULL);
            }
            glPolygonMode(GL_FRONT, GL_FILL);
            glPolygonMode(GL_BACK, GL_FILL);
        }
    }

    
}

#endif