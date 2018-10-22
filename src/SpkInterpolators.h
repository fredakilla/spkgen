#ifndef SPKINTERPOLATORS_H
#define SPKINTERPOLATORS_H

#include "BaseNode.h"


struct ParamFloatInterpolator
{
    SPK::Param param;
    SPK::Ref<SPK::Interpolator<float>> interpolatorFloat;
};

struct ResultInterpolator
{
    // param interpolator
    ParamFloatInterpolator paramInterpolator;
    std::vector<ParamFloatInterpolator> paramInterpolators;

    // color interpolator
    SPK::Ref<SPK::Interpolator<SPK::Color>> interpolatorColor;
};

typedef MyNodeData<std::vector<ResultInterpolator>, ENC_INTERPOLATOR> NodeDataSparkInterpolatorList;


//---

typedef MyNodeData<SPK::Ref<SPK::Interpolator<SPK::Color>>, ENC_COLORINTERPOLATOR>  NodeDataSparkColorInterpolator;
typedef MyNodeData<ParamFloatInterpolator, ENC_PARAMINTERPOLATOR>  NodeDataSparkParamInterpolator;



//------------------------------------------------------------------------------------------------------------------------------
// base spark interpolator class
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkInterpolatorBase : public NodeSparkBaseNode
{
protected:
    //void createBaseInterpolatorParams(eString name, bool shared = false);
    //void setBaseInterpolatorParams(ResultInterpolator interpolator);
    //void setResult(ResultInterpolator interpolator);
    //std::vector<SPK::Ref<ResultInterpolator>> _interpolators;
    ///std::vector<ResultInterpolator> _interpolators;
};


//------------------------------------------------------------------------------------------------------------------------------
// default color initializer node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkInterpolator_ColorDefaultInitializer : public NodeSparkInterpolatorBase
{
    SPK::Ref<SPK::Interpolator<SPK::Color>> _colorInterpolator;
private:
    const QString Name() const override { return QString("ColorInit"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkColorInterpolator>(_colorInterpolator); }
    void process() override;
public:
    NodeSparkInterpolator_ColorDefaultInitializer();
};



//------------------------------------------------------------------------------------------------------------------------------
// default param initializer node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkInterpolator_ParamInitializer : public NodeSparkInterpolatorBase
{
    ParamFloatInterpolator _paramInterpolator;
private:
    const QString Name() const override { return QString("ParamInit"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkParamInterpolator>(_paramInterpolator); }
    void process() override;
public:
    NodeSparkInterpolator_ParamInitializer();
};


#endif // SPKINTERPOLATORS_H
