#ifndef OBJECT_H__
#define OBJECT_H__

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include "texture.h"
#include "utils.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

using namespace std;
using namespace glm; 

class Scene;
class Object{
public:
    friend class Scene;
    
    Object(int verticesNumber,int indexNumber, GLfloat *Vertices_information,GLushort *index);
    ~Object();
    
    void SetColor(float r, float g, float b);
    void SetShininess(float shininess) { mShininess = shininess; };
    void SetStrength(float strength) { mStrength = strength; };
    
    void SetPosition(vec3 position);
    void SetEulerAngles(vec3 eulerAngles);
    void SetScale(vec3 scale);
    
    vec3 GetPosition() { return mPosition; }
    vec3 GetEulerAngles() { return mEulerAngles; };

    vec3 GetGlobalPosition() { return mPosition; }
    vec3 GetGlobalEulerAngles() { return mEulerAngles; };


    mat4 GetModelMatrix();
    
    void PushRightMatrix(mat4 matrix);
    void PushLeftMatrix(mat4 matrix);
    
    void SetWireframe(bool on_wireframe) { mIsWireframe = on_wireframe; };
    void LoadTexture2DSimpleBmp(const char *name,int header_size,int Width,int Height,int BGR=0);
    void SetTextureCombinationEnabled(bool enabled) {mIsTextureCombinationEnabled = enabled;};
    bool GetTextureCombinationEnabled() const;
    
    void AddDependency(Object* obj);
    void AddDependencies(vector<Object*> objects);

    void SetPivot(float x, float y, float z);
    void SetPivot(vec3 pivot);

    vec3 GetPivot() const;

    void Model(mat4 modelMatrix);
    void EvalModelMatrix();

    void SetInheritedModel(mat4 matrixModel, bool propagate = true);

private:
    void Render(GLint position,GLint normal,GLint texcoord);

    bool mIsIndexed;
    bool mIsWireframe;
    bool mNoNormal;
    float mColor[3];
    float mShininess;
    float mStrength;
    
    vec3 mPivot;
    vec3 mPosition;
    vec3 mEulerAngles;
    vec3 mScale;
    bool isModelMatrixDirty;

    mat4 mModelMatrix;
    mat4 mInheritedModelMatrix;

    GLuint mVBO;
    GLuint mEBO;
    int mVerticesNumber;
    int mIndexNumber;

    bool mIsNoTexture;
    Texture *mTextures;
    bool mIsTextureCombinationEnabled;

    //Dependencies for facilitating transformations
    //it's not great, but in this case, it's ok
    vector<Object*> mDependencies;
};

Object::Object(int verticesNumber,int indexNumber, GLfloat *Vertices_information,GLushort *index):mVerticesNumber(verticesNumber),mIndexNumber(indexNumber){
    mIsIndexed = true;
    mIsWireframe = false;
    mNoNormal = false;
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
    mModelMatrix = mat4(1.0);
    mInheritedModelMatrix = mat4(1.0);

    isModelMatrixDirty = false;
    mPosition = vec3(0.0);
    mEulerAngles = vec3(0.0);
    mScale = vec3(1.0);

    mPivot[0] = mPivot[1] = mPivot[2] = 0;
    mIsTextureCombinationEnabled = false;
}

Object::~Object(){
    glDeleteBuffers(1, &mVBO);
    if (mIsIndexed)
        glDeleteBuffers(1, &mEBO);
}

void Object::SetPosition(vec3 position) { 
    mPosition = position; 
    isModelMatrixDirty = true;
}

void Object::SetEulerAngles(vec3 eulerAngles) { 
    mEulerAngles = eulerAngles; 
    isModelMatrixDirty = true;
}

void Object::SetScale(vec3 scale) { 
    mScale = scale; 
    isModelMatrixDirty = true;
}

void Object::SetColor(float r, float g, float b){
    mColor[0] = r;
    mColor[1] = g;
    mColor[2] = b;
    return;
}

void Object::SetPivot(float x, float y, float z){
    mPivot = vec3(x, y, z);
}

void Object::SetPivot(vec3 pivot) {
    mPivot = pivot;
}

vec3 Object::GetPivot() const{
    return mPivot;
}

/// Prefer using SetPosition, SetRotation and SetEulerAngles to work with the model outside the class
void Object::Model(mat4 modelMatrix){
    mModelMatrix = modelMatrix;
    for (Object* obj : mDependencies) {
        obj->SetInheritedModel(modelMatrix);
    }
}

void Object::EvalModelMatrix() {
    mat4 objScale = scale(mat4(1.0), vec3(mScale));
    mat4 objPosition = translate(mat4(1.0), vec3(mPosition));
    
    mat4 toPivot = translate(mat4(1.0), -vec3(mPivot));

    mat4 objRotation = mat4(1.0);
    objRotation = rotate(objRotation, radians(mEulerAngles.x), vec3(1.0f, 0.0f, 0.0f));
    objRotation = rotate(objRotation, radians(mEulerAngles.y), vec3(0.0f, 1.0f, 0.0f));
    objRotation = rotate(objRotation, radians(mEulerAngles.z), vec3(0.0f, 0.0f, 1.0f));

    mat4 fromPivot = translate(mat4(1.0), vec3(mPivot));

    mat4 objTransform = mInheritedModelMatrix * objPosition * fromPivot * objRotation * toPivot;

    Model(objTransform * objScale);
}

void Object::SetInheritedModel(mat4 modelMatrix, bool propagate) {
    mInheritedModelMatrix = modelMatrix;
    isModelMatrixDirty = true;

    if (propagate) {
        for (Object* obj : mDependencies) {
            obj->SetInheritedModel(mInheritedModelMatrix * mModelMatrix);
        }
    }
}


void Object::PushRightMatrix(mat4 matrix){
    mModelMatrix = mModelMatrix * matrix;
}

void Object::PushLeftMatrix(mat4 matrix){
    mModelMatrix = matrix * mModelMatrix;
}

mat4 Object::GetModelMatrix() {
    return mModelMatrix;
}

bool Object::GetTextureCombinationEnabled() const{
    return mIsTextureCombinationEnabled;
}

void Object::AddDependency(Object* obj){
    mDependencies.push_back(obj);
}

void Object::AddDependencies(vector<Object*> objects) {
    for (auto obj : objects) {
        AddDependency(obj);
    }
}

void Object::LoadTexture2DSimpleBmp(const char *name,int headerSize,int width,int height,int BGR){
    if(mIsNoTexture){
        cout << "Object without uv " << endl;
        exit(1);
    }
    mTextures = new Texture(GL_TEXTURE_2D,name);
    mTextures->LoadSimpleBmp(headerSize,width,height,BGR);
}

void Object::Render(GLint position,GLint normal,GLint texcoord){    
    if (isModelMatrixDirty) {
        EvalModelMatrix();
        isModelMatrixDirty = false;
    }

    if (mNoNormal || mIsNoTexture){
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