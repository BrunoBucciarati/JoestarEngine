#include "Camera.h"
#include "../Math/Quaternion.h"
#include "../Base/GameObject.h"

namespace Joestar {
    Camera::~Camera()
    {}
	void Camera::ProcessHID(HID* hid, float deltaTime)
    {
        float velocity = mSpeed * deltaTime;
        bool dirty = false;
        Vector3f pos = GetPosition();
        if (hid->CheckKeyboardInput(KEY_W))
        {
            pos += mFront * velocity;
            dirty = true;
        }
        if (hid->CheckKeyboardInput(KEY_S))
        {
            pos -= mFront * velocity;
            dirty = true;
        }
        if (hid->CheckKeyboardInput(KEY_A))
        {
            pos -= mRight * velocity;
            dirty = true;
        }
        if (hid->CheckKeyboardInput(KEY_D))
        {
            pos += mRight * velocity;
            dirty = true;
        }

        if (hid->IsMouseRightDown())
        {
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
                Quaternionf roty = AxisAngleToQuaternion(mRight, -offsetY);
                mFront = Normalize(RotateVectorByQuat(rotx * roty, mFront));
                mUp = RotateVectorByQuat(rotx * roty, mUp);
                mRight = RotateVectorByQuat(rotx, mRight);

                Vector3f tmp = Normalize(Cross(mFront, mUp));
                mUp = Normalize(Cross(tmp, mFront));
                mRight = Normalize(Cross(mUp, mFront));
                dirty = true;
            }
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
            SetPosition(pos);
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
        //mUp = Normalize(Cross(mFront, mRight));
        mView.LookAt(GetPosition(), mFront, mRight, mUp);
        Vector4f testP = mView.MultiplyVector4(Vector4f(0.F, 0.F, 0.F, 1.F));
        Vector4f testP1 = mProjection.MultiplyVector4(testP);
        LOG("Up: %.2f, %.2f, %.2f\n", mUp.x, mUp.y, mUp.z);
        LOG("Right: %.2f, %.2f, %.2f\n", mRight.x, mRight.y, mRight.z);
        LOG("Front: %.2f, %.2f, %.2f\n", mFront.x, mFront.y, mFront.z);
    }

    void Camera::SetWorldRotation(const Quaternionf& quat)
    {
        mGameObject->SetRotation(quat);
        Matrix3x3f mat = quat.RotationMatrix();
        mRight = mat.GetColumn(0);
        mUp = mat.GetColumn(1);
        mFront = mat.GetColumn(2);
        UpdateCameraVectors();
    }
}