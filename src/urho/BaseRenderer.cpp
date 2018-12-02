#include "BaseRenderer.h"
#include "helpers/Events.h"
#include "../UrhoDevice.h"


#include <Urho3D/Urho3DAll.h>



BaseRenderer3D::BaseRenderer3D() :
    Urho3D::Object(UrhoDevice::gUrhoContext)
{
    _scene = new Scene(UrhoDevice::gUrhoContext);
    _scene->CreateComponent<Octree>();

    ResourceCache* cache = GetSubsystem<ResourceCache>();

    Node* node = _scene->CreateChild("Box");
    node->SetPosition(Vector3(0,0,0));
    StaticModel* staticModel = node->CreateComponent<StaticModel>();
    staticModel->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
    staticModel->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));

    Node* lightNode = _scene->CreateChild("DirectionalLight");
    lightNode->SetDirection(Vector3(0.6f, -1.0f, 0.8f));
    auto* light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);

    Node* nodeCamera = _scene->CreateChild("Camera");
    nodeCamera->SetPosition(Vector3(0,1,-5));
    _cameraController = nodeCamera->CreateComponent<CameraController>();
    Camera* camera = nodeCamera->CreateComponent<Camera>();
    camera->SetFarClip(2000);

    Renderer* renderer = GetSubsystem<Renderer>();
    _viewport = new Viewport(context_, _scene, camera);
    renderer->SetViewport(0, _viewport);

    _grid.CreateGrid(_scene);
    _grid.ShowGrid();





    /*// Create a new empty scene.
    _scene = Scene::create();

    // set fps camera
    Vector3 cameraPosition(0, 1, 5);
    _fpCamera.initialize(1.0, 10000.0f);
    _fpCamera.setPosition(cameraPosition);
    _scene->addNode(_fpCamera.getRootNode());
    _scene->setActiveCamera(_fpCamera.getCamera());
    _scene->getActiveCamera()->setAspectRatio(4/3);

    // load 3d grid
    loadGrid(_scene);

    // Add listeners for events
    EventManager::getInstance()->addListener(GP_EVENT_LISTENER(this, GPRenderer3D::onMouseEvent), MouseEvent::ID());
    EventManager::getInstance()->addListener(GP_EVENT_LISTENER(this, GPRenderer3D::onKeyEvent), KeyEvent::ID());*/
}


void BaseRenderer3D::resize(int width, int height)
{
    _viewport->SetRect(IntRect(0, 0, width, height));

   /* if(_scene)
    {
        Camera* camera = _scene->getActiveCamera();
        if(camera)
        {
            float ratio = (float)width / (float)height;
            camera->setAspectRatio(ratio);
        }
    }*/
}

void BaseRenderer3D::update(float elapsedTime)
{
    //_scene->Update(elapsedTime);

   /* if(Game::getInstance()->isMouseCaptured())
        _fpCamera.updateCamera(elapsedTime);*/
}

void BaseRenderer3D::onMouseEvent()
{
    /*auto mouseEvent = std::dynamic_pointer_cast<MouseEvent>(eventData);
    if(!mouseEvent)
        return;

    if(Game::getInstance()->isMouseCaptured())
        _fpCamera.mouseEvent(mouseEvent->event, mouseEvent->mousePos.x, mouseEvent->mousePos.y, 0);
    else
    {
        // force to clear camera movement if key press event was not sent before releasing mouse capture
        _fpCamera.clearMoveFlag();
    }*/
}

void BaseRenderer3D::onKeyEvent()
{
   /* auto keyEvent = std::dynamic_pointer_cast<KeyEvent>(eventData);
    if(!keyEvent)
        return;

    if(Game::getInstance()->isMouseCaptured())
        _fpCamera.keyEvent(keyEvent->event, keyEvent->key);*/
}
