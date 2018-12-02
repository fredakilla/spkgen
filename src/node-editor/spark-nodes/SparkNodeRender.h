#ifndef SPKRENDERER_H
#define SPKRENDERER_H

#include "../../urho/BaseRenderer.h"
#include <Spark/SPARK.h>

#include <Urho3D/Scene/Node.h>

class SparkNodeRender : public BaseRenderer3D
{
public:
    SparkNodeRender();

    void setCurentParticleSystem(SPK::Ref<SPK::System> sparkSystem);

    void update(float elapsedTime) override;
    void render(float elapsedTime) override;

    bool updateEmitters(Urho3D::Node* node, float elapsedTime);
    bool drawScene(Urho3D::Node* node);

private:

    void handleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

    //! show debug shapes in effects
    bool _isShowDebug;
    Urho3D::SharedPtr<Urho3D::Node> _sparkNode;
};

#endif // SPKRENDERER_H
