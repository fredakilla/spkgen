#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Math/StringHash.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Math/Vector3.h>

class CameraController : public Urho3D::LogicComponent
{
    URHO3D_OBJECT(CameraController, Urho3D::LogicComponent)

public:

    enum CameraMode
    {
        FirstPerson,
        Orbit
    };

    //! Constructor
    CameraController(Urho3D::Context* context);
    //! Set camera mode (fps or orbit)
    void setMode(CameraMode mode);
    //! set look at target (only orbit mode)
    void setTarget(Urho3D::Vector3 target);

private:

    void Update(float timeStep) override;
    void MouseButtonPressed(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void MouseButtonReleased(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void MouseMoveEvent(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void MouseWheelEvent(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

    float _yaw;
    float _pitch;
    bool _isMoving;
    CameraMode _mode;
    float _zDistance;
    Urho3D::Vector3 _target;
};

#endif // CAMERACONTROLLER_H
