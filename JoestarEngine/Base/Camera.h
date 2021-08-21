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

        // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
        //void ProcessKeyboard(Camera_Movement direction, float deltaTime)
        //{
        //    float velocity = MovementSpeed * deltaTime;
        //    if (direction == FORWARD)
        //        Position += Front * velocity;
        //    if (direction == BACKWARD)
        //        Position -= Front * velocity;
        //    if (direction == LEFT)
        //        Position -= Right * velocity;
        //    if (direction == RIGHT)
        //        Position += Right * velocity;
        //}

        void ProcessHID(HID* hid, float dt);

        // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
        //void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
        //{
        //    xoffset *= MouseSensitivity;
        //    yoffset *= MouseSensitivity;

        //    Yaw += xoffset;
        //    Pitch += yoffset;

        //    // make sure that when pitch is out of bounds, screen doesn't get flipped
        //    if (constrainPitch)
        //    {
        //        if (Pitch > 89.0f)
        //            Pitch = 89.0f;
        //        if (Pitch < -89.0f)
        //            Pitch = -89.0f;
        //    }

        //    // update Front, Right and Up Vectors using the updated Euler angles
        //    UpdateCameraVectors();
        //}

        //// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        //void ProcessMouseScroll(float yoffset)
        //{
        //    Zoom -= (float)yoffset;
        //    if (Zoom < 1.0f)
        //        Zoom = 1.0f;
        //    if (Zoom > 85.0f)
        //        Zoom = 85.0f;
        //}

        void Draw() {

        }

    private:
        // calculates the front vector from the Camera's (updated) Euler Angles
        void UpdateCameraVectors();
    };
}