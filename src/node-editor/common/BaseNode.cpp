#include "BaseNode.h"

BaseNode::BaseNode() : _paramWidget(nullptr)
{
}

BaseNode::~BaseNode()
{
}

unsigned int BaseNode::nPorts(PortType portType) const
{
    if(portType == PortType::In)
    {
        return _inputs.size();
    }
    else if(portType == PortType::Out)
    {
        return _outputs.size();
    }
    else
    {
        return 0;
    }
}

QString BaseNode::portCaption(PortType portType, PortIndex portIndex) const
{
    if(portType == PortType::In)
    {
        Q_ASSERT((size_t)portIndex < _inputs.size());
        return _inputs[portIndex]->name;
    }
    else if(portType == PortType::Out)
    {
        Q_ASSERT((size_t)portIndex < _outputs.size());
        return _outputs[portIndex]->name;
    }
    else
    {
        return QString("");
    }
}


NodeDataType BaseNode::dataType(PortType portType, PortIndex portIndex) const
{
    if(portType == PortType::In)
    {
        return _inputs[portIndex]->dataType;
    }
    else if(portType == PortType::Out)
    {
        return _outputs[portIndex]->dataType;
    }
    else
    {
        return NODE_DATA_TYPE[EPT_NONE];
    }
}

void BaseNode::setInData(std::shared_ptr<NodeData> nodeData, PortIndex portIndex)
{
    _inputs[portIndex]->data = nodeData;
    process();
}

unsigned int BaseNode::getParameterCount()
{
    return (unsigned int)_parameters.size();
}

Parameter* BaseNode::getParameter(unsigned int index)
{
    if(index > getParameterCount())
        return nullptr;

    return _parameters.at(index);
}

Parameter* BaseNode::getParameter(const QString name)
{
    for (auto p : _parameters)
    {
        if(p->name == name)
            return p;
    }

    qFatal("FATAL error : Parameter '%s' does not exists in '%s' Node",
           name.toStdString().c_str(),
           this->Name().toStdString().c_str()
           );

    return nullptr;
}

NodeValidationState BaseNode::validationState() const
{
    // check if _paramWidget was created when node has paramaters.
    // if fatal error, you miss to call createParamWidgets() in the node constructor.
    if(_parameters.size() > 0 && _paramWidget == nullptr)
    {
        //qFatal("Fatal: _paramWidget was not created for node '%s'", Name().toStdString().c_str());
        // or
        // force it here ?? (may be issues when serialize ?? to test...)
        //qWarning("Warning: _paramWidget was not created for node '%s'", Name().toStdString().c_str());
        (const_cast<BaseNode*>(this))->createParamWidgets();
    }

    return modelValidationState;
}

QString BaseNode::validationMessage() const
{
    return modelValidationError;
}
