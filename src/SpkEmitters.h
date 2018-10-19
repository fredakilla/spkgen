#ifndef SPKEMITTERS_H
#define SPKEMITTERS_H

#include "BaseNode.h"

//------------------------------------------------------------------------------------------------------------------------------
// base spark emitter class
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkEmitterBase : public NodeSparkBaseNode
{
protected:
    void createBaseEmitterParams(eString name, bool shared = false);
    void setBaseEmitterParams(SPK::Ref<SPK::Emitter> emitter);
    void setResult(SPK::Ref<SPK::Emitter> emitter);

    std::vector<SPK::Ref<SPK::Emitter>> _emitters;
};

//------------------------------------------------------------------------------------------------------------------------------
// emitters list node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkEmitterList : public BaseNode
{
private:
    std::vector<SPK::Ref<SPK::Emitter>> _emitters;
    const QString Name() const override { return QString("Emitters"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkEmitterList>(_emitters); }
    void process() override;
public:
    NodeSparkEmitterList();
};

//------------------------------------------------------------------------------------------------------------------------------
// static emitter node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkEmitterStatic : public NodeSparkEmitterBase
{
private:
    const QString Name() const override { return QString("StaticEmitter"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkEmitterList>(_emitters); }
    void process() override;
public:
    NodeSparkEmitterStatic();
};

//------------------------------------------------------------------------------------------------------------------------------
// spheric emitter node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkEmitterSpheric : public NodeSparkEmitterBase
{
private:
    const QString Name() const override { return QString("SphericEmitter"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkEmitterList>(_emitters); }
    void process() override;
public:
    NodeSparkEmitterSpheric();
};


//------------------------------------------------------------------------------------------------------------------------------
// random emitter node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkEmitterRandom : public NodeSparkEmitterBase
{
private:
    const QString Name() const override { return QString("RandomEmitter"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkEmitterList>(_emitters); }
    void process() override;
public:
    NodeSparkEmitterRandom();
};

#endif // SPKEMITTERS_H
