#ifndef BASEPRENDERER_H
#define BASEPRENDERER_H

#include "GridGeometry.h"
#include "CameraController.h"

#include <Urho3D/Core/Object.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Viewport.h>

/**
 * Interface class for sub-renderers
 * A subrenderer manage it's own scene.
 */
class ISubRenderer
{
public:
    virtual ~ISubRenderer() {}
    virtual void resize(int width, int height) = 0;
};



/**
 * Base subrenderer for 3D view that have
 * a scene, a controler camera and 3d grid.
 */
class BaseRenderer3D : public ISubRenderer, public Urho3D::Object
{
    URHO3D_OBJECT(BaseRenderer3D, Urho3D::Object)

public:
    BaseRenderer3D();
    void resize(int width, int height) override;
    Urho3D::Scene* getScene() { return _scene; }

protected:
    Urho3D::SharedPtr<Urho3D::Scene> _scene;
    Urho3D::SharedPtr<Urho3D::DebugRenderer> _debugRenderer;
    Urho3D::SharedPtr<Urho3D::Viewport> _viewport;
    CameraController* _cameraController;
    GridGeometry _grid;
};


#endif // BASEPRENDERER_H
