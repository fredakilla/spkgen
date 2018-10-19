#ifndef SPKMODIFIERS_H
#define SPKMODIFIERS_H

#include "BaseNode.h"

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


#endif // SPKMODIFIERS_H
