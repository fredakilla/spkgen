#ifndef URHODEVICE_H
#define URHODEVICE_H

#include <Spark/SPARK.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Math/StringHash.h>

class UrhoDevice : public Urho3D::Object
{
    URHO3D_OBJECT(UrhoDevice, Urho3D::Object)

public:
    static UrhoDevice* getInstance();
    static void createContext();
    static void freeContext();
    static Urho3D::Context* gUrhoContext;

    void createRenderWindow(void* hwnd);
    void runFrame();
    void stop();    
    void setCurentParticleSystem(SPK::Ref<SPK::System> sparkSystem);
    void onResize(int width, int height);

private:

    UrhoDevice();
    ~UrhoDevice();

    void initialize();
    void finalize();
    void update(float elapsedTime);
    void render(float elapsedTime);

    void keyEvent();
    bool mouseEvent();

    void handleMouseButtonDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void handleMouseButtonUp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void handleMouseMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void handleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

   // bool drawScene(gplay::Node* node);
   // bool updateEmitters(gplay::Node* node, float elapsedTime);

private:
    //Platform* _platform;
    //Scene* _scene;
    //bool _isShowDebug;  // show debug shapes in effects

    Urho3D::Engine* _engine;    // Urho3D engine
};


#endif // URHODEVICE_H
