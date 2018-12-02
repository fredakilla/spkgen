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


//------------------------------------------------------------------------------------------------------------------------------
// obstacle modifier node
//------------------------------------------------------------------------------------------------------------------------------

NodeSparkModifierObstacle::NodeSparkModifierObstacle()
{
    IN_PORT(ENC_ZONE, "zone");
    OUT_PORT(ENC_MODIFIER, "modifier");

    createBaseModifierParams("Obstacle");
    PARAM_FLOAT("BouncingRatio", eF32_MIN, eF32_MAX, 1.0f);
    PARAM_FLOAT("Friction", eF32_MIN, eF32_MAX, 1.0f);
    PARAM_ENUM("ZoneTest", "INSIDE|OUTSIDE|INTERSECT|ENTER|LEAVE|ALWAYS", 2);
}

void NodeSparkModifierObstacle::process()
{
    // get parameters
    float bouncingRatio = getParameter("BouncingRatio")->getValueAsFloat();
    float friction = getParameter("Friction")->getValueAsFloat();
    SPK::ZoneTest zoneTest = TABLE_ZONE_TEST[ getParameter("ZoneTest")->getValueAsEnum() ];

    // create new modifier
    SPK::Ref<SPK::Obstacle> obstacleModifier = SPK::Obstacle::create();
    obstacleModifier->setBouncingRatio(bouncingRatio);
    obstacleModifier->setFriction(friction);

    // get input zone
    std::shared_ptr<NodeDataSparkZone> inZone = getInput<NodeDataSparkZone>(0);
    if(inZone && inZone->_result.get())
    {
        obstacleModifier->setZone(inZone->_result);
        obstacleModifier->setZoneTest(zoneTest);
    }

    // set base modifier parameters
    setBaseModifierParams(obstacleModifier);

    // set new modifier as node result
    setResult(obstacleModifier);

    // trigger nodes connections
    dataUpdated(0);
}


//------------------------------------------------------------------------------------------------------------------------------
// pointmass modifier node
//------------------------------------------------------------------------------------------------------------------------------

NodeSparkModifierPointMass::NodeSparkModifierPointMass()
{
    OUT_PORT(ENC_MODIFIER, "modifier");

    createBaseModifierParams("PointMass");
    PARAM_FXYZ("Position", eF32_MIN, eF32_MAX, 0.0f, 0.0f, 0.0f);
    PARAM_FLOAT("Mass", eF32_MIN, eF32_MAX, 1.0f);
    PARAM_FLOAT("Offset", eF32_MIN, eF32_MAX, 0.01f);
}

void NodeSparkModifierPointMass::process()
{
    // get parameters
    eFXYZ position = getParameter("Position")->getValueAsFXYZ();
    float mass = getParameter("Mass")->getValueAsFloat();
    float offset = getParameter("Offset")->getValueAsFloat();

    // create new modifier
    SPK::Ref<SPK::PointMass> pointMassModifier = SPK::PointMass::create();
    pointMassModifier->setPosition(ToSpkVector3D(position));
    pointMassModifier->setMass(mass);
    pointMassModifier->setOffset(offset);

    // set base modifier parameters
    setBaseModifierParams(pointMassModifier);

    // set new modifier as node result
    setResult(pointMassModifier);

    // trigger nodes connections
    dataUpdated(0);
}


//------------------------------------------------------------------------------------------------------------------------------
// random force modifier node
//------------------------------------------------------------------------------------------------------------------------------

NodeSparkModifierRandomForce::NodeSparkModifierRandomForce()
{
    OUT_PORT(ENC_MODIFIER, "modifier");

    createBaseModifierParams(Name());
    PARAM_FXYZ("MinVector", eF32_MIN, eF32_MAX, 0.0f, 0.0f, 0.0f);
    PARAM_FXYZ("MaxVector", eF32_MIN, eF32_MAX, 0.0f, 1.0f, 0.0f);
    PARAM_FXY("Periods", 0.0f, eF32_MAX, 1.0f, 1.0f);
}

void NodeSparkModifierRandomForce::process()
{
    // get parameters
    eFXYZ minVector = getParameter("MinVector")->getValueAsFXYZ();
    eFXYZ maxVector = getParameter("MaxVector")->getValueAsFXYZ();
    eFXY periods = getParameter("Periods")->getValueAsFXY();

    // create new modifier
    SPK::Ref<SPK::RandomForce> randForceModifier = SPK::RandomForce::create();
    randForceModifier->setVectors(ToSpkVector3D(minVector), ToSpkVector3D(maxVector));
    randForceModifier->setPeriods(periods.x, periods.y);

    // set base modifier parameters
    setBaseModifierParams(randForceModifier);

    // set new modifier as node result
    setResult(randForceModifier);

    // trigger nodes connections
    dataUpdated(0);
}


//------------------------------------------------------------------------------------------------------------------------------
// rotator modifier node
//------------------------------------------------------------------------------------------------------------------------------

NodeSparkModifierRotator::NodeSparkModifierRotator()
{
    OUT_PORT(ENC_MODIFIER, "modifier");

    createBaseModifierParams(Name());
}

void NodeSparkModifierRotator::process()
{
    // create new modifier
    SPK::Ref<SPK::Rotator> rotatorModifier = SPK::Rotator::create();

    // set base modifier parameters
    setBaseModifierParams(rotatorModifier);

    // set new modifier as node result
    setResult(rotatorModifier);

    // trigger nodes connections
    dataUpdated(0);
}


//------------------------------------------------------------------------------------------------------------------------------
// vortex modifier node
//------------------------------------------------------------------------------------------------------------------------------

NodeSparkModifierVortex::NodeSparkModifierVortex()
{
    OUT_PORT(ENC_MODIFIER, "modifier");

    createBaseModifierParams(Name());
    PARAM_FXYZ("Position", eF32_MIN, eF32_MAX, 0.0f, 0.0f, 0.0f);
    PARAM_FXYZ("Direction", eF32_MIN, eF32_MAX, 0.0f, 1.0f, 0.0f);
    PARAM_FLOAT("RotationSpeed", eF32_MIN, eF32_MAX, 1.0f);
    PARAM_FLOAT("AttractionSpeed", eF32_MIN, eF32_MAX, 0.0f);
    PARAM_BOOL("AngularSpeed", eFALSE);
    PARAM_BOOL("LinearSpeed", eFALSE);
    PARAM_FLOAT("EyeRadius", eF32_MIN, eF32_MAX, 0.0f);
    PARAM_BOOL("KillingParticles", eFALSE);
}

void NodeSparkModifierVortex::process()
{
    // get parameters
    eFXYZ position = getParameter("Position")->getValueAsFXYZ();
    eFXYZ direction = getParameter("Direction")->getValueAsFXYZ();
    float rotationSpeed = getParameter("RotationSpeed")->getValueAsFloat();
    float attractionSpeed = getParameter("AttractionSpeed")->getValueAsFloat();
    bool angularSpeed = getParameter("AngularSpeed")->getValueAsBool();
    bool linearSpeed = getParameter("LinearSpeed")->getValueAsBool();
    float eyeRadius = getParameter("EyeRadius")->getValueAsFloat();
    bool killingParticles = getParameter("KillingParticles")->getValueAsBool();

    // create new modifier
    SPK::Ref<SPK::Vortex> vortexModifier = SPK::Vortex::create();
    vortexModifier->setPosition( ToSpkVector3D(position) );
    vortexModifier->setDirection( ToSpkVector3D(direction) );
    vortexModifier->setRotationSpeed(rotationSpeed, angularSpeed);
    vortexModifier->setAttractionSpeed(attractionSpeed, linearSpeed);
    vortexModifier->setEyeRadius(eyeRadius);
    vortexModifier->enableParticleKilling(killingParticles);

    // set base modifier parameters
    setBaseModifierParams(vortexModifier);

    // set new modifier as node result
    setResult(vortexModifier);

    // trigger nodes connections
    dataUpdated(0);
}


//------------------------------------------------------------------------------------------------------------------------------
// emitterAttacher modifier node
//------------------------------------------------------------------------------------------------------------------------------

NodeSparkModifierEmitterAttacher::NodeSparkModifierEmitterAttacher()
{
    IN_PORT(ENC_EMITTER, "emitter");
    IN_PORT(ENC_GROUP, "group");
    OUT_PORT(ENC_MODIFIER, "modifier");

    createBaseModifierParams(Name());
    PARAM_BOOL("Orientation", eFALSE);
    PARAM_BOOL("Rotation", eFALSE);
}

void NodeSparkModifierEmitterAttacher::process()
{
    // get parameters
    bool orientate = getParameter("Orientation")->getValueAsBool();
    bool rotate = getParameter("Rotation")->getValueAsBool();


    // get input emitter
    std::shared_ptr<NodeDataSparkEmitterList> inEmitter = getInput<NodeDataSparkEmitterList>(0);
    if(!inEmitter)
    {
        setValidationState(NodeValidationState::Warning, "Missing emitter input");
        return;
    }

    // get input group
    std::shared_ptr<NodeDataSparkGroupList> inGroup = getInput<NodeDataSparkGroupList>(1);
    if(!inGroup)
    {
        setValidationState(NodeValidationState::Warning, "Missing group input");
        return;
    }

    // check for errors

    if(inEmitter->_result.size() > 1)
    {
        setValidationState(NodeValidationState::Error, "Wainting single emitter not a list");
        return;
    }
    else if(inEmitter->_result.size() == 0)
    {
        setValidationState(NodeValidationState::Warning, "Emitter is empty");
        return;
    }

    if(inGroup->_result.size() > 1)
    {
        setValidationState(NodeValidationState::Error, "Wainting single group not a list");
        return;
    }
    else if(inGroup->_result.size() == 0)
    {
        setValidationState(NodeValidationState::Warning, "Group is empty");
        return;
    }

    setValidationState(NodeValidationState::Valid);


    // create new modifier
    SPK::Ref<SPK::EmitterAttacher> emitterAttacherModifier = SPK::EmitterAttacher::create();
    emitterAttacherModifier->setEmitter(inEmitter->_result.at(0));
    emitterAttacherModifier->setTargetGroup(inGroup->_result.at(0));
    emitterAttacherModifier->enableEmitterOrientation(orientate, rotate);

    // set base modifier parameters
    setBaseModifierParams(emitterAttacherModifier);

    // set new modifier as node result
    setResult(emitterAttacherModifier);

    // trigger nodes connections
    dataUpdated(0);
}


//------------------------------------------------------------------------------------------------------------------------------
// linearForce modifier node
//------------------------------------------------------------------------------------------------------------------------------

static const SPK::Param TABLE_PARAM[] = { SPK::PARAM_SCALE, SPK::PARAM_MASS, SPK::PARAM_ANGLE, SPK::PARAM_TEXTURE_INDEX, SPK::PARAM_ROTATION_SPEED };
static const SPK::Factor TABLE_FACTOR[] = { SPK::FACTOR_CONSTANT, SPK::FACTOR_LINEAR, SPK::FACTOR_QUADRATIC, SPK::FACTOR_CUBIC };

NodeSparkModifierLinearForce::NodeSparkModifierLinearForce()
{
    IN_PORT(ENC_ZONE, "zone");
    OUT_PORT(ENC_MODIFIER, "modifier");

    createBaseModifierParams(Name());
    PARAM_ENUM("ZoneTest", "INSIDE|OUTSIDE|INTERSECT|ENTER|LEAVE|ALWAYS", 0);
    PARAM_FXYZ("Value", eF32_MIN, eF32_MAX, 0.0f, 0.0f, 0.0f);
    PARAM_BOOL("RelativeValue", eFALSE);
    PARAM_BOOL("SquaredSpeed", eFALSE);
    PARAM_BOOL("UseParam", eFALSE);
    PARAM_ENUM("Parameter", "Scale|Mass|Angle|TextureIndex|RotationSpeed", 0);
    PARAM_ENUM("Factor", "Constant|Linear|Quadratic|Cubic", 0);
    PARAM_FLOAT("Coefficient", eF32_MIN, eF32_MAX, 1.0f);
}

void NodeSparkModifierLinearForce::process()
{
    // get parameters
    SPK::ZoneTest zoneTest = TABLE_ZONE_TEST[ getParameter("ZoneTest")->getValueAsEnum() ];
    SPK::Vector3D value = ToSpkVector3D( getParameter("Value")->getValueAsFXYZ() );
    bool relative = getParameter("RelativeValue")->getValueAsBool();
    bool squaredSpeed = getParameter("SquaredSpeed")->getValueAsBool();
    bool useParam = getParameter("UseParam")->getValueAsBool();
    SPK::Param parameter =  TABLE_PARAM[ getParameter("Parameter")->getValueAsEnum() ];
    SPK::Factor factorType = TABLE_FACTOR[ getParameter("Factor")->getValueAsEnum() ];
    float coefficient = getParameter("Coefficient")->getValueAsFloat();

    // get input zone
    std::shared_ptr<NodeDataSparkZone> inZone = getInput<NodeDataSparkZone>(0);
    if(!inZone || !inZone->_result.get())
    {
        setValidationState(NodeValidationState::Warning, "Missing zone input");
        return;
    }
    else
    {
        setValidationState(NodeValidationState::Valid);
    }

    // create new modifier
    SPK::Ref<SPK::LinearForce> linearForceModifier = SPK::LinearForce::create();
    linearForceModifier->setValue(value);
    linearForceModifier->setRelative(relative, squaredSpeed);
    linearForceModifier->setCoef(coefficient);
    if(useParam)
        linearForceModifier->setParam(parameter, factorType);
    else
        linearForceModifier->setNoParam();
    linearForceModifier->setZone(inZone->_result);
    linearForceModifier->setZoneTest(zoneTest);

    // set base modifier parameters
    setBaseModifierParams(linearForceModifier);

    // set new modifier as node result
    setResult(linearForceModifier);

    // trigger nodes connections
    dataUpdated(0);
}
