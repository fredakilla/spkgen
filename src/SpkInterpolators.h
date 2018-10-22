#ifndef SPKINTERPOLATORS_H
#define SPKINTERPOLATORS_H

#include "BaseNode.h"

struct ResultInterpolator
{
    struct ParamFloatInterpolator
    {
        SPK::Param param;
        SPK::Ref<SPK::Interpolator<float>> interpolatorFloat;
    };

    // param interpolator
    ParamFloatInterpolator paramInterpolator;
    std::vector<ParamFloatInterpolator> paramInterpolators;

    // color interpolator
    SPK::Ref<SPK::Interpolator<SPK::Color>> interpolatorColor;
};

typedef MyNodeData<std::vector<SPK::Ref<ResultInterpolator>>, ENC_INTERPOLATOR>  NodeDataSparkInterpolatorList;



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
    std::vector<ResultInterpolator> _interpolators;
};


//------------------------------------------------------------------------------------------------------------------------------
// default color initializer node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkInterpolator_ColorDefaultInitializer : public NodeSparkInterpolatorBase
{
private:
    const QString Name() const override { return QString("ColorInit"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkInterpolatorList>(_interpolators); }
    void process() override;
public:
    NodeSparkInterpolator_ColorDefaultInitializer();
};



#endif // SPKINTERPOLATORS_H
