#include "SpkInterpolators.h"


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
    OUT_PORT(ENC_INTERPOLATOR, "interpolator");

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
    //_interpolators.interpolatorColor.reset();
    //_interpolators.interpolatorColor = defaultInitialiser;
    //setResult(defaultInitialiser);

    // trigger nodes connections
    dataUpdated(0);
}
