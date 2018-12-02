#include "BaseRenderer.h"
#include "helpers/Events.h"
#include "../UrhoDevice.h"

#include <Urho3D/Urho3DAll.h>

BaseRenderer3D::BaseRenderer3D() :
    Urho3D::Object(UrhoDevice::gUrhoContext)
{
    _scene = new Scene(UrhoDevice::gUrhoContext);
    _scene->CreateComponent<Octree>();

    Node* nodeCamera = _scene->CreateChild("Camera");
    nodeCamera->SetPosition(Vector3(0,1,-5));
    _cameraController = nodeCamera->CreateComponent<CameraController>();
    Camera* camera = nodeCamera->CreateComponent<Camera>();
    camera->SetNearClip(0.1f);
    camera->SetFarClip(1000.f);

    Renderer* renderer = GetSubsystem<Renderer>();
    _viewport = new Viewport(context_, _scene, camera);
    renderer->SetViewport(0, _viewport);


    _debugRenderer = new DebugRenderer(context_);
    _scene->AddComponent(_debugRenderer, 0, LOCAL);


    _grid.CreateGrid(_scene);
    _grid.ShowGrid();
}


void BaseRenderer3D::resize(int width, int height)
{
    if(_viewport)
        _viewport->SetRect(IntRect(0, 0, width, height));
}

void BaseRenderer3D::update(float elapsedTime)
{
}


