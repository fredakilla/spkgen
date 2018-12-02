#ifndef PARAMWIDGET_H
#define PARAMWIDGET_H

#include <QScrollArea>
#include <QVBoxLayout>
#include <QJsonObject>

class BaseNode;
class Parameter;

class ParamWidget : public QScrollArea
{
    Q_OBJECT

public:
    ParamWidget(BaseNode* node, QWidget* parent = nullptr);
    void save(QJsonObject& json);
    void restore(QJsonObject const &json);

private:

    void createWidgets();
    template<typename T> void createParamTrackEditX(Parameter* p, QHBoxLayout* hbl);
    template<typename T> void createParamTrackEditXY(Parameter* p, QHBoxLayout* hbl);
    template<typename T> void createParamTrackEditXYZ(Parameter* p, QHBoxLayout* hbl);
    template<typename T> void createParamTrackEditXYZW(Parameter* p, QHBoxLayout* hbl);
    void createParamLineEdit(Parameter* p, QHBoxLayout* hbl);
    void createParamBoolButton(Parameter* p, QHBoxLayout* hbl);
    void createParamEnum(Parameter* p, QHBoxLayout* hbl);
    void createParamFile(Parameter* p, QHBoxLayout* hbl);
    void createParamFlags(Parameter* p, QHBoxLayout* hbl);
    void createParamRGBA(Parameter* p, QHBoxLayout* hbl);
    void createParamButton(Parameter* p, QHBoxLayout* hbl);

    QVBoxLayout* _layout;
    BaseNode* _node;
};

#endif // PARAMWIDGET_H
