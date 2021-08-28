#pragma once

#include "../Math/Vector3.h"
#include <vector>
#include "../Math/Matrix4x4.h"
#include "../IO/HID.h"
#include "../IO/Log.h"

namespace Joestar {
    // Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
    enum Camera_Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    // Default camera values
    const float YAW = 90.0f;
    const float PITCH = 0.0f;
    const float SPEED = 1.f;
    const float SENSITIVITY = .3f;
    const float ZOOM = 45.0f;


    // An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
    class Camera
    {
    public:
        // camera Attributes
        Vector3f Position;
        Vector3f Front;
        Vector3f Up;
        Vector3f Right;
        Vector3f WorldUp;
        Matrix4x4f view;
        Matrix4x4f projection;
        // euler Angles
        float Yaw;
        float Pitch;
        // camera options
        float MovementSpeed;
        float MouseSensitivity;
        float Zoom;
        // constructor with vectors
        Camera(Vector3f position = Vector3f(0.0, 0.0, -2.0f), Vector3f up = Vector3f(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(Vector3f(0.0f, 0.0f, 1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
        {
            Position = position;
            WorldUp = up;
            Yaw = yaw;
            Pitch = pitch;
            UpdateCameraVectors();
            projection.SetPerspective(Zoom, 800.0f / 600.0f, 0.1f, 100.0f);
        }

        // returns the view matrix calculated using Euler Angles and the LookAt Matrix
        inline Matrix4x4f& GetViewMatrix() {
            return view;
        }

        inline Matrix4x4f& GetProjectionMatrix() {
            return projection;
        }

        void ProcessHID(HID* hid, float dt);

        void Draw() {

        }

    private:
        // calculates the front vector from the Camera's (updated) Euler Angles
        void UpdateCameraVectors();
    };
}