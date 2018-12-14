#include "UrhoDevice.h"
#include "node-editor/common/BaseNode.h"
#include "node-editor/spark-nodes/SparkNodeRender.h"
#include "urho/BaseRenderer.h"

#include <Urho3D/Urho3DAll.h>
#include <Urhox/Spark/UrhoSpark.h>
#include <Urhox/SystemUI/SystemUI.h>

ISubRenderer* _curentSubRenderer = nullptr;
Urho3D::Context* UrhoDevice::gUrhoContext = nullptr;


UrhoDevice::UrhoDevice() :
    Object(gUrhoContext),
    _engine(nullptr)
{
}

UrhoDevice::~UrhoDevice()
{
}

UrhoDevice* UrhoDevice::getInstance()
{
    static UrhoDevice __urhoDeviceInstance;
    return &__urhoDeviceInstance;
}

void UrhoDevice::createContext()
{
    eASSERT(!gUrhoContext);
    gUrhoContext = new Urho3D::Context();
    eASSERT(gUrhoContext);
}

void UrhoDevice::freeContext()
{
    if (gUrhoContext)
        delete gUrhoContext;
    gUrhoContext = nullptr;
}

void UrhoDevice::createRenderWindow(void* hwnd)
{
    // set engine paramaters
    VariantMap engineParameters;
    engineParameters[EP_VSYNC] = false;
    engineParameters[EP_FULL_SCREEN] = false;
    engineParameters[EP_WINDOW_WIDTH] = 1280;
    engineParameters[EP_WINDOW_HEIGHT] = 720;
    engineParameters[EP_WINDOW_RESIZABLE] = true;
    engineParameters[EP_EXTERNAL_WINDOW] = hwnd;

    // create urho3d engine
    _engine = new Urho3D::Engine(gUrhoContext);
    _engine->Initialize(engineParameters);
    _engine->SetMaxFps(200);

    // register objects
    RegisterSparkLibrary(context_);
    context_->RegisterFactory<CameraController>();

    // SystemUI subsytem need to be registered after engine initialization
    context_->RegisterSubsystem(new SystemUI(context_));
    context_->GetSubsystem<SystemUI>()->Start();

    ui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ui::GetIO().BackendFlags |= ImGuiBackendFlags_HasMouseCursors;

    // create spark renderer
    SparkNodeRender* spkRenderer = new SparkNodeRender();
    _curentSubRenderer = spkRenderer;

    /*SubscribeToEvent(E_MOUSEBUTTONDOWN, URHO3D_HANDLER(UrhoDevice, handleMouseButtonDown));
    SubscribeToEvent(E_MOUSEBUTTONUP, URHO3D_HANDLER(UrhoDevice, handleMouseButtonUp));
    SubscribeToEvent(E_MOUSEMOVE, URHO3D_HANDLER(UrhoDevice, handleMouseMove));
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(UrhoDevice, handleKeyDown));*/
}

void UrhoDevice::runFrame()
{
    _engine->RunFrame();

    /*// begin frame
    Renderer::getInstance().beginFrame();
    QtImGui::newFrame();

    // call gplay frame that will invoke update and render methods.
    frame();

    // end frame
    //ImGui::Render();
    QtImGui::endFrame();
    Renderer::getInstance().endFrame();*/
}


void UrhoDevice::handleMouseButtonDown(StringHash eventType, VariantMap& eventData)
{
}

void UrhoDevice::handleMouseButtonUp(StringHash eventType, VariantMap& eventData)
{
}

void UrhoDevice::handleMouseMove(StringHash eventType, VariantMap& eventData)
{
}

void UrhoDevice::handleKeyDown(StringHash eventType, VariantMap& eventData)
{
}

void UrhoDevice::stop()
{
}

void UrhoDevice::onResize(int width, int height)
{
    // resize urho SDL window
    Graphics* graphics = GetSubsystem<Graphics>();
    SDL_Window * win = (SDL_Window*)graphics->GetWindow();
    SDL_SetWindowSize(win, width, height);

    // resize current renderer
    if (_curentSubRenderer)
        _curentSubRenderer->resize(width, height);
}

void UrhoDevice::setCurentParticleSystem(SPK::Ref<SPK::System> sparkSystem)
{
    SparkNodeRender* _spkRenderer = dynamic_cast<SparkNodeRender*>(_curentSubRenderer);
    if(_spkRenderer)
    {
        _spkRenderer->setCurentParticleSystem(sparkSystem);
    }
}
