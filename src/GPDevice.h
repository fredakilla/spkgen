#ifndef GPDEVICE_H
#define GPDEVICE_H

#include <gplay-engine.h>
#include <spark/SPARK.h>
#include <QObject>

using namespace gplay;

class GPDevice : public Game
{
public:
    static GPDevice& get();
    void createRenderWindow(void* hwnd);
    void frame();
    void stop();
    void resizeRenderView(int width, int height);

    void setCurentParticleSystem(SPK::Ref<SPK::System> sparkSystem);

private:
    GPDevice();
    ~GPDevice();

    void initialize() override;
    void finalize() override;
    void update(float elapsedTime) override;
    void render(float elapsedTime) override;


    bool drawScene(gplay::Node* node);
    bool updateEmitters(gplay::Node* node, float elapsedTime);




    static GPDevice* _instance;
    gplay::Platform* _platform;
    Scene* _scene;
    bool _isShowDebug;  // show debug shapes in effects
};

#endif // GPDEVICE_H
