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
                mFront = RotateVectorByQuat(roty, mFront);
                mFront = Normalize(RotateVectorByQuat(rotx, mFront));
                mUp = RotateVectorByQuat(roty, mUp);
                mUp = RotateVectorByQuat(rotx, mUp);
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
        mView.LookAt(GetPosition(), mFront, mRight, mUp);
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

    Frustum Camera::GetFrustum()
    {
        Frustum ret;
        Matrix4x4f vp = mView * mProjection;
        ret.DefineFromViewProj(vp);
        return ret;
    }
}