#ifndef SPKSYSTEMNODE_H
#define SPKSYSTEMNODE_H

#include "../common/BaseNode.h"

//------------------------------------------------------------------------------------------------------------------------------
// group list
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkGroupList : public BaseNode
{
private:
    std::vector<SPK::Ref<SPK::Group>> _groups;
    const QString Name() const override { return QString("Groups"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkGroupList>(_groups); }
    void process() override;
public:
    NodeSparkGroupList();
};

//------------------------------------------------------------------------------------------------------------------------------
// group
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkGroup : public NodeSparkBaseNode
{
private:
    std::vector<SPK::Ref<SPK::Group>> _groups;
    const QString Name() const override { return QString("Group"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkGroupList>(_groups); }
    void process() override;
public:
    NodeSparkGroup();
};

//------------------------------------------------------------------------------------------------------------------------------
// system
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkSystem : public BaseNode
{
private:
    SPK::Ref<SPK::System> _system;
    const QString Name() const override { return QString("System"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkSystem>(_system); }
    void process() override;
public:
    NodeSparkSystem();
    SPK::Ref<SPK::System> getResult() { return _system; }
};

//------------------------------------------------------------------------------------------------------------------------------
// quad renderer
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkQuadRenderer : public NodeSparkBaseNode
{
private:
    SPK::Ref<SPK::Renderer> _renderer;
    const QString Name() const override { return QString("QuadRenderer"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkRenderer>(_renderer); }
    void process() override;
public:
    NodeSparkQuadRenderer();
};

//------------------------------------------------------------------------------------------------------------------------------
// test node
//------------------------------------------------------------------------------------------------------------------------------
class NodeSparkTest : public NodeSparkBaseNode
{
private:
    SPK::Ref<SPK::Renderer> _renderer;
    const QString Name() const override { return QString("Test"); }
    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<NodeDataSparkRenderer>(_renderer); }
    void process() override;
public:
    NodeSparkTest();

private Q_SLOTS:
    void onTestButtonClick();
};

#endif // SPKSYSTEMNODE_H
