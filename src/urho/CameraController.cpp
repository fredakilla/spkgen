#include "CameraController.h"
#include "../UrhoDevice.h"

#include <Urho3D/Urho3DAll.h>

using namespace Urho3D;

CameraController::CameraController(Context* context) :
    LogicComponent(context),
    _yaw(0.0f),
    _pitch(0.0f),
    _isMoving(false),
    _mode(Orbit),
    _zDistance(5.0f),
    _target(Vector3::ZERO)

{
    SetUpdateEventMask(USE_UPDATE);

    SubscribeToEvent(E_MOUSEMOVE, URHO3D_HANDLER(CameraController, MouseMoveEvent));
    SubscribeToEvent(E_MOUSEBUTTONDOWN, URHO3D_HANDLER(CameraController, MouseButtonPressed));
    SubscribeToEvent(E_MOUSEBUTTONUP, URHO3D_HANDLER(CameraController, MouseButtonReleased));
}

void CameraController::setMode(CameraMode mode)
{
    _mode = mode;
}

void CameraController::setTarget(Urho3D::Vector3 target)
{
    _target = target;
}

void CameraController::Update(float timeStep)
{
    if (_isMoving)
    {
        Input* input = GetSubsystem<Input>();

        // Movement speed as world units per second
        const float moveSpeed = 20.0f * timeStep;

        if (_mode == CameraMode::FirstPerson)
        {
            // Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
            // Use the Translate() function (default local space) to move relative to the node's orientation.
            if (input->GetKeyDown(KEY_W))
                node_->Translate(Vector3::FORWARD * moveSpeed);
            if (input->GetKeyDown(KEY_S))
                node_->Translate(Vector3::BACK * moveSpeed);
            if (input->GetKeyDown(KEY_A))
                node_->Translate(Vector3::LEFT * moveSpeed);
            if (input->GetKeyDown(KEY_D))
                node_->Translate(Vector3::RIGHT * moveSpeed);

            // move camera
            node_->SetRotation(Quaternion(_pitch, _yaw, 0.0f));
        }
        else if(_mode == CameraMode::Orbit)
        {
            if (input->GetKeyDown(KEY_W))
                _zDistance -= moveSpeed;
            if (input->GetKeyDown(KEY_S))
                _zDistance += moveSpeed;

            if(_zDistance < 0.0f)
                _zDistance = 0.0f;

            Vector3 targetPosition = _target;
            Quaternion q = Quaternion(-_pitch, -_yaw, 0.0f);
            Vector3 cameraOffset(0.0f, 0.0f, _zDistance);
            Vector3 cameraPosition = targetPosition - (q * cameraOffset);
            node_->SetPosition(cameraPosition);  // Set new camera position and lookat values
            node_->LookAt(targetPosition);
        }
    }
}

// map MouseButton Qt -> SDL
int LookUpButtonMap[] =
{
    0x00000000,        // NoButton         = 0x00000000,
    SDL_BUTTON_LMASK,  // LeftButton       = 0x00000001,
    SDL_BUTTON_RMASK,  // RightButton      = 0x00000002,
    SDL_BUTTON_MMASK,  // MidButton        = 0x00000004,
    SDL_BUTTON_MMASK,  // MiddleButton     = MidButton,
};

void CameraController::MouseButtonPressed(StringHash eventType, VariantMap& eventData)
{
    using namespace MouseButtonDown;
    int buttons = eventData[P_BUTTONS].GetInt();

    int b = LookUpButtonMap[buttons];

    if(b & MOUSEB_RIGHT)
        _isMoving = true;
}

void CameraController::MouseButtonReleased(StringHash eventType, VariantMap& eventData)
{
    _isMoving = false;
}

void CameraController::MouseMoveEvent(StringHash eventType, VariantMap& eventData)
{
    using namespace MouseMove;
    int px = eventData[P_X].GetInt();
    int py = eventData[P_Y].GetInt();
    int dx = eventData[P_DX].GetInt();
    int dy = eventData[P_DY].GetInt();
    int buttons = eventData[P_BUTTONS].GetInt();
    int qualifiers = eventData[P_QUALIFIERS].GetInt();

    if(_isMoving)
    {
        // Mouse sensitivity as degrees per pixel
        const float MOUSE_SENSITIVITY = 0.5f;

        // Use this frame's mouse motion to adjust camera node yaw and pitch.
        // Clamp the pitch between -90 and 90 degrees
        IntVector2 mouseMove(dx, dy);

        _yaw += MOUSE_SENSITIVITY * mouseMove.x_;
        _pitch += MOUSE_SENSITIVITY * mouseMove.y_;
        _pitch = Clamp(_pitch, -90.0f, 90.0f);
    }
}
