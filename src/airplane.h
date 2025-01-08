#ifndef AIRPLANE_H__
#define AIRPLANE_H__

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include "object.h"

using namespace std;
using namespace glm;

class Airplane {
public:
    Airplane();
	void Update();
	void SetPosition(vec3 position);
	void SetEulerAngles(vec3 eulerAngles);
	void Move(vec3 movement);
    vector<Object*> GetObjects();
private:
	float mSpeed;
	
	vec3 mPosition;
	vec3 mEulerAngles;

    vector<Object*> mObjects;

	Object* mBody;
    Object* mElevator;

	Object* mLeftAileron;
	Object* mRightAileron;

	Object* mLeftRudder;
	Object* mRightRudder;

	Object* mLeftEngine;
	Object* mRightEngine;

	Object* mLeftFlap;
	Object* mRightFlap;

	Object* mLeftFan;
	Object* mRightFan;
};

void Airplane::Update() {

}

void Airplane::SetPosition(vec3 position) {

}

void Airplane::SetEulerAngles(vec3 eulerAngles) {

}

void Airplane::Move(vec3 movement) {

}

Airplane::Airplane() {
    // wip
    int x = 122;
    int y = 256;
    int z = 256;

    mBody = read_obj_file("models/osprey/Body.obj");
    mBody->LoadTexture2DSimpleBmp("models/TexturaMetal.bmp", x, y, z);

    mElevator = read_obj_file("models/osprey/Elevator.obj");
    mElevator->LoadTexture2DSimpleBmp("models/TexturaMetal.bmp", x, y, z);

    mLeftAileron = read_obj_file("models/osprey/Left_Aileron.obj");
    mLeftAileron->LoadTexture2DSimpleBmp("models/TexturaMetal.bmp", x, y, z);

    mLeftEngine = read_obj_file("models/osprey/Left_Engine.obj");
    mLeftEngine->LoadTexture2DSimpleBmp("models/TexturaMetal.bmp", x, y, z);

    mLeftFan = read_obj_file("models/osprey/Left_Fan.obj");
    mLeftFan->LoadTexture2DSimpleBmp("models/TexturaMetal.bmp", x, y, z);

    mLeftFlap = read_obj_file("models/osprey/Left_Flap.obj");
    mLeftFlap->LoadTexture2DSimpleBmp("models/TexturaMetal.bmp", x, y, z);

    mLeftRudder = read_obj_file("models/osprey/Left_Rudder.obj");
    mLeftRudder->LoadTexture2DSimpleBmp("models/TexturaMetal.bmp", x, y, z);

    mRightAileron = read_obj_file("models/osprey/Right_Aileron.obj");
    mRightAileron->LoadTexture2DSimpleBmp("models/TexturaMetal.bmp", x, y, z);

    mRightEngine = read_obj_file("models/osprey/Right_Engine.obj");
    mRightEngine->LoadTexture2DSimpleBmp("models/TexturaMetal.bmp", x, y, z);

    mRightFan = read_obj_file("models/osprey/Right_Fan.obj");
    mRightFan->LoadTexture2DSimpleBmp("models/TexturaMetal.bmp", x, y, z);

    mRightFlap = read_obj_file("models/osprey/Right_Flap.obj");
    mRightFlap->LoadTexture2DSimpleBmp("models/TexturaMetal.bmp", x, y, z);

    mRightRudder = read_obj_file("models/osprey/Right_Rudder.obj");
    mRightRudder->LoadTexture2DSimpleBmp("models/TexturaMetal.bmp", x, y, z);

    mObjects.push_back(mBody);
    mObjects.push_back(mElevator);
    mObjects.push_back(mLeftAileron);
    mObjects.push_back(mLeftEngine);
    mObjects.push_back(mLeftFan);
    mObjects.push_back(mLeftFlap);
    mObjects.push_back(mLeftRudder);
    mObjects.push_back(mRightAileron);
    mObjects.push_back(mRightEngine);
    mObjects.push_back(mRightFan);
    mObjects.push_back(mRightFlap);
    mObjects.push_back(mRightRudder);
}

vector<Object*> Airplane::GetObjects() {
    return mObjects;
}

#endif 