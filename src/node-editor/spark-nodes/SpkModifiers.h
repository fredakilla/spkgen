#ifndef SPKMODIFIERS_H
#define SPKMODIFIERS_H

#include "../common/BaseNode.h"

//------------------------------------------------------------------------------------------------------------------------------
// base spark modifier class
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkModifierBase : public NodeSparkBaseNode
{
protected:
    void createBaseModifierParams(eString name, bool shared = false);
    void setBaseModifierParams(SPK::Ref<SPK::Modifier> modifier);
    void setResult(SPK::Ref<SPK::Modifier> modifier);

    std::vector<SPK::Ref<SPK::Modifier>> _modifiers;
};

//------------------------------------------------------------------------------------------------------------------------------
// modifiers list node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkModifierList : public BaseNode
{
private:
    std::vector<SPK::Ref<SPK::Modifier>> _modifiers;
    const QString Name() const override { return QString("Modifiers"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkModifierList>(_modifiers); }
    void process() override;
public:
    NodeSparkModifierList();
};


//------------------------------------------------------------------------------------------------------------------------------
// gravity modifier node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkModifierGravity : public NodeSparkModifierBase
{
private:
    const QString Name() const override { return QString("Gravity"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkModifierList>(_modifiers); }
    void process() override;
public:
    NodeSparkModifierGravity();
};


//------------------------------------------------------------------------------------------------------------------------------
// friction modifier node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkModifierFriction : public NodeSparkModifierBase
{
private:
    const QString Name() const override { return QString("Friction"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkModifierList>(_modifiers); }
    void process() override;
public:
    NodeSparkModifierFriction();
};


//------------------------------------------------------------------------------------------------------------------------------
// collider modifier node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkModifierCollider : public NodeSparkModifierBase
{
private:
    const QString Name() const override { return QString("Collider"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkModifierList>(_modifiers); }
    void process() override;
public:
    NodeSparkModifierCollider();
};


//------------------------------------------------------------------------------------------------------------------------------
// destroyer modifier node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkModifierDestroyer : public NodeSparkModifierBase
{
private:
    const QString Name() const override { return QString("Destroyer"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkModifierList>(_modifiers); }
    void process() override;
public:
    NodeSparkModifierDestroyer();
};


//------------------------------------------------------------------------------------------------------------------------------
// obstacle modifier node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkModifierObstacle : public NodeSparkModifierBase
{
private:
    const QString Name() const override { return QString("Obstacle"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkModifierList>(_modifiers); }
    void process() override;
public:
    NodeSparkModifierObstacle();
};


//------------------------------------------------------------------------------------------------------------------------------
// pointmass modifier node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkModifierPointMass : public NodeSparkModifierBase
{
private:
    const QString Name() const override { return QString("PointMass"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkModifierList>(_modifiers); }
    void process() override;
public:
    NodeSparkModifierPointMass();
};


//------------------------------------------------------------------------------------------------------------------------------
// random force modifier node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkModifierRandomForce : public NodeSparkModifierBase
{
private:
    const QString Name() const override { return QString("RandomForce"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkModifierList>(_modifiers); }
    void process() override;
public:
    NodeSparkModifierRandomForce();
};


//------------------------------------------------------------------------------------------------------------------------------
// rotator modifier node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkModifierRotator : public NodeSparkModifierBase
{
private:
    const QString Name() const override { return QString("Rotator"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkModifierList>(_modifiers); }
    void process() override;
public:
    NodeSparkModifierRotator();
};


//------------------------------------------------------------------------------------------------------------------------------
// vortex modifier node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkModifierVortex : public NodeSparkModifierBase
{
private:
    const QString Name() const override { return QString("Vortex"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkModifierList>(_modifiers); }
    void process() override;
public:
    NodeSparkModifierVortex();
};


//------------------------------------------------------------------------------------------------------------------------------
// emitterAttacher modifier node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkModifierEmitterAttacher : public NodeSparkModifierBase
{
private:
    const QString Name() const override { return QString("EmitterAttacher"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkModifierList>(_modifiers); }
    void process() override;
public:
    NodeSparkModifierEmitterAttacher();
};


//------------------------------------------------------------------------------------------------------------------------------
// linearForce modifier node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkModifierLinearForce : public NodeSparkModifierBase
{
private:
    const QString Name() const override { return QString("LinearForce"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkModifierList>(_modifiers); }
    void process() override;
public:
    NodeSparkModifierLinearForce();
};
#endif // SPKMODIFIERS_H
