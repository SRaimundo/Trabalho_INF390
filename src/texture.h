#ifndef TEXTURE_H__
#define TEXTURE_H__

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


class Texture{
    public:
        friend class scene;
        Texture(GLenum TextureTarget, const std::string& FileName);
        bool LoadSimpleBmp(int header_size,int Width,int Height,int BGR);
        void Bind(GLenum TextureUnit);
    private:
        
        std::string mFileName;
        GLenum mTextureTarget;
        GLuint mTextureObj;
        int mImageWidth = 0;
        int mImageHeight = 0;
        int mImageBPP = 0;
        void LoadInternal(void* image_data);
};

Texture::Texture(GLenum TextureTarget, const std::string& FileName){
    mTextureTarget = TextureTarget;
    mFileName      = FileName;
}

bool Texture::LoadSimpleBmp(int header_size,int Width,int Height,int BGR){
   GLubyte tex[Height][Width][3];
   try{
        ifstream arq(mFileName,ios::binary);
        char c;
        if(!arq){
            cout << "Error ao abrir" << mFileName;
            exit(1);
        }
        int i = 0;
        for(int i = 0; i < header_size ; i++)
            c = arq.get();
        for(int i = 0; i < Height ; i++)
            for(int j = 0; j < Width ; j++){
                if(!(BGR)){
                    c = arq.get();
                    tex[i][j][2] = c;
                    c =  arq.get();
                    tex[i][j][1] = c ;
                    c =  arq.get();
                    tex[i][j][0] = c;
                }else{
                    c = arq.get();
                    tex[i][j][0] = c;
                    c =  arq.get();
                    tex[i][j][1] = c ;
                    c =  arq.get();
                    tex[i][j][2] = c;
                }
            }

        arq.close();
        arq.clear();
    }
    catch(...){
        cout << "Erro ao ler imagem" << endl;
        return false;
    }
   
   
    mImageWidth = Width;
    mImageHeight = Height;
    mImageBPP = 3;
    LoadInternal((void*)tex);
    return true;
}

void Texture::LoadInternal(void* image_data){
    glGenTextures(1, &mTextureObj);
    glBindTexture(mTextureTarget, mTextureObj);

    if(mTextureTarget == GL_TEXTURE_2D){
        switch (mImageBPP) {
        case 1:
            glTexImage2D(mTextureTarget, 0, GL_RED, mImageWidth, mImageHeight, 0, GL_RED, GL_UNSIGNED_BYTE, image_data);
            break;

        case 2:
            glTexImage2D(mTextureTarget, 0, GL_RG, mImageWidth, mImageHeight, 0, GL_RG, GL_UNSIGNED_BYTE, image_data);
            break;

        case 3:
            glTexImage2D(mTextureTarget, 0, GL_RGB, mImageWidth, mImageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
            break;

        case 4:
            glTexImage2D(mTextureTarget, 0, GL_RGBA, mImageWidth, mImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
            break;

        }
    }else{
        printf("Support for texture target %x is not implemented\n", mTextureTarget);
        exit(1);
    }

    glTexParameteri(mTextureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(mTextureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(mTextureTarget, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(mTextureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(mTextureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glGenerateMipmap(mTextureTarget);

    glBindTexture(mTextureTarget, 0);
}



void Texture::Bind(GLenum TextureUnit){
    glActiveTexture(TextureUnit);
    glBindTexture(mTextureTarget, mTextureObj);
}

#endif