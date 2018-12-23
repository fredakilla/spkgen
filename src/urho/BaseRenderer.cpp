#include "BaseRenderer.h"
#include "../UrhoDevice.h"

#include <Urho3D/Urho3DAll.h>


/// Custom logic component for moving the animated model and rotating at area edges.
class Mover : public LogicComponent
{
    URHO3D_OBJECT(Mover, LogicComponent);

public:
    explicit Mover(Context* context);
    void SetParameters(float moveSpeed, float rotationSpeed, const BoundingBox& bounds);
    void Update(float timeStep) override;
    float GetMoveSpeed() const { return moveSpeed_; }
    float GetRotationSpeed() const { return rotationSpeed_; }
    const BoundingBox& GetBounds() const { return bounds_; }

private:
    float moveSpeed_;
    float rotationSpeed_;
    BoundingBox bounds_;
};


Mover::Mover(Context* context) :
    LogicComponent(context),
    moveSpeed_(0.0f),
    rotationSpeed_(0.0f)
{
    // Only the scene update event is needed: unsubscribe from the rest for optimization
    SetUpdateEventMask(USE_UPDATE);
}

void Mover::SetParameters(float moveSpeed, float rotationSpeed, const BoundingBox& bounds)
{
    moveSpeed_ = moveSpeed;
    rotationSpeed_ = rotationSpeed;
    bounds_ = bounds;
}

void Mover::Update(float timeStep)
{
    node_->Translate(Vector3::FORWARD * moveSpeed_ * timeStep);

    // If in risk of going outside the plane, rotate the model right
    Vector3 pos = node_->GetPosition();
    if (pos.x_ < bounds_.min_.x_ || pos.x_ > bounds_.max_.x_ || pos.z_ < bounds_.min_.z_ || pos.z_ > bounds_.max_.z_)
        node_->Yaw(rotationSpeed_ * timeStep);

    // Get the model's first (only) animation state and advance its time. Note the convenience accessor to other components
    // in the same scene node
    auto* model = node_->GetComponent<AnimatedModel>(true);
    if (model->GetNumAnimationStates())
    {
        AnimationState* state = model->GetAnimationStates()[0];
        state->AddTime(timeStep);
    }
}



/**
 * Renderer to preview spark effects in a 3D scene
 */
BaseRenderer3D::BaseRenderer3D() :
    Urho3D::Object(UrhoDevice::gUrhoContext)
  , _showDebugShapes(true)
{
    context_->RegisterFactory<Mover>();

    // create scene
    _scene = new Scene(UrhoDevice::gUrhoContext);
    _scene->CreateComponent<Octree>();

    // create 3d grid
    _grid.CreateGrid(_scene);

    // create scene 1
    createScene1();

    // create camera
    Node* nodeCamera = _scene->CreateChild("Camera");
    nodeCamera->SetPosition(Vector3(0,1,-5));
    _cameraController = nodeCamera->CreateComponent<CameraController>();
    Camera* camera = nodeCamera->CreateComponent<Camera>();
    camera->SetNearClip(0.1f);
    camera->SetFarClip(1000.f);

    // create viewport
    Renderer* renderer = GetSubsystem<Renderer>();
    _viewport = new Viewport(context_, _scene, camera);
    renderer->SetViewport(0, _viewport);

    // create debug renderer (for debug shapes)
    _debugRenderer = new DebugRenderer(context_);
    _scene->AddComponent(_debugRenderer, 0, LOCAL);

    // show grid scene by default
    setScene(0);
}

void BaseRenderer3D::createScene1()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    // Root scene node
    _rootNodeScene1 = _scene->CreateChild("_rootNodeScene1");

    // Create scene node & StaticModel component for showing a static plane
    Node* planeNode = _rootNodeScene1->CreateChild("Plane");
    planeNode->SetScale(Vector3(100.0f, 1.0f, 100.0f));
    auto* planeObject = planeNode->CreateComponent<StaticModel>();
    planeObject->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
    planeObject->SetMaterial(cache->GetResource<Material>("Materials/StoneTiled.xml"));

    // Create a Zone component for ambient lighting & fog control
    Node* zoneNode = _rootNodeScene1->CreateChild("Zone");
    auto* zone = zoneNode->CreateComponent<Zone>();
    zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
    zone->SetAmbientColor(Color(0.7f, 0.7f, 0.7f));
    zone->SetFogColor(Color(0.5f, 0.6f, 0.85f));
    zone->SetFogStart(100.0f);
    zone->SetFogEnd(300.0f);

    // Create a directional light to the world. Enable cascaded shadows on it
    Node* lightNode = _rootNodeScene1->CreateChild("DirectionalLight");
    lightNode->SetDirection(Vector3(0.6f, -1.0f, 0.8f));
    auto* light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetCastShadows(true);
    light->SetColor(Color(0.5f, 0.5f, 0.5f));
    light->SetShadowBias(BiasParameters(0.00025f, 0.5f));
    light->SetShadowCascade(CascadeParameters(10.0f, 50.0f, 200.0f, 0.0f, 0.8f));

    // Create animated models
    const unsigned NUM_MODELS = 30;
    const float MODEL_MOVE_SPEED = 2.0f;
    const float MODEL_ROTATE_SPEED = 100.0f;
    const BoundingBox bounds(Vector3(-20.0f, 0.0f, -20.0f), Vector3(20.0f, 0.0f, 20.0f));

    for (unsigned i = 0; i < NUM_MODELS; ++i)
    {
        Node* modelNode = _rootNodeScene1->CreateChild("Jill");
        modelNode->SetPosition(Vector3(Random(40.0f) - 20.0f, 0.0f, Random(40.0f) - 20.0f));
        modelNode->SetRotation(Quaternion(0.0f, Random(360.0f), 0.0f));

        auto* modelObject = modelNode->CreateComponent<AnimatedModel>();
        modelObject->SetModel(cache->GetResource<Model>("Models/Kachujin/Kachujin.mdl"));
        modelObject->SetMaterial(cache->GetResource<Material>("Models/Kachujin/Materials/Kachujin.xml"));
        modelObject->SetCastShadows(true);

        auto* walkAnimation = cache->GetResource<Animation>("Models/Kachujin/Kachujin_Walk.ani");

        AnimationState* state = modelObject->AddAnimationState(walkAnimation);
        if (state)
        {
            // Enable full blending weight and looping
            state->SetWeight(1.0f);
            state->SetLooped(true);
            state->SetTime(Random(walkAnimation->GetLength()));
        }

        auto* mover = modelNode->CreateComponent<Mover>();
        mover->SetParameters(MODEL_MOVE_SPEED, MODEL_ROTATE_SPEED, bounds);
    }

    // create some static models
    const unsigned NUM_OBJECTS = 200;
    for (unsigned i = 0; i < NUM_OBJECTS; ++i)
    {
        Node* mushroomNode = _rootNodeScene1->CreateChild("Mushroom");
        mushroomNode->SetPosition(Vector3(Random(90.0f) - 45.0f, 0.0f, Random(90.0f) - 45.0f));
        mushroomNode->SetRotation(Quaternion(0.0f, Random(360.0f), 0.0f));
        mushroomNode->SetScale(1.0f);
        auto* mushroomObject = mushroomNode->CreateComponent<StaticModel>();
        mushroomObject->SetModel(cache->GetResource<Model>("Models/Mushroom.mdl"));
        mushroomObject->SetMaterial(cache->GetResource<Material>("Materials/Mushroom.xml"));
    }
}

void BaseRenderer3D::resize(int width, int height)
{
    if(_viewport)
        _viewport->SetRect(IntRect(0, 0, width, height));
}

void BaseRenderer3D::setScene(int index)
{
    switch (index)
    {
    case 0:
        _grid.ShowGrid(_scene);
        _rootNodeScene1->SetDeepEnabled(false);
        break;

    case 1:
        _grid.HideGrid();
        _rootNodeScene1->SetDeepEnabled(true);
        break;
    }
}

void BaseRenderer3D::showDebugShapes(bool enabled)
{
    _showDebugShapes = enabled;
}

