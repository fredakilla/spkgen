#ifndef GRAPHEDITOR_H
#define GRAPHEDITOR_H

#include <QWidget>
#include <QRadioButton>

class GraphView;

class GraphEditor : public QWidget
{
    Q_OBJECT
public:
    GraphEditor(GraphView* graphView, QWidget* parent = nullptr);

public Q_SLOTS:
    void onSetPath();
    void onSetPath4();

private:
    QRadioButton* _radioX;
    QRadioButton* _radioY;
    QRadioButton* _radioZ;
    QRadioButton* _radioW;
    GraphView* _graphView;
};

#endif // GRAPHEDITOR_H
