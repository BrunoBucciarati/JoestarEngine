#include "Camera.h"

namespace Joestar {
    Camera::~Camera() {

    }
	void Camera::ProcessHID(HID* hid, float deltaTime) {
        float velocity = MovementSpeed * deltaTime;
        bool dirty = false;
        if (hid->CheckKeyboardInput(KEY_W)) {
            Position += Front * velocity;
            dirty = true;
        }
        if (hid->CheckKeyboardInput(KEY_S)) {
            Position -= Front * velocity;
            dirty = true;
        }
        if (hid->CheckKeyboardInput(KEY_A)) {
            Position -= Right * velocity;
            dirty = true;
        }
        if (hid->CheckKeyboardInput(KEY_D)) {
            Position += Right * velocity;
            dirty = true;
        }

        float* offset = hid->GetMouseOffset();
        if (offset[0] != 0.f || offset[1] != 0.f) {
            Yaw += offset[0] * MouseSensitivity;
            //LOG("offsetY: %.2f\n", offset[1]);
            Pitch += offset[1] * MouseSensitivity;
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
            dirty = true;
        }

        float* scroll = hid->GetMouseScroll();
        if (scroll[1] != 0.f) {
            Zoom -= scroll[1];
            if (Zoom < 1.0f)
               Zoom = 1.0f;
            if (Zoom > 85.0f)
               Zoom = 85.0f;
            projection.SetPerspective(Zoom, 800.0f / 600.0f, 0.1f, 100.0f);
        }

        if (dirty) {
            UpdateCameraVectors();
        }
	}

    void Camera::UpdateCameraVectors() {
        // calculate the new Front vector
        Vector3f front;
        front.x = cos(Deg2Rad(Yaw)) * cos(Deg2Rad(Pitch));
        front.y = sin(Deg2Rad(Pitch));
        front.z = sin(Deg2Rad(Yaw)) * cos(Deg2Rad(Pitch));
        Front = Normalize(front);
        LOG("front: %.2f, %.2f, %.2f\n", Front.x, Front.y, Front.z);
        // also re-calculate the Right and Up vector
        Right = Normalize(Cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        LOG("Right: %.2f, %.2f, %.2f\n", Right.x, Right.y, Right.z);
        Up = Normalize(Cross(Right, Front));
        LOG("Up: %.2f, %.2f, %.2f\n", Up.x, Up.y, Up.z);
        view.LookAt(Position, Front, Right, Up);
    }
}