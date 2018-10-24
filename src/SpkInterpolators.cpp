#include "SpkInterpolators.h"

SPK::Param TABLE_PARAM[] =
{
    SPK::PARAM_SCALE,
    SPK::PARAM_MASS,
    SPK::PARAM_ANGLE,
    SPK::PARAM_TEXTURE_INDEX,
    SPK::PARAM_ROTATION_SPEED
};

SPK::InterpolationType TABLE_INTERPOLATION_TYPE[] =
{
    SPK::INTERPOLATOR_LIFETIME,     // interpolation is done from 0 to 100% between 0 and 1 in graph
    SPK::INTERPOLATOR_AGE,          // interpolation is done from 0 to max particle lifetime in seconds (see group)
    SPK::INTERPOLATOR_PARAM,        // interpolation affect selected param
    SPK::INTERPOLATOR_VELOCITY      // interpolation is based on the particle velocity
};


//------------------------------------------------------------------------------------------------------------------------------
// path node
//------------------------------------------------------------------------------------------------------------------------------

NodePath::NodePath() : _path(nullptr)
{
    OUT_PORT(ENC_PATH, "path");

    PARAM_BUTTON("button", "Edit path");
    PARAM_ENUM("PathType", "Constant|Linear|Cubic|Akima|Bessel|Pchip|Quintic", 1);

    _path = new Path(PathType::EPT_LINEAR);
    _path->setLoopMode(PathLoopMode::ELM_LAST);
    _path->addKey(0.0, 0.0);
    _path->addKey(1.0, 0.0);
}

void NodePath::process()
{
    PathType pathType = (PathType)getParameter("PathType")->getValueAsEnum();

    // change path propeties in graph editor.
}


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
// graph color interpolator node
//------------------------------------------------------------------------------------------------------------------------------

NodeSparkInterpolator_ColorInterpolatorGraph::NodeSparkInterpolator_ColorInterpolatorGraph()
{
    IN_PORT(ENC_PATH, "path")
    OUT_PORT(ENC_COLORINTERPOLATOR, "colors");

    createBaseObjectParams(Name());
    PARAM_ENUM("Type", "Lifetime|Age|Param|Velocity", 0);
    PARAM_ENUM("Param", "Scale|Mass|Angle|TextureIndex|RotationSpeed", 1);
}

void NodeSparkInterpolator_ColorInterpolatorGraph::process()
{
    // get parameters
    SPK::InterpolationType interpolateType =  TABLE_INTERPOLATION_TYPE[ getParameter("Type")->getValueAsEnum() ];
    SPK::Param param =  TABLE_PARAM[ getParameter("Param")->getValueAsEnum() ];

    // get path from input

    Path* path = nullptr;
    std::shared_ptr<NodeDataPath> inPath = getInput<NodeDataPath>(0);
    if(inPath.get() && inPath->_result)
    {
        path = inPath->_result;
    }
    else
    {
        // error
        return;
    }

    // create new interpolator
    SPK::Ref<SPK::GraphInterpolator<SPK::Color>> graphInterpolator;
    graphInterpolator = SPK::GraphInterpolator<SPK::Color>::create();
    graphInterpolator->setType(interpolateType, param);


    // fo each keys, evaluate value and add it into the spark interpolator graph
    for (eU32 i=0; i<path->getKeyCount(); ++i)
    {
        // get time key
        eF32 time = path->getKeyByIndex(i).time;

        // evaluate color value at time
        SPK::Color color;
        color.r = path->evaluate(time) * 255.0;
        color.g = 0;
        color.b = 0;
        color.a = 255;

        /*SPK::Color color;
                color.r = path.getSubPath(0).getKeyByTime(time)->val * 255.0f;
                color.g = path.getSubPath(1).getKeyByTime(time)->val * 255.0f;
                color.b = path.getSubPath(2).getKeyByTime(time)->val * 255.0f;
                color.a = path.getSubPath(3).getKeyByTime(time)->val * 255.0f;*/

        // add new graph entry
        graphInterpolator->addEntry(time, color);
    }



    // set base spark object parameters
    setBaseObjectParams(graphInterpolator);

    // set new interpolator as node result
    setResult(graphInterpolator);

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
    ParamFloatInterpolator result;
    result.interpolatorFloat = defaultInitialiser;
    result.param = param;
    setResult(result);

    // trigger nodes connections
    dataUpdated(0);
}


//------------------------------------------------------------------------------------------------------------------------------
// random param initializer node
//------------------------------------------------------------------------------------------------------------------------------

NodeSparkInterpolator_ParamInitializerRandom::NodeSparkInterpolator_ParamInitializerRandom()
{
    OUT_PORT(ENC_PARAMINTERPOLATOR, "param");

    createBaseObjectParams(Name());
    PARAM_FLOAT("MinValue", eF32_MIN, eF32_MAX, 1.0f);
    PARAM_FLOAT("MaxValue", eF32_MIN, eF32_MAX, 1.0f);
    PARAM_ENUM("Param", "Scale|Mass|Angle|TextureIndex|RotationSpeed", 1);
}

void NodeSparkInterpolator_ParamInitializerRandom::process()
{
     // get parameters
    float minValue = getParameter("MinValue")->getValueAsFloat();
    float maxValue = getParameter("MaxValue")->getValueAsFloat();
    SPK::Param param = TABLE_PARAM[ getParameter("Param")->getValueAsEnum() ];

    // create new interpolator
    SPK::Ref<SPK::RandomInitializer<float>> randomInitializer;
    randomInitializer = SPK::RandomInitializer<float>::create(minValue, maxValue);

    // set base spark object parameters
    setBaseObjectParams(randomInitializer);

    // set new interpolator as node result
    ParamFloatInterpolator result;
    result.interpolatorFloat = randomInitializer;
    result.param = param;
    setResult(result);

    // trigger nodes connections
    dataUpdated(0);
}


//------------------------------------------------------------------------------------------------------------------------------
// simple param interpolator node
//------------------------------------------------------------------------------------------------------------------------------

NodeSparkInterpolator_ParamInterpolatorSimple::NodeSparkInterpolator_ParamInterpolatorSimple()
{
    OUT_PORT(ENC_PARAMINTERPOLATOR, "param");

    createBaseObjectParams(Name());
    PARAM_FLOAT("BirthValue", eF32_MIN, eF32_MAX, 1.0f);
    PARAM_FLOAT("DeathValue", eF32_MIN, eF32_MAX, 1.0f);
    PARAM_ENUM("Param", "Scale|Mass|Angle|TextureIndex|RotationSpeed", 1);
}

void NodeSparkInterpolator_ParamInterpolatorSimple::process()
{
     // get parameters
    float birthValue = getParameter("BirthValue")->getValueAsFloat();
    float deathValue = getParameter("DeathValue")->getValueAsFloat();
    SPK::Param param = TABLE_PARAM[ getParameter("Param")->getValueAsEnum() ];

    // create new interpolator
    SPK::Ref<SPK::SimpleInterpolator<float>> simpleInterpolator;
    simpleInterpolator = SPK::SimpleInterpolator<float>::create(birthValue, deathValue);

    // set base spark object parameters
    setBaseObjectParams(simpleInterpolator);

    // set new interpolator as node result
    ParamFloatInterpolator result;
    result.interpolatorFloat = simpleInterpolator;
    result.param = param;
    setResult(result);

    // trigger nodes connections
    dataUpdated(0);
}

//------------------------------------------------------------------------------------------------------------------------------
// random param interpolator node
//------------------------------------------------------------------------------------------------------------------------------

NodeSparkInterpolator_ParamInterpolatorRandom::NodeSparkInterpolator_ParamInterpolatorRandom()
{
    OUT_PORT(ENC_PARAMINTERPOLATOR, "param");

    createBaseObjectParams(Name());
    PARAM_FXY("BirthValue", eF32_MIN, eF32_MAX, 1.0f, 1.0f);
    PARAM_FXY("DeathValue", eF32_MIN, eF32_MAX, 1.0f, 1.0f);
    PARAM_ENUM("Param", "Scale|Mass|Angle|TextureIndex|RotationSpeed", 1);
}

void NodeSparkInterpolator_ParamInterpolatorRandom::process()
{
     // get parameters
    float minBirthValue = getParameter("BirthValue")->getValueAsFXY().x;
    float maxBirthValue = getParameter("BirthValue")->getValueAsFXY().y;
    float minDeathValue = getParameter("DeathValue")->getValueAsFXY().x;
    float maxDeathValue = getParameter("DeathValue")->getValueAsFXY().y;
    SPK::Param param = TABLE_PARAM[ getParameter("Param")->getValueAsEnum() ];

    // create new interpolator
    SPK::Ref<SPK::RandomInterpolator<float>> randomInterpolator;
    randomInterpolator = SPK::RandomInterpolator<float>::create(minBirthValue, maxBirthValue, minDeathValue, maxDeathValue);

    // set base spark object parameters
    setBaseObjectParams(randomInterpolator);

    // set new interpolator as node result
    ParamFloatInterpolator result;
    result.interpolatorFloat = randomInterpolator;
    result.param = param;
    setResult(result);

    // trigger nodes connections
    dataUpdated(0);
}
