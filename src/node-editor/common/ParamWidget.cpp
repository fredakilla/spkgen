#include "ParamWidget.h"
#include "BaseNode.h"
#include "CustomWidgets.h"

#include <QLabel>
#include <QSpinBox>
#include <QJsonArray>

ParamWidget::ParamWidget(BaseNode* node, QWidget* parent)
    : QScrollArea(parent)
    , _layout(nullptr)
    , _node(node)
{
    setWidgetResizable(true);
    setWidget(new QFrame(viewport()));

    _layout = new QVBoxLayout(widget());
    _layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    _layout->setSpacing(2);
    setLayout(_layout);

    createWidgets();
}


void ParamWidget::createWidgets()
{
    // calculate max description label width
    eInt maxLabelWidth = 0;

    for (eU32 i=0; i<_node->getParameterCount(); i++)
    {
        const Parameter* p = _node->getParameter(i);
        const eInt curWidth = fontMetrics().width(QString(p->name));
        maxLabelWidth = eMax(maxLabelWidth, curWidth);
    }

    // create parameter widgets
    for(unsigned int i=0; i<_node->getParameterCount(); i++)
    {
        Parameter* p = _node->getParameter(i);

        // create horz. layout and add description label
        QLabel *descrLabel = new QLabel(QString(p->name));
        descrLabel->setFixedWidth(maxLabelWidth);
        QHBoxLayout *hbl = new QHBoxLayout;
        _layout->addLayout(hbl);
        hbl->setSpacing(2);
        hbl->addWidget(descrLabel);

        // create widget according parameter type
        switch(p->type)
        {

        case EPT_BUTTON:
            createParamButton(p, hbl);
            break;

        case EPT_FILE:
            createParamFile(p, hbl);
            break;

        case EPT_STRING:
            createParamLineEdit(p, hbl);
            break;

        case EPT_ENUM:
            createParamEnum(p, hbl);
            break;

        case EPT_FLAGS:
            createParamFlags(p, hbl);
            break;

        case EPT_RGBA:
            createParamRGBA(p, hbl);
            break;

        case EPT_BOOL:
            createParamBoolButton(p, hbl);
            break;

        case EPT_FLOAT:
            createParamTrackEditX<eF32>(p, hbl);
            break;

        case EPT_FXY:
            createParamTrackEditXY<eFXY>(p, hbl);
            break;

        case EPT_FXYZ:
            createParamTrackEditXYZ<eFXYZ>(p, hbl);
            break;

        case EPT_FXYZW:
            createParamTrackEditXYZW<eFXYZW>(p, hbl);
            break;

        case EPT_INT:
            createParamTrackEditX<eInt>(p, hbl);
            break;

        case EPT_IXY:
            createParamTrackEditXY<eIXY>(p, hbl);
            break;

        case EPT_IXYZ:
            createParamTrackEditXYZ<eIXYZ>(p, hbl);
            break;

        case EPT_IXYZW:
            createParamTrackEditXYZW<eIXYZW>(p, hbl);
            break;
        }
    }
}

void ParamWidget::createParamButton(Parameter* p, QHBoxLayout* hbl)
{
    eButton* simpleButton = new eButton(*p);
    hbl->addWidget(simpleButton);
    connect(simpleButton, &eButton::onParameterChanged, _node, &BaseNode::onParameterChanged);
}

void ParamWidget::createParamRGBA(Parameter* p, QHBoxLayout* hbl)
{
    eColorFrame* colorFrame = new eColorFrame(*p);
    hbl->addWidget(colorFrame);
    connect(colorFrame, &eColorFrame::onParameterChanged, _node, &BaseNode::onParameterChanged);
}

void ParamWidget::createParamFlags(Parameter* p, QHBoxLayout* hbl)
{
    const QStringList descrItems = QString(p->getDescription()).split("|");
    for (eInt j=0; j<descrItems.size(); j++)
    {
        eFlagButton* flagButton = new eFlagButton(*p, descrItems[j], j);
        hbl->addWidget(flagButton, 1);
        connect(flagButton, &eFlagButton::onParameterChanged, _node, &BaseNode::onParameterChanged);
    }
}

void ParamWidget::createParamFile(Parameter* p, QHBoxLayout* hbl)
{
    eFileFrame* fileFrame = new eFileFrame(*p);
    hbl->addWidget(fileFrame);
    connect(fileFrame, &eFileFrame::onParameterChanged, _node, &BaseNode::onParameterChanged);
}


void ParamWidget::createParamEnum(Parameter* p, QHBoxLayout* hbl)
{
    eComboBox* comboBox = new eComboBox(*p);
    hbl->addWidget(comboBox);
    connect(comboBox, &eComboBox::onParameterChanged, _node, &BaseNode::onParameterChanged);
}

void ParamWidget::createParamLineEdit(Parameter* p, QHBoxLayout* hbl)
{
    //QString* var = &(p->baseValue.get<QString>());
    eLineEdit* textEdit = new eLineEdit(*p);
    hbl->addWidget(textEdit);
    connect(textEdit, &eLineEdit::onParameterChanged, _node, &BaseNode::onParameterChanged);
}

void ParamWidget::createParamBoolButton(Parameter* p, QHBoxLayout* hbl)
{
    eBoolButton* boolButton = new eBoolButton(*p);
    hbl->addWidget(boolButton);
    connect(boolButton, &eBoolButton::onParameterChanged, _node, &BaseNode::onParameterChanged);
}

template<typename T>
void ParamWidget::createParamTrackEditX(Parameter* p, QHBoxLayout* hbl)
{
    T& var = p->baseValue.get<T>();
    eParamTrackEdit* trackEdit = new eParamTrackEdit(var, *p);
    hbl->addWidget(trackEdit);
    connect(trackEdit, &eParamTrackEdit::onParameterChanged, _node, &BaseNode::onParameterChanged);
}

template<typename T>
void ParamWidget::createParamTrackEditXY(Parameter* p, QHBoxLayout* hbl)
{
    T* var = &(p->baseValue.get<T>());

    eParamTrackEdit* trackEdit1 = new eParamTrackEdit(var->x, *p);
    hbl->addWidget(trackEdit1);
    connect(trackEdit1, &eParamTrackEdit::onParameterChanged, _node, &BaseNode::onParameterChanged);

    eParamTrackEdit* trackEdit2 = new eParamTrackEdit(var->y, *p);
    hbl->addWidget(trackEdit2);
    connect(trackEdit2, &eParamTrackEdit::onParameterChanged, _node, &BaseNode::onParameterChanged);
}

template<typename T>
void ParamWidget::createParamTrackEditXYZ(Parameter* p, QHBoxLayout* hbl)
{
    T* var = &(p->baseValue.get<T>());

    eParamTrackEdit* trackEdit1 = new eParamTrackEdit(var->x, *p);
    hbl->addWidget(trackEdit1);
    connect(trackEdit1, &eParamTrackEdit::onParameterChanged, _node, &BaseNode::onParameterChanged);

    eParamTrackEdit* trackEdit2 = new eParamTrackEdit(var->y, *p);
    hbl->addWidget(trackEdit2);
    connect(trackEdit2, &eParamTrackEdit::onParameterChanged, _node, &BaseNode::onParameterChanged);

    eParamTrackEdit* trackEdit3 = new eParamTrackEdit(var->z, *p);
    hbl->addWidget(trackEdit3);
    connect(trackEdit3, &eParamTrackEdit::onParameterChanged, _node, &BaseNode::onParameterChanged);
}

template<typename T>
void ParamWidget::createParamTrackEditXYZW(Parameter* p, QHBoxLayout* hbl)
{
    T* var = &(p->baseValue.get<T>());

    eParamTrackEdit* trackEdit1 = new eParamTrackEdit(var->x, *p);
    hbl->addWidget(trackEdit1);
    connect(trackEdit1, &eParamTrackEdit::onParameterChanged, _node, &BaseNode::onParameterChanged);

    eParamTrackEdit* trackEdit2 = new eParamTrackEdit(var->y, *p);
    hbl->addWidget(trackEdit2);
    connect(trackEdit2, &eParamTrackEdit::onParameterChanged, _node, &BaseNode::onParameterChanged);

    eParamTrackEdit* trackEdit3 = new eParamTrackEdit(var->z, *p);
    hbl->addWidget(trackEdit3);
    connect(trackEdit3, &eParamTrackEdit::onParameterChanged, _node, &BaseNode::onParameterChanged);

    eParamTrackEdit* trackEdit4 = new eParamTrackEdit(var->w, *p);
    hbl->addWidget(trackEdit4);
    connect(trackEdit4, &eParamTrackEdit::onParameterChanged, _node, &BaseNode::onParameterChanged);
}



void ParamWidget::save(QJsonObject& json)
{
    QJsonArray paraArray;

    for(unsigned int i=0; i<_node->getParameterCount(); i++)
    {
        Parameter* p = _node->getParameter(i);

        QJsonObject param;

        QString key = QString::number(p->type);
        switch(p->type)
        {
        case EPT_NONE:
            Q_ASSERT(0);
            break;
        case EPT_BUTTON:
        case EPT_STRING:
        case EPT_FILE:
            param[key] = p->getValueAsString();
            break;
        case EPT_BOOL:
            param[key] = p->getValueAsBool();
            break;
        case EPT_FLAGS:
            param[key] = p->getValueAsFlags();
            break;
        case EPT_ENUM:
            param[key] = p->getValueAsEnum();
            break;
        case EPT_FLOAT:
            param[key] = p->getValueAsFloat();
            break;
        case EPT_FXY:
        {
            QJsonArray valuArray;
            valuArray.append((float)p->getValueAsFXY().x);
            valuArray.append((float)p->getValueAsFXY().y);
            param[key] = valuArray;
        }
            break;
        case EPT_FXYZ:
        {
            QJsonArray valuArray;
            valuArray.append((float)p->getValueAsFXYZ().x);
            valuArray.append((float)p->getValueAsFXYZ().y);
            valuArray.append((float)p->getValueAsFXYZ().z);
            param[key] = valuArray;
        }
            break;
        case EPT_FXYZW:
        {
            QJsonArray valuArray;
            valuArray.append((float)p->getValueAsFXYZW().x);
            valuArray.append((float)p->getValueAsFXYZW().y);
            valuArray.append((float)p->getValueAsFXYZW().z);
            valuArray.append((float)p->getValueAsFXYZW().w);
            param[key] = valuArray;
        }
            break;
        case EPT_INT:
            param[key] = p->getValueAsInt();
            break;
        case EPT_IXY:
        {
            QJsonArray valuArray;
            valuArray.append((int)p->getValueAsIXY().x);
            valuArray.append((int)p->getValueAsIXY().y);
            param[key] = valuArray;
        }
            break;
        case EPT_IXYZ:
        {
            QJsonArray valuArray;
            valuArray.append((int)p->getValueAsIXYZ().x);
            valuArray.append((int)p->getValueAsIXYZ().y);
            valuArray.append((int)p->getValueAsIXYZ().z);
            param[key] = valuArray;
        }
            break;
        case EPT_IXYZW:
        {
            QJsonArray valuArray;
            valuArray.append((int)p->getValueAsIXYZW().x);
            valuArray.append((int)p->getValueAsIXYZW().y);
            valuArray.append((int)p->getValueAsIXYZW().z);
            valuArray.append((int)p->getValueAsIXYZW().w);
            param[key] = valuArray;
        }
            break;
        case EPT_TEXT:
            break;
        case EPT_RGB:
            break;
        case EPT_RGBA:
        {
            QJsonArray valuArray;
            valuArray.append((int)p->getValueAsColor().r);
            valuArray.append((int)p->getValueAsColor().g);
            valuArray.append((int)p->getValueAsColor().b);
            valuArray.append((int)p->getValueAsColor().a);
            param[key] = valuArray;
        }
            break;

        default:
            // parameter type is not yet implemented ?
            Q_ASSERT(0);
        }
        paraArray.append(param);


    }

    json["parameters"] = paraArray;


}

void ParamWidget::restore(QJsonObject const &json)
{
    QJsonArray paraArray = json["parameters"].toArray();
    for (int paraIndex = 0; paraIndex < paraArray.size(); ++paraIndex)
    {
        QJsonObject paraObject = paraArray[paraIndex].toObject();

        // in case of parameter has not been saved.
        // go to next
        if(paraObject.size() == 0)
            continue;

        QString paraTypeString = paraObject.keys().at(0);
        ParamType type = (ParamType)paraTypeString.toUInt();

        Parameter* p = _node->getParameter(paraIndex);

        switch(type)
        {
        case EPT_NONE:
            Q_ASSERT(0);
            break;
        case EPT_BUTTON:
        case EPT_STRING:
        case EPT_FILE:
            p->baseValue = paraObject.begin().value().toString();
            break;
        case EPT_BOOL:
            p->baseValue = (bool)paraObject.begin().value().toBool();
            break;
        case EPT_FLAGS:
            p->baseValue = (unsigned char)paraObject.begin().value().toInt();
            break;
        case EPT_ENUM:
            p->baseValue = (int)paraObject.begin().value().toInt();
            break;
        case EPT_FLOAT:
            p->baseValue = (float)paraObject.begin().value().toDouble();
            break;
        case EPT_FXY:
        {
            const QJsonArray valueArray = paraObject.begin().value().toArray();
            float x = (float)valueArray.at(0).toDouble();
            float y = (float)valueArray.at(1).toDouble();
            p->baseValue = eFXY(x,y);
        }
            break;
        case EPT_FXYZ:
        {
            const QJsonArray valueArray = paraObject.begin().value().toArray();
            float x = (float)valueArray.at(0).toDouble();
            float y = (float)valueArray.at(1).toDouble();
            float z = (float)valueArray.at(2).toDouble();
            p->baseValue = eFXYZ(x,y,z);
        }
            break;
        case EPT_FXYZW:
        {
            const QJsonArray valueArray = paraObject.begin().value().toArray();
            float x = (float)valueArray.at(0).toDouble();
            float y = (float)valueArray.at(1).toDouble();
            float z = (float)valueArray.at(2).toDouble();
            float w = (float)valueArray.at(3).toDouble();
            p->baseValue = eFXYZW(x,y,z,w);
        }
            break;
        case EPT_INT:
            p->baseValue = (int)paraObject.begin().value().toInt();
            break;
        case EPT_IXY:
        {
            const QJsonArray valueArray = paraObject.begin().value().toArray();
            int x = valueArray.at(0).toInt();
            int y = valueArray.at(1).toInt();
            p->baseValue = eIXY(x,y);
        }
            break;
        case EPT_IXYZ:
        {
            const QJsonArray valueArray = paraObject.begin().value().toArray();
            int x = valueArray.at(0).toInt();
            int y = valueArray.at(1).toInt();
            int z = valueArray.at(2).toInt();
            p->baseValue = eIXYZ(x,y,z);
        }
            break;
        case EPT_IXYZW:
        {
            const QJsonArray valueArray = paraObject.begin().value().toArray();
            int x = valueArray.at(0).toInt();
            int y = valueArray.at(1).toInt();
            int z = valueArray.at(2).toInt();
            int w = valueArray.at(3).toInt();
            p->baseValue = eIXYZW(x,y,z,w);
        }
            break;
        case EPT_TEXT:
            break;
        case EPT_RGB:
            break;
        case EPT_RGBA:
        {
            const QJsonArray valueArray = paraObject.begin().value().toArray();
            int r = valueArray.at(0).toInt();
            int g = valueArray.at(1).toInt();
            int b = valueArray.at(2).toInt();
            int a = valueArray.at(3).toInt();
            p->baseValue = eColor(r,g,b,a);
        }
            break;
        default:
            Q_ASSERT(0);
        }

        Q_EMIT(p->parameterUpdated());
    }
}


