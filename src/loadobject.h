#ifndef LOAD_H__
#define LOAD_H__
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <vector>
#include "utils.h"
using namespace std;
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "object.h"
#include "scene.h"
#include <map>

Object *read_obj_file(string filename){
    std::ifstream file(filename);
    if(!file){
        std::cerr << "Error opening file: " << filename << std::endl;
        throw std::ios_base::failure("Error opening file: " + filename);
    }
    vector<GLfloat> position;
    vector<GLfloat> normal;
    vector<GLfloat> uv;
    vector<GLushort> index_position;
    vector<GLushort> index_normal;
    vector<GLushort> index_uv;
    std::string line;
    while(std::getline(file, line)){
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        if(token == "v"){
            float x, y, z;
            iss >> x >> y >> z;
            position.push_back(x);
            position.push_back(y);
            position.push_back(z);
        }else if(token == "vn"){
            float nx, ny, nz;
            iss >> nx >> ny >> nz;
            normal.push_back(nx);
            normal.push_back(ny);
            normal.push_back(nz);
        }else if(token == "vt"){
            float u, v;
            iss >> u >> v;
            uv.push_back(u);
            uv.push_back(v);
        }else if(token == "f"){
            for(int i = 0; i < 3; ++i){
                uint32_t index_v, index_n, index_u;
                iss >> index_v;
                index_position.push_back(index_v - 1);
                if(iss.peek() == '/'){
                    iss.ignore();
                    if(iss.peek() != '/'){
                        iss >> index_u;
                        index_uv.push_back(index_u - 1);
                    }if(iss.peek() == '/'){
                        iss.ignore();
                        iss >> index_n;
                        index_normal.push_back(index_n - 1);
                    }
                }
            }
        }
    }
    file.close();
    Object *my_obj;
    if(normal.empty()){
        std::cerr << "Error parsing obj file: " << filename << std::endl;
        throw std::ios_base::failure("Error parsing obj file: " + filename);

    }else{
        map<string, int> index_unique;
        vector<GLushort> index_final;
        vector<GLfloat> my_data;
        int key_index = 0;
        for(int i = 0; i < index_position.size(); i++){
            string key = std::to_string(index_position[i])+":"+std::to_string(index_normal[i])
            +":"+std::to_string(index_uv[i]);
            if(index_unique.find(key) == index_unique.end()){
                index_unique[key] = key_index;
                my_data.push_back(position[index_position[i]*3]);
                my_data.push_back(position[index_position[i]*3+1]);
                my_data.push_back(position[index_position[i]*3+2]);
                my_data.push_back(normal[index_normal[i]*3]);
                my_data.push_back(normal[index_normal[i]*3+1]);
                my_data.push_back(normal[index_normal[i]*3+2]);
                my_data.push_back(uv[index_uv[i]*2]);
                my_data.push_back(uv[index_uv[i]*2+1]);
                index_final.push_back(key_index);
                key_index++;
            }
            else{
                index_final.push_back(index_unique[key]);
            }
        }
        my_obj = new Object(my_data.size()/8,index_final.size(),my_data.data(),index_final.data());
    }
    return my_obj;
}

#endif