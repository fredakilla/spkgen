#include "SpkModifiers.h"


SPK::ZoneTest TABLE_ZONE_TEST[6] = {
    SPK::ZONE_TEST_INSIDE,
    SPK::ZONE_TEST_OUTSIDE,
    SPK::ZONE_TEST_INTERSECT,
    SPK::ZONE_TEST_ENTER,
    SPK::ZONE_TEST_LEAVE,
    SPK::ZONE_TEST_ALWAYS
};


//--------------------------------------------------------------------------------------------
// base spark modifier
//--------------------------------------------------------------------------------------------

void NodeSparkModifierBase::createBaseModifierParams(eString name, bool shared)
{
    // create base SPKObject parameters
    createBaseObjectParams(name,shared);

    // create base modifier parameters
    PARAM_BOOL("LocalToSystem", eTRUE);
}

void NodeSparkModifierBase::setBaseModifierParams(SPK::Ref<SPK::Modifier> modifier)
{
    if(modifier)
    {
        // get parameters
        bool isLocal = getParameter("LocalToSystem")->getValueAsBool();

        // set Modifier base parameters
        modifier->setLocalToSystem(isLocal);

        // set base SPKObject parameters
        setBaseObjectParams(modifier);
    }
}

void NodeSparkModifierBase::setResult(SPK::Ref<SPK::Modifier> modifier)
{
    if(_modifiers.size() > 1)
        _modifiers.back().reset();
    _modifiers.clear();
    _modifiers.push_back(modifier);
}



//--------------------------------------------------------------------------------------------
// modifier list node
//--------------------------------------------------------------------------------------------

NodeSparkModifierList::NodeSparkModifierList()
{
    IN_PORT(ENC_MODIFIER, "modifier");
    IN_PORT(ENC_MODIFIER, "modifier");
    IN_PORT(ENC_MODIFIER, "modifier");
    IN_PORT(ENC_MODIFIER, "modifier");
    IN_PORT(ENC_MODIFIER, "modifier");
    OUT_PORT(ENC_MODIFIER, "modifiers");
}

void NodeSparkModifierList::process()
{
    _modifiers.clear();
    for(size_t i=0; i<_inputs.size(); i++)
    {
        std::shared_ptr<NodeDataSparkModifierList> in = getInput<NodeDataSparkModifierList>(i);
        if(in.get())
        {
            for(size_t j=0; j<in->_result.size(); j++)
                _modifiers.push_back(in->_result[j]);
        }
    }

    dataUpdated(0);
}

//------------------------------------------------------------------------------------------------------------------------------
// gravity modifier node
//------------------------------------------------------------------------------------------------------------------------------

NodeSparkModifierGravity::NodeSparkModifierGravity()
{
    OUT_PORT(ENC_MODIFIER, "modifier");

    createBaseModifierParams("Gravity");
    PARAM_FXYZ("Gravity", eF32_MIN, eF32_MAX, 0.0f, -9.0f, 0.0f);
}

void NodeSparkModifierGravity::process()
{
    // get parameters
    eFXYZ gravity = getParameter("Gravity")->getValueAsFXYZ();

    // create new modifier
    SPK::Ref<SPK::Gravity> gravityModifier = SPK::Gravity::create();
    gravityModifier->setValue(ToSpkVector3D(gravity));

    // set base modifier parameters
    setBaseModifierParams(gravityModifier);

    // set new modifier as node result
    setResult(gravityModifier);

    // trigger nodes connections
    dataUpdated(0);
}


//------------------------------------------------------------------------------------------------------------------------------
// friction modifier node
//------------------------------------------------------------------------------------------------------------------------------

NodeSparkModifierFriction::NodeSparkModifierFriction()
{
    OUT_PORT(ENC_MODIFIER, "modifier");

    createBaseModifierParams("Friction");
    PARAM_FLOAT("Friction", eF32_MIN, eF32_MAX, 0.0f);
}

void NodeSparkModifierFriction::process()
{
    // get parameters
    float friction = getParameter("Friction")->getValueAsFloat();

    // create new modifier
    SPK::Ref<SPK::Friction> frictionModifier = SPK::Friction::create(friction);

    // set base modifier parameters
    setBaseModifierParams(frictionModifier);

    // set new modifier as node result
    setResult(frictionModifier);

    // trigger nodes connections
    dataUpdated(0);
}


//------------------------------------------------------------------------------------------------------------------------------
// collider modifier node
//------------------------------------------------------------------------------------------------------------------------------

NodeSparkModifierCollider::NodeSparkModifierCollider()
{
    OUT_PORT(ENC_MODIFIER, "modifier");

    createBaseModifierParams("Collider");
    PARAM_FLOAT("Elasticity", 0.0f, eF32_MAX, 1.0f);
}

void NodeSparkModifierCollider::process()
{
    // get parameters
    float elasticity = getParameter("Elasticity")->getValueAsFloat();

    // create new modifier
    SPK::Ref<SPK::Collider> colliderModifier = SPK::Collider::create();
    colliderModifier->setElasticity(elasticity);

    // set base modifier parameters
    setBaseModifierParams(colliderModifier);

    // set new modifier as node result
    setResult(colliderModifier);

    // trigger nodes connections
    dataUpdated(0);
}


//------------------------------------------------------------------------------------------------------------------------------
// destroyer modifier node
//------------------------------------------------------------------------------------------------------------------------------

NodeSparkModifierDestroyer::NodeSparkModifierDestroyer()
{
    IN_PORT(ENC_ZONE, "zone");
    OUT_PORT(ENC_MODIFIER, "modifier");

    createBaseModifierParams("Destroyer");
    PARAM_ENUM("ZoneTest", "INSIDE|OUTSIDE|INTERSECT|ENTER|LEAVE|ALWAYS", 2);
}

void NodeSparkModifierDestroyer::process()
{
    // get parameters
    SPK::ZoneTest zoneTest = TABLE_ZONE_TEST[ getParameter("ZoneTest")->getValueAsEnum() ];

    // create new modifier
    SPK::Ref<SPK::Destroyer> destroyerModifier = SPK::Destroyer::create();
    destroyerModifier->setZoneTest(zoneTest);

    // get input zone
    std::shared_ptr<NodeDataSparkZone> inZone = getInput<NodeDataSparkZone>(0);
    if(inZone && inZone->_result.get())
    {
        destroyerModifier->setZone(inZone->_result);
    }



    // set base modifier parameters
    setBaseModifierParams(destroyerModifier);

    // set new modifier as node result
    setResult(destroyerModifier);

    // trigger nodes connections
    dataUpdated(0);
}
