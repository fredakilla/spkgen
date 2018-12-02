#ifndef ZONENODE_H
#define ZONENODE_H

#include "../common/BaseNode.h"

//------------------------------------------------------------------------------------------------------------------------------
// base spark zone class
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkZoneBase : public NodeSparkBaseNode
{
protected:
    void createBaseZoneParams(eString name, bool shared = false);
    void setBaseZoneParams(SPK::Ref<SPK::Zone> zone);
    void setResult(SPK::Ref<SPK::Zone> zone);

    SPK::Ref<SPK::Zone> _zone;
};


//------------------------------------------------------------------------------------------------------------------------------
// point zone node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkZonePoint : public NodeSparkZoneBase
{
private:
    const QString Name() const override { return QString("PointZone"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkZone>(_zone); }
    void process() override;
public:
    NodeSparkZonePoint();
};

//------------------------------------------------------------------------------------------------------------------------------
// plane zone node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkZonePlane : public NodeSparkZoneBase
{
private:
    const QString Name() const override { return QString("PlaneZone"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkZone>(_zone); }
    void process() override;
public:
    NodeSparkZonePlane();
};


//------------------------------------------------------------------------------------------------------------------------------
// sphere zone node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkZoneSphere : public NodeSparkZoneBase
{
private:
    const QString Name() const override { return QString("SphereZone"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkZone>(_zone); }
    void process() override;
public:
    NodeSparkZoneSphere();
};


//------------------------------------------------------------------------------------------------------------------------------
// box zone node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkZoneBox : public NodeSparkZoneBase
{
private:
    const QString Name() const override { return QString("BoxZone"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkZone>(_zone); }
    void process() override;
public:
    NodeSparkZoneBox();
};

//------------------------------------------------------------------------------------------------------------------------------
// cylinder zone node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkZoneCylinder : public NodeSparkZoneBase
{
private:
    const QString Name() const override { return QString("CylinderZone"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkZone>(_zone); }
    void process() override;
public:
    NodeSparkZoneCylinder();
};

//------------------------------------------------------------------------------------------------------------------------------
// ring zone node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkZoneRing : public NodeSparkZoneBase
{
private:
    const QString Name() const override { return QString("RingZone"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkZone>(_zone); }
    void process() override;
public:
    NodeSparkZoneRing();
};

#endif // ZONENODE_H
