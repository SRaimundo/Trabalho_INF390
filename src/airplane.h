#ifndef AIRPLANE_H__
#define AIRPLANE_H__

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <algorithm>
#include "utils.h"
#include "object.h"

using namespace std;
using namespace glm;

struct AirplaneInput {
    bool mLeft = false;
    bool mRight = false;
    bool mUp = false;
    bool mDown = false;
    bool mTakeOff = false;
    bool mLand = false;
};

struct Tween {
    float* value;
    float startValue;
    float finalValue;
    float stepPercentage;
    bool ended = false;
};

class Airplane {
public:
    Airplane();

	void Update();
    void Input(AirplaneInput input);
    
    void SetPosition(vec3 position);
    void SetEulerAngles(vec3 eulerAngles);

    void TakeOff();
    void Land();

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

    bool mIsTakingOff = false; 
    bool mIsLanding = false; 
    bool mIsGrounded = true;

    float mSpeed = 0.05;
    float mFanRotation = 0;
    float mFanRotationSpeed = 0;
    float mFanMaxRotationSpeed = 50;
    float mRollSpeed = 2;

    float mMaxRoll = 80;
    float mMaxPitch = 50;

    list<Tween> mTweens = {};
    list<Tween*> mTweensToClean = {};

    void TweenRoutine();
};


// just testing what could be done with tweens instead of handling so many lerps all around
// currently not being used, but prob will use to reset plane rotations when landing and taking off (instead of 
// handling many states from each part)
void Airplane::TweenRoutine() {
    for (auto tween : mTweens) {
        float currentValue = *(tween.value);
        currentValue = currentValue + ((tween.finalValue - tween.startValue) * tween.stepPercentage);
        if (Near(*(tween.value), tween.finalValue, 0.001)) {
            currentValue = tween.finalValue;
            tween.ended = true;
        }
        *(tween.value) = currentValue;
    }

    // credits: https://stackoverflow.com/questions/67710057/how-exactly-remove-if-works
    mTweens.resize(std::distance(
        mTweens.begin(),
        remove_if(mTweens.begin(), mTweens.end(), [](Tween t) {return t.ended;})
    ));
}

void Airplane::Land() {

    float landTimeScale = 3.0f;

    vec3 bodyEulerAngles = blenderAxis(GetEulerAngles());
    if (!Near(bodyEulerAngles.x, 0) || !Near(bodyEulerAngles.z, 0)) {
        bodyEulerAngles.x = Lerp(bodyEulerAngles.x, 0, 0.01 * landTimeScale);
        bodyEulerAngles.y = Lerp(bodyEulerAngles.y, 0, 0.01 * landTimeScale);

        SetEulerAngles(glAxis(bodyEulerAngles));
        return;
    }

    vec3 leftEngineEulerAngles = blenderAxis(mLeftEngine->GetEulerAngles());
    vec3 rightEngineEulerAngles = blenderAxis(mRightEngine->GetEulerAngles());
    vec3 position = blenderAxis(mBody->GetPosition());

    leftEngineEulerAngles.y = Lerp(leftEngineEulerAngles.y, 0, 0.005 * landTimeScale);
    rightEngineEulerAngles.y = Lerp(rightEngineEulerAngles.y, 0, 0.005 * landTimeScale);

    float groundHeight = 0;
    // jet engines are upwards
    if (Near(leftEngineEulerAngles.y, 0, 0.5)) {
        leftEngineEulerAngles.y = 0;
        rightEngineEulerAngles.y = 0;

        if (!Near(position.z, groundHeight, 0.1)) {
            // decrease fan rotation speed 
            mFanRotationSpeed = Lerp(mFanRotationSpeed, 20, 0.003 * landTimeScale);

            float fanSpeedInverseRatio = 1 - (mFanRotationSpeed / mFanMaxRotationSpeed);

            // get the airplane down
            float positionLerpRatio = fanSpeedInverseRatio * fanSpeedInverseRatio * fanSpeedInverseRatio;
            position.z = Lerp(position.z, groundHeight, positionLerpRatio * 0.01 * landTimeScale);
            printf("%s\n", toString(position).c_str());
        }
        else {

            // on ground, begin to turn engines off
            mFanRotationSpeed = Lerp(mFanRotationSpeed, 0, 0.005 * landTimeScale);

            if (Near(mFanRotationSpeed, 0, 0.5)) {
                mFanRotationSpeed = 0;
                mIsLanding = false;
                mIsGrounded = true;
            }

            position.z = 0;
        }
    }

    mBody->SetPosition(glAxis(position));
    mLeftEngine->SetEulerAngles(glAxis(leftEngineEulerAngles));
    mRightEngine->SetEulerAngles(glAxis(rightEngineEulerAngles));
}

void Airplane::TakeOff() {
    
    float takeOffTimeScale = 3.0f;
    
    // increase fan rotation
    // initial take off
    vec3 leftEngineEulerAngles = blenderAxis(mLeftEngine->GetEulerAngles());
    vec3 rightEngineEulerAngles = blenderAxis(mRightEngine->GetEulerAngles());
    vec3 position = blenderAxis(mBody->GetPosition());

    float takeOffHeight = -40.0f;

    if (!Near(position.z, takeOffHeight, 1)) {
        mIsGrounded = false;

        mFanRotationSpeed = Lerp(mFanRotationSpeed, mFanMaxRotationSpeed, 0.001 * takeOffTimeScale);

        leftEngineEulerAngles.y = Lerp(leftEngineEulerAngles.y, 0, 0.1 * takeOffTimeScale);
        rightEngineEulerAngles.y = Lerp(rightEngineEulerAngles.y, 0, 0.1 * takeOffTimeScale);

        float fanSpeedRatio = mFanRotationSpeed / mFanMaxRotationSpeed;

        // due to how the axis are converted between gl and blender axis, direct z values have to be negative
        position.z = Lerp(position.z, takeOffHeight, fanSpeedRatio * fanSpeedRatio * fanSpeedRatio * 0.05 * takeOffTimeScale);
        printf("%s\n", toString(position).c_str());
        mBody->SetPosition(glAxis(position));
    }
    // on air, rotate the engines forward and finish takeoff
    else {
        float rotateEngineTo = -90;

        leftEngineEulerAngles.y = Lerp(leftEngineEulerAngles.y, rotateEngineTo, 0.02 * takeOffTimeScale);
        rightEngineEulerAngles.y = Lerp(rightEngineEulerAngles.y, rotateEngineTo, 0.02 * takeOffTimeScale);

        if (Near(leftEngineEulerAngles.y, rotateEngineTo, 1)) {
            mIsTakingOff = false;
            leftEngineEulerAngles.y = rotateEngineTo;
            rightEngineEulerAngles.y = rotateEngineTo;
        }
    }

    mLeftEngine->SetEulerAngles(glAxis(leftEngineEulerAngles));
    mRightEngine->SetEulerAngles(glAxis(rightEngineEulerAngles));
}

void Airplane::Update() {
    TweenRoutine();

    printf("Pos: %s - Rot: %s - Input: (l: %d r: %d u: %d d:%d)\n", 
        toString(GetPosition()).c_str(),
        toString(GetEulerAngles()).c_str(),
        mInput.mLeft, mInput.mRight, mInput.mUp, mInput.mDown);

    mFanRotation += fmod(mFanRotationSpeed, 360);
    mLeftFan->SetEulerAngles(glAxis(vec3(0, 0, mFanRotation)));
    mRightFan->SetEulerAngles(glAxis(vec3(0, 0, mFanRotation + 15)));

    if (mInput.mTakeOff && !mIsLanding && mIsGrounded) {
        mIsTakingOff = true;
    }

    if (mInput.mLand && !mIsTakingOff && !mIsGrounded) {
        mIsLanding = true;
    }

    if (mIsTakingOff) {
        TakeOff();
        return;
    }

    if (mIsLanding) {
        Land();
        return;
    }

    if (mIsGrounded) {
        return; 
    }


    vec3 elevatorEulerAngles = blenderAxis(mElevator->GetEulerAngles());
    vec3 bodyEulerAngles = blenderAxis(mBody->GetEulerAngles());

    quat currentOrientation = quat(vec3(bodyEulerAngles.y, bodyEulerAngles.z, bodyEulerAngles.x));
    vec3 localForward = currentOrientation * vec3(1.0, 0.0, 0.0);

    vec forward = normalize(Forward(-bodyEulerAngles.y, bodyEulerAngles.z, bodyEulerAngles.x));
    forward = vec3(-forward.z, -forward.y, forward.x);
    Move(forward * mSpeed);

    //printf("%s\n", toString(forward).c_str());
    
    // pitch
    if (mInput.mDown || mInput.mUp) {
        elevatorEulerAngles.y = Lerp(elevatorEulerAngles.y, mInput.mDown ? 50.0 : -50, 0.05);
        mElevator->SetEulerAngles(glAxis(elevatorEulerAngles));

        bodyEulerAngles.y = Lerp(bodyEulerAngles.y, mInput.mDown ? bodyEulerAngles.y + mMaxPitch : bodyEulerAngles.y - mMaxPitch, 0.02);
        mBody->SetEulerAngles(glAxis(bodyEulerAngles));
    }
    else {
        elevatorEulerAngles.y = Lerp(elevatorEulerAngles.y, 0, 0.03);
        mElevator->SetEulerAngles(glAxis(elevatorEulerAngles));

        mBody->SetEulerAngles(glAxis(bodyEulerAngles));
    }

    vec3 lAileronEulerAngles = blenderAxis(mLeftAileron->GetEulerAngles());
    vec3 rAileronEulerAngles = blenderAxis(mRightAileron->GetEulerAngles());

    vec3 rudderEulerAngles = blenderAxis(mLeftRudder->GetEulerAngles());
    // roll
    if (mInput.mLeft || mInput.mRight) {

#pragma region retry
        //quat q = quat(GetEulerAngles(), localForward);

        //vec3 rollEuler = normalize(eulerAngles(rollQuat) * currentOrientation * vec3(-1, 0, 0));
        //printf("rollEuler: %s", toString(rollEuler).c_str());

        //bodyEulerAngles += mInput.mRight ? rollEuler : -rollEuler;

        //currentOrientation = glm::normalize(rollQuat * currentOrientation);
        //vec3 newForward = currentOrientation * glm::vec3(0.0f, 0.0f, -1.0f);

        //bodyEulerAngles.x = Lerp(bodyEulerAngles.x, mInput.mRight ? bodyEulerAngles.x + mMaxRoll : bodyEulerAngles.x -mMaxRoll, 0.05);
        //bodyEulerAngles.y = Lerp(bodyEulerAngles.y, mInput.mDown ? bodyEulerAngles.y + mMaxPitch : bodyEulerAngles.y - mMaxPitch, 0.02);

        //vec3 pivot = mBody->GetPivot();
        //mat4 objRotation = mat4(1.0);
        //vec3 eulerA = GetEulerAngles();
        //mat4 toPivot = translate(mat4(1.0), -vec3(pivot));

        //objRotation = rotate(objRotation, radians(eulerA.x), vec3(1.0f, 0.0f, 0.0f));
        //objRotation = rotate(objRotation, radians(eulerA.y), vec3(0.0f, 1.0f, 0.0f));
        //objRotation = rotate(objRotation, radians(eulerA.z), vec3(0.0f, 0.0f, 1.0f));

        //mat4 objRotation2 = mat4(1.0);
        //objRotation2 = rotate(objRotation2, radians(eulerA.x), vec3(1.0f, 0.0f, 0.0f));
        //objRotation2 = rotate(objRotation2, radians(eulerA.y + 5.f), vec3(0.0f, 1.0f, 0.0f));
        //objRotation2 = rotate(objRotation2, radians(eulerA.z), vec3(0.0f, 0.0f, 1.0f));

        //mat4 fromPivot = translate(mat4(1.0), vec3(pivot));
        //mat4 objTransform = fromPivot * objRotation2 * toPivot;
        //
        //vec3 result = eulerAngles(quat(objRotation * objRotation2));
#pragma endregion retry

        SetEulerAngles(UpdateRollWithEuler(GetEulerAngles(), mRollSpeed * mInput.mLeft ? -1 : 1));

        float aileronMaxRot = 60;

        lAileronEulerAngles.y = Lerp(lAileronEulerAngles.y, mInput.mLeft ? -aileronMaxRot : aileronMaxRot, 0.1);
        rAileronEulerAngles.y = Lerp(rAileronEulerAngles.y, mInput.mLeft ? aileronMaxRot : -aileronMaxRot, 0.1);

        float rudderMaxRot = 30;

        rudderEulerAngles.z = Lerp(rudderEulerAngles.z, mInput.mLeft ? -rudderMaxRot : rudderMaxRot, 0.06);
    }
    else {
        lAileronEulerAngles.y = Lerp(lAileronEulerAngles.y, 0, 0.03);
        rAileronEulerAngles.y = Lerp(rAileronEulerAngles.y, 0, 0.03);
        rudderEulerAngles.z = Lerp(rudderEulerAngles.z, 0, 0.03);
    }
    
    mLeftAileron->SetEulerAngles(glAxis(lAileronEulerAngles));
    mRightAileron->SetEulerAngles(glAxis(rAileronEulerAngles));

    mLeftRudder->SetEulerAngles(glAxis(rudderEulerAngles));
    mRightRudder->SetEulerAngles(glAxis(rudderEulerAngles));

//    printf("pos: %s rot: %s \n", 
//        toString(mBody->GetPosition()).c_str(), 
//        toString(mBody->GetPosition()).c_str());
}

void Airplane::Input(AirplaneInput input) {
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

mat4 Airplane::GetModelMatrix() const{
    return mBody->GetModelMatrix();
}

Airplane::Airplane() {
    //Tween t = { &mSpeed, mSpeed, -11, 0.1 };
    //mTweens.push_back(t);

    // wip
    int x = 0;
    int y = 2048;
    int z = 2048;

    mBody = read_obj_file("models/osprey-r/Body.obj");
    mBody->LoadTexture2DSimpleBmp("models/osprey-r-tex.bmp", x, y, z);
    mBody->SetPivot(blenderAxis(vec3(0, 0, 0)));

    mElevator = read_obj_file("models/osprey-r/Elevator.obj");
    mElevator->LoadTexture2DSimpleBmp("models/osprey-r-tex.bmp", x, y, z);
    mElevator->SetPivot(blenderAxis(vec3(-9.45f, 0.2f, 2.62f)));

    mLeftAileron = read_obj_file("models/osprey-r/Left_Aileron.obj");
    mLeftAileron->LoadTexture2DSimpleBmp("models/osprey-r-tex.bmp", x, y, z);
    mLeftAileron->SetPivot(blenderAxis(vec3(-0.2f, 5, 3.27f)));

    mLeftEngine = read_obj_file("models/osprey-r/Left_Engine.obj");
    mLeftEngine->LoadTexture2DSimpleBmp("models/osprey-r-tex.bmp", x, y, z);
    mLeftEngine->SetPivot(blenderAxis(vec3(1.05f, 6.5f, 3.3f)));

    mLeftFan = read_obj_file("models/osprey-r/Left_Fan.obj");
    mLeftFan->LoadTexture2DSimpleBmp("models/osprey-r-tex.bmp", x, y, z);
    mLeftFan->SetPivot(blenderAxis(vec3(0.64f, 7.043f, 5.37f)));

    mLeftFlap = read_obj_file("models/osprey-r/Left_Flap.obj");
    mLeftFlap->LoadTexture2DSimpleBmp("models/osprey-r-tex.bmp", x, y, z);
    mLeftFlap->SetPivot(blenderAxis(vec3(-0.38f, 2.57f, 3.76f)));

    mLeftRudder = read_obj_file("models/osprey-r/Left_Rudder.obj");
    mLeftRudder->LoadTexture2DSimpleBmp("models/osprey-r-tex.bmp", x, y, z);
    mLeftRudder->SetPivot(blenderAxis(vec3(-9.6f, 2.51f, 3.21f)));

    mRightAileron = read_obj_file("models/osprey-r/Right_Aileron.obj");
    mRightAileron->LoadTexture2DSimpleBmp("models/osprey-r-tex.bmp", x, y, z);
    mRightAileron->SetPivot(blenderAxis(vec3(-0.2f, -5, 3.27f)));

    mRightEngine = read_obj_file("models/osprey-r/Right_Engine.obj");
    mRightEngine->LoadTexture2DSimpleBmp("models/osprey-r-tex.bmp", x, y, z);
    mRightEngine->SetPivot(blenderAxis(vec3(vec3(1.05f, -6.5f, 3.3f))));

    mRightFan = read_obj_file("models/osprey-r/Right_Fan.obj");
    mRightFan->LoadTexture2DSimpleBmp("models/osprey-r-tex.bmp", x, y, z);
    mRightFan->SetPivot(blenderAxis(vec3(0.64f, -7.043f, 5.37f)));

    mRightFlap = read_obj_file("models/osprey-r/Right_Flap.obj");
    mRightFlap->LoadTexture2DSimpleBmp("models/osprey-r-tex.bmp", x, y, z);
    mRightFlap->SetPivot(blenderAxis(vec3(-0.38f, -2.57f, 3.76f)));

    mRightRudder = read_obj_file("models/osprey-r/Right_Rudder.obj");
    mRightRudder->LoadTexture2DSimpleBmp("models/osprey-r-tex.bmp", x, y, z);
    mRightRudder->SetPivot(blenderAxis(vec3(-9.6f, -2.51f, 3.21f)));

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