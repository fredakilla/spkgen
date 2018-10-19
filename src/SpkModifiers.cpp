#include "SpkModifiers.h"


//--------------------------------------------------------------------------------------------
// base spark modifier
//--------------------------------------------------------------------------------------------

void NodeSparkModifierBase::createBaseModifierParams(eString name, bool shared)
{
    // create base SPKObject parameters
    createBaseObjectParams(name,shared);

    // create base modifier parameters
    PARAM_BOOL("LocalToSystem", eTRUE);
}

void NodeSparkModifierBase::setBaseModifierParams(SPK::Ref<SPK::Modifier> modifier)
{
    if(modifier)
    {
        // get parameters
        bool isLocal = getParameter("LocalToSystem")->getValueAsBool();

        // set Modifier base parameters
        modifier->setLocalToSystem(isLocal);

        // set base SPKObject parameters
        setBaseObjectParams(modifier);
    }
}

void NodeSparkModifierBase::setResult(SPK::Ref<SPK::Modifier> modifier)
{
    if(_modifiers.size() > 1)
        _modifiers.back().reset();
    _modifiers.clear();
    _modifiers.push_back(modifier);
}



//--------------------------------------------------------------------------------------------
// modifier list node
//--------------------------------------------------------------------------------------------

NodeSparkModifierList::NodeSparkModifierList()
{
    IN_PORT(EPT_MODIFIER, "modifier");
    IN_PORT(EPT_MODIFIER, "modifier");
    OUT_PORT(EPT_MODIFIER, "modifiers");
}

void NodeSparkModifierList::process()
{
    _modifiers.clear();
    for(size_t i=0; i<_inputs.size(); i++)
    {
        std::shared_ptr<NodeDataSparkModifierList> in = getInput<NodeDataSparkModifierList>(i);
        if(in.get())
        {
            for(size_t j=0; j<in->_result.size(); j++)
                _modifiers.push_back(in->_result[j]);
        }
    }

    dataUpdated(0);
}
