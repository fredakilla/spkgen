#include "SpkInterpolators.h"

SPK::Param TABLE_PARAM[] =
{
    SPK::PARAM_SCALE,
    SPK::PARAM_MASS,
    SPK::PARAM_ANGLE,
    SPK::PARAM_TEXTURE_INDEX,
    SPK::PARAM_ROTATION_SPEED
};

//------------------------------------------------------------------------------------------------------------------------------
// base spark interpolator class
//------------------------------------------------------------------------------------------------------------------------------

void NodeSparkColorInterpolatorBase::setResult(const SPK::Ref<SPK::Interpolator<SPK::Color>> &colorInterpolator)
{
    _colorInterpolator.reset();
    _colorInterpolator = colorInterpolator;
}


//------------------------------------------------------------------------------------------------------------------------------
// base spark interpolator class
//------------------------------------------------------------------------------------------------------------------------------

void NodeSparkParamInterpolatorBase::setResult(const ParamFloatInterpolator &interpolator)
{
    if(_paramInterpolators.size() > 1)
        _paramInterpolators.back().interpolatorFloat.reset();
    _paramInterpolators.clear();
    _paramInterpolators.push_back(interpolator);
}


//------------------------------------------------------------------------------------------------------------------------------
// default color initializer node
//------------------------------------------------------------------------------------------------------------------------------

NodeSparkInterpolator_ColorInitializerDefault::NodeSparkInterpolator_ColorInitializerDefault()
{
    OUT_PORT(ENC_COLORINTERPOLATOR, "colors");

    createBaseObjectParams(Name());
    PARAM_RGBA("DefaultValue", 255, 0, 0, 255);
}

void NodeSparkInterpolator_ColorInitializerDefault::process()
{
     // get parameters
    SPK::Color color = ToSpkColor( getParameter("DefaultValue")->getValueAsColor() );

    // create new interpolator
    SPK::Ref<SPK::DefaultInitializer<SPK::Color>> defaultInitialiser;
    defaultInitialiser = SPK::DefaultInitializer<SPK::Color>::create(color);

    // set base spark object parameters
    setBaseObjectParams(defaultInitialiser);

    // set new interpolator as node result
    setResult(defaultInitialiser);

    // trigger nodes connections
    dataUpdated(0);
}


//------------------------------------------------------------------------------------------------------------------------------
// random color initializer node
//------------------------------------------------------------------------------------------------------------------------------

NodeSparkInterpolator_ColorInitializerRandom::NodeSparkInterpolator_ColorInitializerRandom()
{
    OUT_PORT(ENC_COLORINTERPOLATOR, "colors");

    createBaseObjectParams(Name());
    PARAM_RGBA("MinColor", 255, 0, 0, 255);
    PARAM_RGBA("MaxColor", 255, 255, 0, 255);
}

void NodeSparkInterpolator_ColorInitializerRandom::process()
{
     // get parameters
    SPK::Color minColor = ToSpkColor( getParameter("MinColor")->getValueAsColor() );
    SPK::Color maxColor = ToSpkColor( getParameter("MaxColor")->getValueAsColor() );

    // create new interpolator
    SPK::Ref<SPK::RandomInitializer<SPK::Color>> randomInitializer;
    randomInitializer = SPK::RandomInitializer<SPK::Color>::create(minColor, maxColor);

    // set base spark object parameters
    setBaseObjectParams(randomInitializer);

    // set new interpolator as node result
    setResult(randomInitializer);

    // trigger nodes connections
    dataUpdated(0);
}


//------------------------------------------------------------------------------------------------------------------------------
// simple color interpolator node
//------------------------------------------------------------------------------------------------------------------------------

NodeSparkInterpolator_ColorInterpolatorSimple::NodeSparkInterpolator_ColorInterpolatorSimple()
{
    OUT_PORT(ENC_COLORINTERPOLATOR, "colors");

    createBaseObjectParams(Name());
    PARAM_RGBA("BirthColor", 255, 0, 0, 255);
    PARAM_RGBA("DeathColor", 255, 255, 0, 255);
}

void NodeSparkInterpolator_ColorInterpolatorSimple::process()
{
     // get parameters
    SPK::Color birthColor = ToSpkColor( getParameter("BirthColor")->getValueAsColor() );
    SPK::Color deathColor = ToSpkColor( getParameter("DeathColor")->getValueAsColor() );

    // create new interpolator
    SPK::Ref<SPK::SimpleInterpolator<SPK::Color>> simpleInterpolator;
    simpleInterpolator =  SPK::SimpleInterpolator<SPK::Color>::create(birthColor, deathColor);

    // set base spark object parameters
    setBaseObjectParams(simpleInterpolator);

    // set new interpolator as node result
    setResult(simpleInterpolator);

    // trigger nodes connections
    dataUpdated(0);
}


//------------------------------------------------------------------------------------------------------------------------------
// random color interpolator node
//------------------------------------------------------------------------------------------------------------------------------

NodeSparkInterpolator_ColorInterpolatorRandom::NodeSparkInterpolator_ColorInterpolatorRandom()
{
    OUT_PORT(ENC_COLORINTERPOLATOR, "colors");

    createBaseObjectParams(Name());
    PARAM_RGBA("MinBirthColor", 255, 0, 0, 255);
    PARAM_RGBA("MaxBirthColor", 255, 255, 0, 255);
    PARAM_RGBA("MinDeathColor", 0, 255, 255, 255);
    PARAM_RGBA("MaxDeathColor", 0, 255, 0, 255);
}

void NodeSparkInterpolator_ColorInterpolatorRandom::process()
{
     // get parameters
    SPK::Color minBirthColor = ToSpkColor( getParameter("MinBirthColor")->getValueAsColor() );
    SPK::Color maxBirthColor = ToSpkColor( getParameter("MaxBirthColor")->getValueAsColor() );
    SPK::Color minDeathColor = ToSpkColor( getParameter("MinDeathColor")->getValueAsColor() );
    SPK::Color maxDeathColor = ToSpkColor( getParameter("MaxDeathColor")->getValueAsColor() );

    // create new interpolator
    SPK::Ref<SPK::RandomInterpolator<SPK::Color>> randomInterpolator;
    randomInterpolator =  SPK::RandomInterpolator<SPK::Color>::create(minBirthColor, maxBirthColor, minDeathColor, maxDeathColor);

    // set base spark object parameters
    setBaseObjectParams(randomInterpolator);

    // set new interpolator as node result
    setResult(randomInterpolator);

    // trigger nodes connections
    dataUpdated(0);
}





//------------------------------------------------------------------------------------------------------------------------------
// param interpolator list node
//------------------------------------------------------------------------------------------------------------------------------

NodeSparkInterpolatorParamList::NodeSparkInterpolatorParamList()
{
    IN_PORT(ENC_PARAMINTERPOLATOR, "param");
    IN_PORT(ENC_PARAMINTERPOLATOR, "param");
    IN_PORT(ENC_PARAMINTERPOLATOR, "param");
    IN_PORT(ENC_PARAMINTERPOLATOR, "param");
    IN_PORT(ENC_PARAMINTERPOLATOR, "param");
    OUT_PORT(ENC_PARAMINTERPOLATOR, "params");
}

void NodeSparkInterpolatorParamList::process()
{
    _paramInterpolators.clear();
    for(size_t i=0; i<_inputs.size(); i++)
    {
        std::shared_ptr<NodeDataSparkParamInterpolatorList> in = getInput<NodeDataSparkParamInterpolatorList>(i);
        if(in.get())
        {
            for(size_t j=0; j<in->_result.size(); j++)
                _paramInterpolators.push_back(in->_result[j]);
        }
    }

    dataUpdated(0);
}


//------------------------------------------------------------------------------------------------------------------------------
// default param initializer node
//------------------------------------------------------------------------------------------------------------------------------

NodeSparkInterpolator_ParamInitializer::NodeSparkInterpolator_ParamInitializer()
{
    OUT_PORT(ENC_PARAMINTERPOLATOR, "param");

    createBaseObjectParams(Name());
    PARAM_FLOAT("Value", eF32_MIN, eF32_MAX, 1.0f);
    PARAM_ENUM("Param", "Scale|Mass|Angle|TextureIndex|RotationSpeed", 1);
}

void NodeSparkInterpolator_ParamInitializer::process()
{
     // get parameters
    float value = getParameter("Value")->getValueAsFloat();
    SPK::Param param = TABLE_PARAM[ getParameter("Param")->getValueAsEnum() ];

    // create new interpolator
    SPK::Ref<SPK::DefaultInitializer<float>> defaultInitialiser;
    defaultInitialiser = SPK::DefaultInitializer<float>::create(value);

    // set base spark object parameters
    setBaseObjectParams(defaultInitialiser);

    // set new interpolator as node result
    ParamFloatInterpolator interpolator;
    interpolator.interpolatorFloat = defaultInitialiser;
    interpolator.param = param;
    setResult(interpolator);

    // trigger nodes connections
    dataUpdated(0);
}
