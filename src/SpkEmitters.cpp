#include "SpkEmitters.h"
#include "core/Base.h"

//--------------------------------------------------------------------------------------------
// base spark emitter
//--------------------------------------------------------------------------------------------

void NodeSparkEmitterBase::createBaseEmitterParams(eString name, bool shared)
{
    // create base SPKObject parameters
    createBaseObjectParams(name,shared);

    // create base Emitter parameters
    PARAM_BOOL("Active", eTRUE);
    PARAM_IXY("Tank", -1, 10000, -1, -1);
    PARAM_FLOAT("Flow", -1, eF32_MAX, 25);
    PARAM_FXY("Force", eF32_MIN, eF32_MAX, 1.0f, 1.0f);
}

void NodeSparkEmitterBase::setBaseEmitterParams(SPK::Ref<SPK::Emitter> emitter)
{
    if(emitter)
    {
        // get parameters
        bool active = getParameter("Active")->baseValue.get<bool>();
        eIXY tank = getParameter("Tank")->baseValue.get<eIXY>();
        eF32 flow = getParameter("Flow")->baseValue.get<eF32>();
        eFXY force = getParameter("Force")->baseValue.get<eFXY>();

        // check parameters entries
        float minTank = tank.x;
        float maxTank = tank.y;
        if (minTank < 0 || maxTank < 0) minTank = maxTank = -1;
        if (minTank > maxTank)
        {
            // min tank is greater than max tank. Swap values.
            std::swap(minTank,maxTank);
        }

        // set Emitter parameters
        emitter->setActive(active);
        emitter->setTank(minTank, maxTank);
        emitter->setFlow(flow);
        emitter->setForce(force.x, force.y);

        // set base SPKObject parameters
        setBaseObjectParams(emitter);
    }
}

void NodeSparkEmitterBase::setResult(SPK::Ref<SPK::Emitter> emitter)
{
    if(_emitters.size() > 1)
        _emitters.back().reset();
    _emitters.clear();
    _emitters.push_back(emitter);
}

//--------------------------------------------------------------------------------------------
// emitters list node
//--------------------------------------------------------------------------------------------

NodeSparkEmitterList::NodeSparkEmitterList()
{
    IN_PORT(ENC_EMITTER, "emitter");
    IN_PORT(ENC_EMITTER, "emitter");
    IN_PORT(ENC_EMITTER, "emitter");
    IN_PORT(ENC_EMITTER, "emitter");
    IN_PORT(ENC_EMITTER, "emitter");
    OUT_PORT(ENC_EMITTER, "emitters");
}

void NodeSparkEmitterList::process()
{
    _emitters.clear();
    for(size_t i=0; i<_inputs.size(); i++)
    {
        std::shared_ptr<NodeDataSparkEmitterList> in = getInput<NodeDataSparkEmitterList>(i);
        if(in.get())
        {
            for(size_t j=0; j<in->_result.size(); j++)
                _emitters.push_back(in->_result[j]);
        }
    }

    dataUpdated(0);
}

//--------------------------------------------------------------------------------------------
// static emitter node
//--------------------------------------------------------------------------------------------

NodeSparkEmitterStatic::NodeSparkEmitterStatic()
{
    IN_PORT(ENC_ZONE, "zone");
    OUT_PORT(ENC_EMITTER, "emitter");

    createBaseEmitterParams("StaticEmit");
}

void NodeSparkEmitterStatic::process()
{
    // create new emitter
    SPK::Ref<SPK::StaticEmitter> staticEmitter = SPK::StaticEmitter::create();

    // set base emitter parameters
    setBaseEmitterParams(staticEmitter);

    // set zone if exists
    std::shared_ptr<NodeDataSparkZone> inZone = getInput<NodeDataSparkZone>(0);
    if(inZone && inZone->_result.get())
    {
        staticEmitter->setZone(inZone->_result);
    }

    // set new emitter as node result
    setResult(staticEmitter);

    // trigger nodes connections
    dataUpdated(0);
}

//--------------------------------------------------------------------------------------------
// spheric emitter node
//--------------------------------------------------------------------------------------------

NodeSparkEmitterSpheric::NodeSparkEmitterSpheric()
{
    IN_PORT(ENC_ZONE, "zone");
    OUT_PORT(ENC_EMITTER, "emitter");

    createBaseEmitterParams("SphericEmit");
    PARAM_FXYZ("Direction", eF32_MIN, eF32_MAX, 0.0f, 1.0f, 0.0f);
    PARAM_FXY("Angles", 0, 2, 0.0f, 0.0f);
}

void NodeSparkEmitterSpheric::process()
{
    // get parameters
    eFXYZ direction = getParameter("Direction")->getValueAsFXYZ();
    eFXY angles = getParameter("Angles")->getValueAsFXY();

    // create new emitter
    SPK::Ref<SPK::SphericEmitter> sphericEmitter = SPK::SphericEmitter::create();
    sphericEmitter->setDirection( SPK::Vector3D(direction.x, direction.y, direction.z) );
    sphericEmitter->setAngles(angles.x * MATH_PI, angles.y * MATH_PI);

    // set base emitter parameters
    setBaseEmitterParams(sphericEmitter);

    // set zone if exists
    std::shared_ptr<NodeDataSparkZone> inZone = getInput<NodeDataSparkZone>(0);
    if(inZone && inZone->_result.get())
    {
        sphericEmitter->setZone(inZone->_result);
    }

    // set new emitter as node result
    setResult(sphericEmitter);

    // trigger nodes connections
    dataUpdated(0);
}

//--------------------------------------------------------------------------------------------
// random emitter node
//--------------------------------------------------------------------------------------------

NodeSparkEmitterRandom::NodeSparkEmitterRandom()
{
    IN_PORT(ENC_ZONE, "zone");
    OUT_PORT(ENC_EMITTER, "emitter");

    createBaseEmitterParams("RandomEmit");
}

void NodeSparkEmitterRandom::process()
{
    // create new emitter
    SPK::Ref<SPK::RandomEmitter> randomEmitter = SPK::RandomEmitter::create();

    // set base emitter parameters
    setBaseEmitterParams(randomEmitter);

    // set zone if exists
    std::shared_ptr<NodeDataSparkZone> inZone = getInput<NodeDataSparkZone>(0);
    if(inZone && inZone->_result.get())
    {
        randomEmitter->setZone(inZone->_result);
    }

    // set new emitter as node result
    setResult(randomEmitter);

    // trigger nodes connections
    dataUpdated(0);
}

//--------------------------------------------------------------------------------------------
// straight emitter node
//--------------------------------------------------------------------------------------------

NodeSparkEmitterStraight::NodeSparkEmitterStraight()
{
    IN_PORT(ENC_ZONE, "zone");
    OUT_PORT(ENC_EMITTER, "emitter");

    createBaseEmitterParams("RandomEmit");
    PARAM_FXYZ("Direction", eF32_MIN, eF32_MAX, 0.0f, 1.0f, 0.0f);
}

void NodeSparkEmitterStraight::process()
{
    // get parameters
    eFXYZ direction = getParameter("Direction")->getValueAsFXYZ();

    // create new emitter
    SPK::Ref<SPK::StraightEmitter> straightEmitter = SPK::StraightEmitter::create();
    straightEmitter->setDirection(ToSpkVector3D(direction));

    // set base emitter parameters
    setBaseEmitterParams(straightEmitter);

    // set zone if exists
    std::shared_ptr<NodeDataSparkZone> inZone = getInput<NodeDataSparkZone>(0);
    if(inZone && inZone->_result.get())
    {
        straightEmitter->setZone(inZone->_result);
    }

    // set new emitter as node result
    setResult(straightEmitter);

    // trigger nodes connections
    dataUpdated(0);
}


//--------------------------------------------------------------------------------------------
// normal emitter node
//--------------------------------------------------------------------------------------------

NodeSparkEmitterNormal::NodeSparkEmitterNormal()
{
    IN_PORT(ENC_ZONE, "zone");
    OUT_PORT(ENC_EMITTER, "emitter");

    createBaseEmitterParams("NormalEmit");
    PARAM_BOOL("InvertedNormals", eFALSE);
}

void NodeSparkEmitterNormal::process()
{
    // get parameters
    bool inverted = getParameter("InvertedNormals")->getValueAsBool();

    // create new emitter
    SPK::Ref<SPK::NormalEmitter> normalEmitter = SPK::NormalEmitter::create();
    normalEmitter->setInverted(inverted);

    // set base emitter parameters
    setBaseEmitterParams(normalEmitter);

    // set zone if exists
    std::shared_ptr<NodeDataSparkZone> inZone = getInput<NodeDataSparkZone>(0);
    if(inZone && inZone->_result.get())
    {
        normalEmitter->setZone(inZone->_result);
    }

    // set new emitter as node result
    setResult(normalEmitter);

    // trigger nodes connections
    dataUpdated(0);
}

