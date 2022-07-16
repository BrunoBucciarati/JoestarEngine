#pragma once

#include "../Math/Vector3.h"
#include "../Math/Frustum.h"
#include "../Math/Matrix4x4.h"
#include "../IO/HID.h"
#include "../IO/Log.h"
#include "../Component/Component.h"
#include "../Base/GameObject.h"
#include "../Misc/GlobalConfig.h"

namespace Joestar {

    // An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
    class Camera : public Component
    {
        REGISTER_OBJECT(Camera, Component)
    public:
        // constructor with vectors
        Camera(EngineContext* ctx, GameObject* go) : Super(ctx, go)
        {
            mNearClip = 0.1f;
            mFarClip = 1000.0f;
            UpdateCameraVectors();
            int width = GetSubsystem<GlobalConfig>()->GetConfig<int>(CONFIG_WINDOW_WIDTH);
            int height = GetSubsystem<GlobalConfig>()->GetConfig<int>(CONFIG_WINDOW_HEIGHT);
            mAspect = float(width) / height;
            mProjection.SetPerspective(mZoom, mAspect, mNearClip, mFarClip);
        }

        // returns the view matrix calculated using Euler Angles and the LookAt Matrix
        Matrix4x4f GetViewMatrix()
        {
            return mView;
        }

        Matrix4x4f GetProjectionMatrix()
        {
            return mProjection;
        }

        Vector3f GetPosition()
        {
            return mGameObject->GetPosition();
        }

        void SetOrthographic(float orthographicSize)
        {
            mProjection.SetOrtho(-orthographicSize * mAspect, orthographicSize * mAspect, -orthographicSize, orthographicSize, mNearClip, mFarClip);
        }

        void SetPerspective()
        {
            mProjection.SetPerspective(mFov, mAspect, mNearClip, mFarClip);
        }

        void ProcessHID(HID* hid, float dt);

        void SetPosition(const Vector3f& pos)
        {
            mGameObject->SetPosition(pos);
        }
        void SetFront(const Vector3f& dir)
        {
            mFront = dir;
        }
        void SetWorldRotation(const Quaternionf& quat);
        Frustum GetFrustum();
    private:
        // calculates the front vector from the Camera's (updated) Euler Angles
        void UpdateCameraVectors();
        // camera Attributes
        Vector3f mFront{ Normalize(Vector3f(0.0, 0.3, 1.0)) };
        Vector3f mUp{ Normalize(Vector3f(0.0, 1.0, -0.3)) };
        Vector3f mRight{ Vector3f::Right};
        Vector3f mWorldUp{Vector3f::Up};
        Matrix4x4f mView;
        Matrix4x4f mProjection;

        float mSpeed{3000.f};
        float mSensitivity{0.01f};
        float mZoom{60.f};
        float mAspect{45.f};
        float mNearClip{.1f};
        float mFarClip{100.f};
        float mFov{ 45.f };
    };
}