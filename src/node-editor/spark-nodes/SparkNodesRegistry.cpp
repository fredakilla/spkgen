#include "spark-nodes.h"

std::shared_ptr<DataModelRegistry> registerSparkNodesDataModels()
{
    auto ret = std::make_shared<DataModelRegistry>();

    ret->registerModel<NodeSparkTest>("test");

    ret->registerModel<NodeSparkZonePoint>("zones");
    ret->registerModel<NodeSparkZonePlane>("zones");
    ret->registerModel<NodeSparkZoneSphere>("zones");
    ret->registerModel<NodeSparkZoneBox>("zones");
    ret->registerModel<NodeSparkZoneCylinder>("zones");
    ret->registerModel<NodeSparkZoneRing>("zones");

    ret->registerModel<NodeSparkGroup>("system");
    ret->registerModel<NodeSparkSystem>("system");
    ret->registerModel<NodeSparkGroupList>("system");
    ret->registerModel<NodeSparkQuadRenderer>("renderer");

    ret->registerModel<NodeSparkEmitterList>("emitters");
    ret->registerModel<NodeSparkEmitterStatic>("emitters");
    ret->registerModel<NodeSparkEmitterSpheric>("emitters");
    ret->registerModel<NodeSparkEmitterRandom>("emitters");
    ret->registerModel<NodeSparkEmitterStraight>("emitters");
    ret->registerModel<NodeSparkEmitterNormal>("emitters");

    ret->registerModel<NodeSparkModifierList>("modifiers");
    ret->registerModel<NodeSparkModifierGravity>("modifiers");
    ret->registerModel<NodeSparkModifierFriction>("modifiers");
    ret->registerModel<NodeSparkModifierCollider>("modifiers");
    ret->registerModel<NodeSparkModifierDestroyer>("modifiers");
    ret->registerModel<NodeSparkModifierObstacle>("modifiers");
    ret->registerModel<NodeSparkModifierPointMass>("modifiers");
    ret->registerModel<NodeSparkModifierRandomForce>("modifiers");
    ret->registerModel<NodeSparkModifierRotator>("modifiers");
    ret->registerModel<NodeSparkModifierVortex>("modifiers");
    ret->registerModel<NodeSparkModifierEmitterAttacher>("modifiers");
    ret->registerModel<NodeSparkModifierLinearForce>("modifiers");

    ret->registerModel<NodePath>("interpolators");
    ret->registerModel<NodeSparkInterpolator_ColorInitializerDefault>("interpolators");
    ret->registerModel<NodeSparkInterpolator_ColorInitializerRandom>("interpolators");
    ret->registerModel<NodeSparkInterpolator_ColorInterpolatorSimple>("interpolators");
    ret->registerModel<NodeSparkInterpolator_ColorInterpolatorRandom>("interpolators");
    ret->registerModel<NodeSparkInterpolator_ColorInterpolatorGraph>("interpolators");

    ret->registerModel<NodeSparkInterpolatorParamList>("interpolators");
    ret->registerModel<NodeSparkInterpolator_ParamInitializer>("interpolators");
    ret->registerModel<NodeSparkInterpolator_ParamInitializerRandom>("interpolators");
    ret->registerModel<NodeSparkInterpolator_ParamInterpolatorSimple>("interpolators");
    ret->registerModel<NodeSparkInterpolator_ParamInterpolatorRandom>("interpolators");

    return ret;
}
