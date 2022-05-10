#include "Camera.h"
#include "../Math/Quaternion.h"

namespace Joestar {
    Camera::~Camera() {

    }
	void Camera::ProcessHID(HID* hid, float deltaTime)
    {
        float velocity = mSpeed * deltaTime;
        bool dirty = false;
        if (hid->CheckKeyboardInput(KEY_W))
        {
            mPosition += mFront * velocity;
            dirty = true;
        }
        if (hid->CheckKeyboardInput(KEY_S))
        {
            mPosition -= mFront * velocity;
            dirty = true;
        }
        if (hid->CheckKeyboardInput(KEY_A))
        {
            mPosition -= mRight * velocity;
            dirty = true;
        }
        if (hid->CheckKeyboardInput(KEY_D))
        {
            mPosition += mRight * velocity;
            dirty = true;
        }

        float* offset = hid->GetMouseOffset();
        if (offset[0] != 0.f || offset[1] != 0.f)
        {
            F32 offsetX = offset[0] * mSensitivity;
            //LOG("offsetY: %.2f\n", offset[1]);
            F32 offsetY = offset[1] * mSensitivity;
            //Quaternionf front = FromToQuaternion(Vector3f::Front, mFront);
            //Vector3f right = Cross(Vector3f::Up, mFront);
            //Vector3f up = Cross(mFront, right);
            //Quaternionf rotx = Quaternionf::identity();// AxisAngleToQuaternion(Vector3f::Up, offsetX);
            Quaternionf rotx = AxisAngleToQuaternion(Vector3f::Up, offsetX);
            Quaternionf roty = AxisAngleToQuaternion(mRight, offsetY);
            mFront = Normalize(RotateVectorByQuat(rotx * roty, mFront));
            mUp = RotateVectorByQuat(rotx * roty, mUp);
            mRight = RotateVectorByQuat(rotx, mRight);

            Vector3f tmp = Normalize(Cross(mFront, mUp));
            mUp = Normalize(Cross(tmp, mFront));
            mRight = Normalize(Cross(mUp, mFront));
            dirty = true;
        }

        float* scroll = hid->GetMouseScroll();
        if (scroll[1] != 0.f)
        {
            mZoom -= scroll[1];
            if (mZoom < 1.0f)
               mZoom = 1.0f;
            if (mZoom > 85.0f)
               mZoom = 85.0f;
            mProjection.SetPerspective(mZoom, 800.0f / 600.0f, 0.1f, 100.0f);
        }

        if (dirty)
        {
            UpdateCameraVectors();
        }
	}

    void Camera::UpdateCameraVectors() {
        // calculate the new Front vector
        //Vector3f front;
        //front.x = cos(Deg2Rad(mYaw)) * cos(Deg2Rad(mPitch));
        //front.y = sin(Deg2Rad(mPitch));
        //front.z = sin(Deg2Rad(mYaw)) * cos(Deg2Rad(mPitch));
        //mFront = Normalize(front);
        // also re-calculate the Right and Up vector
        //mRight = Normalize(Cross(mFront, mWorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        mUp = Normalize(Cross(mFront, mRight));
        mView.LookAt(mPosition, mFront, mRight, mUp);
        LOG("Up: %.2f, %.2f, %.2f\n", mUp.x, mUp.y, mUp.z);
        LOG("Right: %.2f, %.2f, %.2f\n", mRight.x, mRight.y, mRight.z);
        LOG("Front: %.2f, %.2f, %.2f\n", mFront.x, mFront.y, mFront.z);
    }
}