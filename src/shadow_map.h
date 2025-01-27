#ifndef SHADOWMAPFBO_H
#define	SHADOWMAPFBO_H

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


class ShadowMapFBO
{
public:
    ShadowMapFBO();

    ~ShadowMapFBO();

    bool Init(unsigned int WindowWidth, unsigned int WindowHeight);

    void BindForWriting();

    void BindForReading(GLenum TextureUnit);
    int getWidth(){return  m_Width;};
    int getHeight(){return m_Height;};

private:
    int m_Width,m_Height;
    GLuint m_fbo;
    GLuint m_shadowMap;
};

ShadowMapFBO::ShadowMapFBO()
{
    m_fbo = 0;
    m_shadowMap = 0;
    m_Width = 800;
    m_Height = 800;
}

ShadowMapFBO::~ShadowMapFBO()
{
    if (m_fbo != 0) {
        glDeleteFramebuffers(1, &m_fbo);
    }

    if (m_shadowMap != 0) {
        glDeleteTextures(1, &m_shadowMap);
    }
}

bool ShadowMapFBO::Init(unsigned int Width, unsigned int Height)
{
    m_Width = Width;
    m_Height = Height;
    // Create the FBO
    glGenFramebuffers(1, &m_fbo);

    // Create the depth buffer
    glGenTextures(1, &m_shadowMap);
    glBindTexture(GL_TEXTURE_2D, m_shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Width, Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMap, 0);
  
   // Disable writes to the color buffer
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
  
    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    
    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    

    return true;
}


void ShadowMapFBO::BindForWriting()
{
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    /*
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        cout << "Error no BindForWriting " << endl;
        if(error == GL_INVALID_ENUM )
            cout << "GL_INVALID_ENUM" << endl;
    }
    */
    glViewport(0,0,m_Width,m_Height);
  
}


void ShadowMapFBO::BindForReading(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);    
    glBindTexture(GL_TEXTURE_2D, m_shadowMap);
    /*
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        cout << "Error no BindForWriting " << endl;
        if(error == GL_INVALID_ENUM )
            cout << "GL_INVALID_ENUM" << endl;
    }
    */
}

#endif	/* SHADOWMAPFBO_H */
