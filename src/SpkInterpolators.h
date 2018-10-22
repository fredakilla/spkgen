#ifndef SPKINTERPOLATORS_H
#define SPKINTERPOLATORS_H

#include "BaseNode.h"

//------------------------------------------------------------------------------------------------------------------------------
// base spark color interpolator class
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkColorInterpolatorBase : public NodeSparkBaseNode
{
protected:
    void setResult(const SPK::Ref<SPK::Interpolator<SPK::Color>> &colorInterpolator);
    SPK::Ref<SPK::Interpolator<SPK::Color>> _colorInterpolator;
};

//------------------------------------------------------------------------------------------------------------------------------
// base spark param interpolator class
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkParamInterpolatorBase : public NodeSparkBaseNode
{
protected:
    void setResult(const ParamFloatInterpolator& interpolator);
    std::vector<ParamFloatInterpolator> _paramInterpolators;
};

//------------------------------------------------------------------------------------------------------------------------------
// default color initializer node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkInterpolator_ColorDefaultInitializer : public NodeSparkColorInterpolatorBase
{
private:
    const QString Name() const override { return QString("ColorInitializer"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkColorInterpolator>(_colorInterpolator); }
    void process() override;
public:
    NodeSparkInterpolator_ColorDefaultInitializer();
};

//------------------------------------------------------------------------------------------------------------------------------
// param interpolator list node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkInterpolatorParamList : public NodeSparkParamInterpolatorBase
{
private:
    const QString Name() const override { return QString("Params"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkParamInterpolatorList>(_paramInterpolators); }
    void process() override;
public:
    NodeSparkInterpolatorParamList();
};

//------------------------------------------------------------------------------------------------------------------------------
// default param initializer node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkInterpolator_ParamInitializer : public NodeSparkParamInterpolatorBase
{
private:
    const QString Name() const override { return QString("ParamInitializer"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkParamInterpolatorList>(_paramInterpolators); }
    void process() override;
public:
    NodeSparkInterpolator_ParamInitializer();
};


#endif // SPKINTERPOLATORS_H
