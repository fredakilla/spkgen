#ifndef SPKINTERPOLATORS_H
#define SPKINTERPOLATORS_H

#include "../common/BaseNode.h"

//------------------------------------------------------------------------------------------------------------------------------
// path node
//------------------------------------------------------------------------------------------------------------------------------
class NodePath : public BaseNode
{
    Path* _path;
private:
    const QString Name() const override { return QString("Path"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataPath>(_path); }
    void process() override;
public:
    NodePath();
    Path* getResult() { return _path; }
};


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
class NodeSparkInterpolator_ColorInitializerDefault : public NodeSparkColorInterpolatorBase
{
private:
    const QString Name() const override { return QString("ColorInitializer"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkColorInterpolator>(_colorInterpolator); }
    void process() override;
public:
    NodeSparkInterpolator_ColorInitializerDefault();
};

//------------------------------------------------------------------------------------------------------------------------------
// random color initializer node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkInterpolator_ColorInitializerRandom : public NodeSparkColorInterpolatorBase
{
private:
    const QString Name() const override { return QString("ColorRandomInitializer"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkColorInterpolator>(_colorInterpolator); }
    void process() override;
public:
    NodeSparkInterpolator_ColorInitializerRandom();
};

//------------------------------------------------------------------------------------------------------------------------------
// simple color interpolator node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkInterpolator_ColorInterpolatorSimple : public NodeSparkColorInterpolatorBase
{
private:
    const QString Name() const override { return QString("ColorSimpleInterpolator"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkColorInterpolator>(_colorInterpolator); }
    void process() override;
public:
    NodeSparkInterpolator_ColorInterpolatorSimple();
};

//------------------------------------------------------------------------------------------------------------------------------
// random color interpolator node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkInterpolator_ColorInterpolatorRandom : public NodeSparkColorInterpolatorBase
{
private:
    const QString Name() const override { return QString("ColorRandomInterpolator"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkColorInterpolator>(_colorInterpolator); }
    void process() override;
public:
    NodeSparkInterpolator_ColorInterpolatorRandom();
};

//------------------------------------------------------------------------------------------------------------------------------
// graph color interpolator node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkInterpolator_ColorInterpolatorGraph : public NodeSparkColorInterpolatorBase
{
private:
    const QString Name() const override { return QString("ColorGraphInterpolator"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkColorInterpolator>(_colorInterpolator); }
    void process() override;
public:
    NodeSparkInterpolator_ColorInterpolatorGraph();
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

//------------------------------------------------------------------------------------------------------------------------------
// random param initializer node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkInterpolator_ParamInitializerRandom : public NodeSparkParamInterpolatorBase
{
private:
    const QString Name() const override { return QString("ParamRandomInitializer"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkParamInterpolatorList>(_paramInterpolators); }
    void process() override;
public:
    NodeSparkInterpolator_ParamInitializerRandom();
};

//------------------------------------------------------------------------------------------------------------------------------
// simple param interpolator node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkInterpolator_ParamInterpolatorSimple : public NodeSparkParamInterpolatorBase
{
private:
    const QString Name() const override { return QString("ParamSimpleInterpolator"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkParamInterpolatorList>(_paramInterpolators); }
    void process() override;
public:
    NodeSparkInterpolator_ParamInterpolatorSimple();
};

//------------------------------------------------------------------------------------------------------------------------------
// random param interpolator node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkInterpolator_ParamInterpolatorRandom : public NodeSparkParamInterpolatorBase
{
private:
    const QString Name() const override { return QString("ParamRandomInterpolator"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkParamInterpolatorList>(_paramInterpolators); }
    void process() override;
public:
    NodeSparkInterpolator_ParamInterpolatorRandom();
};



#endif // SPKINTERPOLATORS_H
