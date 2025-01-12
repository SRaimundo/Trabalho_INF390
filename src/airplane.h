#ifndef AIRPLANE_H__
#define AIRPLANE_H__

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include "utils.h"
#include "object.h"

using namespace std;
using namespace glm;

struct AirplaneInput {
    bool mLeft = false;
    bool mRight = false;
    bool mUp = false;
    bool mDown = false;
};

class Airplane {
public:
    Airplane();

	void Update();
    void Input(AirplaneInput input);
    
    void SetPosition(vec3 position);
    void SetEulerAngles(vec3 eulerAngles);


    vec3 GetPosition() { return mBody->GetPosition(); }
    vec3 GetEulerAngles() { return mBody->GetEulerAngles(); };

    mat4 GetModelMatrix() const;

	void Move(vec3 movement);
    vector<Object*> GetObjects();

private:
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

    vec3 mMovement = vec3(0);

    float mSpeed = 2;
    float mFanRotation = 0;
    float mFanRotationSpeed = 15;
};

void Airplane::Update() {
    mFanRotation += fmod(mFanRotationSpeed, 360);
    
    mLeftFan->SetEulerAngles(vec3(0, mFanRotation, 0));
    mRightFan->SetEulerAngles(vec3(0, mFanRotation, 0));

    mMovement.x = mSpeed;

//    printf("pos: %s rot: %s \n", 
//        toString(mBody->GetPosition()).c_str(), 
//        toString(mBody->GetPosition()).c_str());
}

void Airplane::Input(AirplaneInput input) {
    printf("%d %d %d %d \n", input.mLeft, input.mRight, input.mUp, input.mDown);
}

void Airplane::SetPosition(vec3 position) {
    mBody->SetPosition(position);
}

void Airplane::SetEulerAngles(vec3 eulerAngles) {
    mBody->SetEulerAngles(eulerAngles);
}

void Airplane::Move(vec3 movement) {
    mBody->SetPosition(mBody->GetPosition() + movement);
}

glm::mat4 Airplane::GetModelMatrix() const{
    return mBody->GetModelMatrix();
}

Airplane::Airplane() {
    // wip
    int x = 0;
    int y = 1000;
    int z = 1000;

    mBody = read_obj_file("models/osprey/Body.obj");
    mBody->LoadTexture2DSimpleBmp("models/uvmap.bmp", x, y, z);

    mElevator = read_obj_file("models/osprey/Elevator.obj");
    mElevator->LoadTexture2DSimpleBmp("models/uvmap.bmp", x, y, z);

    mLeftAileron = read_obj_file("models/osprey/Left_Aileron.obj");
    mLeftAileron->LoadTexture2DSimpleBmp("models/uvmap.bmp", x, y, z);

    mLeftEngine = read_obj_file("models/osprey/Left_Engine.obj");
    mLeftEngine->LoadTexture2DSimpleBmp("models/uvmap.bmp", x, y, z);

    mLeftFan = read_obj_file("models/osprey/Left_Fan.obj");
    mLeftFan->LoadTexture2DSimpleBmp("models/uvmap.bmp", x, y, z);
    mLeftFan->SetPivot(vec3(-0.09, 5.7, 6.6));

    mLeftFlap = read_obj_file("models/osprey/Left_Flap.obj");
    mLeftFlap->LoadTexture2DSimpleBmp("models/uvmap.bmp", x, y, z);

    mLeftRudder = read_obj_file("models/osprey/Left_Rudder.obj");
    mLeftRudder->LoadTexture2DSimpleBmp("models/uvmap.bmp", x, y, z);

    mRightAileron = read_obj_file("models/osprey/Right_Aileron.obj");
    mRightAileron->LoadTexture2DSimpleBmp("models/uvmap.bmp", x, y, z);

    mRightEngine = read_obj_file("models/osprey/Right_Engine.obj");
    mRightEngine->LoadTexture2DSimpleBmp("models/uvmap.bmp", x, y, z);

    mRightFan = read_obj_file("models/osprey/Right_Fan.obj");
    mRightFan->LoadTexture2DSimpleBmp("models/uvmap.bmp", x, y, z);
    mRightFan->SetPivot(vec3(-0.09, 5.7, -6.6));

    mRightFlap = read_obj_file("models/osprey/Right_Flap.obj");
    mRightFlap->LoadTexture2DSimpleBmp("models/uvmap.bmp", x, y, z);

    mRightRudder = read_obj_file("models/osprey/Right_Rudder.obj");
    mRightRudder->LoadTexture2DSimpleBmp("models/uvmap.bmp", x, y, z);

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

    mBody->AddDependencies(vector<Object*> { 
        mLeftAileron, 
        mLeftFlap, 
        mRightAileron, 
        mRightFlap, 
        mLeftEngine, 
        mRightEngine,
        mLeftEngine, 
        mRightEngine,
        mElevator, 
        mLeftRudder,
        mRightRudder
    });

    mRightEngine->AddDependency(mRightFan);
    mLeftEngine->AddDependency(mLeftFan);

}

vector<Object*> Airplane::GetObjects() {
    return mObjects;
}

#endif 