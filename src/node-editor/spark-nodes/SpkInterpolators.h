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
// path4 node
//------------------------------------------------------------------------------------------------------------------------------
class NodePath4 : public BaseNode
{
    Path4* _path4;
private:
    const QString Name() const override { return QString("Path4"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataPath4>(_path4); }
    void process() override;
public:
    NodePath4();
    Path4* getResult() { return _path4; }
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
    const QString Name() const override { return QString("ColorInitializerRandom"); }
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
    const QString Name() const override { return QString("ColorInterpolator"); }
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
    const QString Name() const override { return QString("ColorInterpolatorRandom"); }
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
    const QString Name() const override { return QString("ColorInterpolatorGraph"); }
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
    const QString Name() const override { return QString("ParamInitializerRandom"); }
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
    const QString Name() const override { return QString("ParamInterpolator"); }
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
    const QString Name() const override { return QString("ParamInterpolatorRandom"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkParamInterpolatorList>(_paramInterpolators); }
    void process() override;
public:
    NodeSparkInterpolator_ParamInterpolatorRandom();
};


//------------------------------------------------------------------------------------------------------------------------------
// graph param interpolator node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkInterpolator_ParamInterpolatorGraph : public NodeSparkParamInterpolatorBase
{
private:
    const QString Name() const override { return QString("ParamInterpolatorGraph"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkParamInterpolatorList>(_paramInterpolators); }
    void process() override;
public:
    NodeSparkInterpolator_ParamInterpolatorGraph();
};

#endif // SPKINTERPOLATORS_H
