#pragma once

#include "../Math/Vector3.h"
#include "../Math/Matrix4x4.h"
#include "../IO/HID.h"
#include "../IO/Log.h"
#include "../Core/Object.h"
#include "../Misc/GlobalConfig.h"

namespace Joestar {

    // An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
    class Camera : public Object
    {
        REGISTER_OBJECT(Camera, Object)
    public:
        // constructor with vectors
        Camera(EngineContext* ctx) : Super(ctx) 
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
        inline Matrix4x4f* GetViewMatrix()
        {
            return &mView;
        }

        inline Matrix4x4f* GetProjectionMatrix()
        {
            return &mProjection;
        }

        Vector3f& GetPosition()
        {
            return mPosition;
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

    private:
        // calculates the front vector from the Camera's (updated) Euler Angles
        void UpdateCameraVectors();
        // camera Attributes
        Vector3f mPosition{ Vector3f(0.0, 1.0, -3.0)};
        Vector3f mFront{ Vector3f::Front };
        Vector3f mUp{ Vector3f::Up };
        Vector3f mRight{ Vector3f::Right};
        Vector3f mWorldUp{Vector3f::Up};
        Matrix4x4f mView;
        Matrix4x4f mProjection;
        // euler Angles
        float mYaw{-90.0f};
        float mPitch{-30.0f};
        // camera options
        float mSpeed{10.f};
        float mSensitivity{1.f};
        float mZoom{60.f};
        float mAspect{45.f};
        float mNearClip{.1f};
        float mFarClip{100.f};
        float mFov{ 45.f };
    };
}