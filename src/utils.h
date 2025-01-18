#ifndef UTILS_H__
#define UTILS_H__

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#define GLM_ENABLE_EXPERIMENTAL
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace std;
using namespace glm;

GLchar *readTextFile(const char *name){
    FILE *fp;
    GLchar *content = NULL;
    int cont = 0;
    if (name == NULL)
        return NULL;
    fp = fopen(name, "rt");
    if (fp == NULL)
        return NULL;
    fseek(fp, 0, SEEK_END);
    cont = ftell(fp);
    rewind(fp);
    if (cont > 0){
        content = (GLchar *)malloc(sizeof(char) * (cont + 1));
        if (content != NULL){
            cont = fread(content, sizeof(char), cont, fp);
            content[cont] = '\0';
        }
    }

    fclose(fp);
    return content;
}

static string toString(vec3 v) {
    stringstream ss;
    ss << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return ss.str();
}

static vec3 blenderAxis(vec3 v) {
    return vec3(v.x, v.z, -v.y);
}

static vec3 glAxis(vec3 v) {
    return vec3(v.x, -v.z, v.y);
}

static vec3 Forward(float x, float y, float z) {
    float pitch = radians(x);
    float yaw = radians(y);
    float roll = radians(z);

    quat quaternion = quat(vec3(pitch, yaw, roll));

    mat4 rotationMatrix = toMat4(quaternion);

    vec3 forward = vec3(rotationMatrix[2]); 
    forward = -normalize(forward); 

    return forward;
}

static vec3 Forward(vec3 eulerAngles) {
    return Forward(eulerAngles.x, eulerAngles.y, eulerAngles.z);
}


static double Lerp(double a, double b, double t)
{
    if (t <= 0.5)
        return a + (b - a) * t;
    else
        return b - (b - a) * (1.0 - t);
}

static bool Near(double a, double b, double epsilon = 0.01) {
    return abs(a - b) < epsilon;
}

static void CreateVertexBufferTriangle(GLuint &VBO, GLuint &EBO, int &v_number, int &i_number){
    GLfloat Vertices[9][3] = {
        {0.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f},
        {-0.7f, 0.7f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.7f, 0.7f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {0.7f, -0.7f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {-0.7f, -0.7f, 0.0f}};
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    v_number = 9;

    GLushort index[24] = {
        0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 5, 0, 5, 6, 0, 6, 7, 0, 7, 8, 0, 8, 1};
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
    i_number = 24;
}

static void AddShader(GLuint ShaderProgram, const char *pShaderText, GLenum ShaderType){
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0){
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }

    const GLchar *p[1];
    p[0] = pShaderText;
    // cout << pShaderText;
    GLint Lengths[1];
    Lengths[0] = strlen(pShaderText);
    glShaderSource(ShaderObj, 1, p, Lengths);
    glCompileShader(ShaderObj);
    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if (!success){
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }

    glAttachShader(ShaderProgram, ShaderObj);
}

static GLuint CompileShaders(const char *pVSFileName, const char *pFSFileName){
    GLuint ShaderProgram = glCreateProgram();

    if (ShaderProgram == 0){
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    char *shv, *shf;
    shv = readTextFile(pVSFileName);

    if (shv == NULL){
        fprintf(stderr, "Error load shader file!\n");
        exit(1);
    };

    shf = readTextFile(pFSFileName);

    if (shf == NULL){
        fprintf(stderr, "Error load shader file!\n");
        exit(1);
    };

    AddShader(ShaderProgram, shv, GL_VERTEX_SHADER);

    AddShader(ShaderProgram, shf, GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024] = {0};

    glLinkProgram(ShaderProgram);

    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
    if (Success == 0){
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
    if (!Success){
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    return ShaderProgram;
}

#endif