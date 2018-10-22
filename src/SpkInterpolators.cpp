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

/*
void NodeSparkInterpolatorBase::setResult(ResultInterpolator interpolator)
{
    if(_emitters.size() > 1)
        _emitters.back().reset();
    _emitters.clear();
    _emitters.push_back(emitter);
}
*/

//------------------------------------------------------------------------------------------------------------------------------
// default color initializer node
//------------------------------------------------------------------------------------------------------------------------------

NodeSparkInterpolator_ColorDefaultInitializer::NodeSparkInterpolator_ColorDefaultInitializer()
{
    OUT_PORT(ENC_COLORINTERPOLATOR, "colors");

    createBaseObjectParams(Name());
    PARAM_RGBA("DefaultValue", 255, 0, 0, 255);
}

void NodeSparkInterpolator_ColorDefaultInitializer::process()
{
     // get parameters
    SPK::Color color = ToSpkColor( getParameter("DefaultValue")->getValueAsColor() );

    // create new interpolator
    SPK::Ref<SPK::DefaultInitializer<SPK::Color>> defaultInitialiser;
    defaultInitialiser = SPK::DefaultInitializer<SPK::Color>::create(color);

    // set base spark object parameters
    setBaseObjectParams(defaultInitialiser);

    // set new interpolator as node result
   /// _interpolators.interpolatorColor.reset();
   /// _interpolators.interpolatorColor = defaultInitialiser;
    //setResult(defaultInitialiser);


   /* ResultInterpolator result;
    result.interpolatorColor = defaultInitialiser;

    if(_interpolators.size() > 1)
        _interpolators.back().interpolatorColor.reset();
    _interpolators.clear();
    _interpolators.push_back(result);*/

    _colorInterpolator.reset();
    _colorInterpolator = defaultInitialiser;



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
    if(_paramInterpolators.size() > 1)
        _paramInterpolators.back().interpolatorFloat.reset();
    _paramInterpolators.clear();

    ParamFloatInterpolator interpolator;
    interpolator.interpolatorFloat = defaultInitialiser;
    interpolator.param = param;
    _paramInterpolators.push_back(interpolator);

    // trigger nodes connections
    dataUpdated(0);
}
