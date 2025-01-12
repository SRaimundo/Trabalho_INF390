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

    AirplaneInput mInput; 

    float mSpeed = 2;
    float mFanRotation = 0;
    float mFanRotationSpeed = 100;
};

void Airplane::Update() {
    
    mFanRotation += fmod(mFanRotationSpeed, 360);
    
    mLeftFan->SetEulerAngles(blenderAxis(vec3(0, 0, mFanRotation)));
    mRightFan->SetEulerAngles(blenderAxis(vec3(0, 0, mFanRotation + 15)));

    mMovement.x = mSpeed;

    vec3 elevatorEulerAngles = blenderAxis(mElevator->GetEulerAngles());
    vec3 bodyEulerAngles = blenderAxis(mBody->GetEulerAngles());

    // pitch
    if (mInput.mDown || mInput.mUp) {
        elevatorEulerAngles.y = Lerp(elevatorEulerAngles.y, mInput.mDown ? -30.0 : 30, 0.1);
        mElevator->SetEulerAngles(glAxis(elevatorEulerAngles));

        bodyEulerAngles.y = Lerp(bodyEulerAngles.y, mInput.mDown ? -25.0 : 25, 0.05);
        mBody->SetEulerAngles(glAxis(bodyEulerAngles));
    }
    else {
        elevatorEulerAngles.y = Lerp(elevatorEulerAngles.y, 0, 0.03);
        mElevator->SetEulerAngles(glAxis(elevatorEulerAngles));

        bodyEulerAngles.y = Lerp(bodyEulerAngles.y, 0, 0.01);
        mBody->SetEulerAngles(glAxis(bodyEulerAngles));
    }

    //
    if (mInput.mLeft || mInput.mRight) {
        bodyEulerAngles.x = Lerp(bodyEulerAngles.x, mInput.mRight ? -25.0 : 25, 0.05);
        mBody->SetEulerAngles(glAxis(bodyEulerAngles));
    }
    else {
        bodyEulerAngles.x = Lerp(bodyEulerAngles.x, 0, 0.02);
        mBody->SetEulerAngles(glAxis(bodyEulerAngles));
    }

//    printf("pos: %s rot: %s \n", 
//        toString(mBody->GetPosition()).c_str(), 
//        toString(mBody->GetPosition()).c_str());
}

void Airplane::Input(AirplaneInput input) {
    printf("%d %d %d %d \n", input.mLeft, input.mRight, input.mUp, input.mDown);
    mInput = input;
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
    mElevator->SetPivot(blenderAxis(vec3(8.34f, -0.12f, 2.82f)));

    mLeftAileron = read_obj_file("models/osprey/Left_Aileron.obj");
    mLeftAileron->LoadTexture2DSimpleBmp("models/uvmap.bmp", x, y, z);
    mLeftAileron->SetPivot(blenderAxis(vec3(0.29f, -4.5f, 3.84f)));

    mLeftEngine = read_obj_file("models/osprey/Left_Engine.obj");
    mLeftEngine->LoadTexture2DSimpleBmp("models/uvmap.bmp", x, y, z);
    mLeftEngine->SetPivot(blenderAxis(vec3(-0.55f, -6.12f, 3.98f)));

    mLeftFan = read_obj_file("models/osprey/Left_Fan.obj");
    mLeftFan->LoadTexture2DSimpleBmp("models/uvmap.bmp", x, y, z);
    mLeftFan->SetPivot(blenderAxis(vec3(-0.09, -6.6, 5.7)));

    mLeftFlap = read_obj_file("models/osprey/Left_Flap.obj");
    mLeftFlap->LoadTexture2DSimpleBmp("models/uvmap.bmp", x, y, z);
    mLeftFlap->SetPivot(blenderAxis(vec3(0.38f, -2.57f, 3.76f)));

    mLeftRudder = read_obj_file("models/osprey/Left_Rudder.obj");
    mLeftRudder->LoadTexture2DSimpleBmp("models/uvmap.bmp", x, y, z);
    mLeftRudder->SetPivot(blenderAxis(vec3(8.16f, -3, 4.3f)));

    mRightAileron = read_obj_file("models/osprey/Right_Aileron.obj");
    mRightAileron->LoadTexture2DSimpleBmp("models/uvmap.bmp", x, y, z);
    mRightAileron->SetPivot(blenderAxis(vec3(0.29f, 4.5f, 3.84f)));

    mRightEngine = read_obj_file("models/osprey/Right_Engine.obj");
    mRightEngine->LoadTexture2DSimpleBmp("models/uvmap.bmp", x, y, z);
    mRightEngine->SetPivot(blenderAxis(vec3(-0.55f, 6.12f, 3.98f)));

    mRightFan = read_obj_file("models/osprey/Right_Fan.obj");
    mRightFan->LoadTexture2DSimpleBmp("models/uvmap.bmp", x, y, z);
    mRightFan->SetPivot(blenderAxis(vec3(-0.09, 6.6, 5.7)));

    mRightFlap = read_obj_file("models/osprey/Right_Flap.obj");
    mRightFlap->LoadTexture2DSimpleBmp("models/uvmap.bmp", x, y, z);
    mRightFlap->SetPivot(blenderAxis(vec3(0.38f, 2.57f, 3.76f)));

    mRightRudder = read_obj_file("models/osprey/Right_Rudder.obj");
    mRightRudder->LoadTexture2DSimpleBmp("models/uvmap.bmp", x, y, z);
    mRightRudder->SetPivot(blenderAxis(vec3(8.16f, 3, 4.3f)));

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