#include "GraphEditor.h"
#include "GraphView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QGroupBox>
#include <QLabel>

GraphEditor::GraphEditor(GraphView* graphView, QWidget* parent) :
    QWidget(parent)
  , _graphView(graphView)
{
    _radioX = new QRadioButton(tr("X"));
    _radioY = new QRadioButton(tr("Y"));
    _radioZ = new QRadioButton(tr("Z"));
    _radioW = new QRadioButton(tr("W"));

    _radioX->setVisible(true);
    _radioY->setVisible(false);
    _radioZ->setVisible(false);
    _radioW->setVisible(false);
    _radioX->setChecked(true);

    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addWidget(new QLabel("Selected path : "));
    hbox->addWidget(_radioX);
    hbox->addWidget(_radioY);
    hbox->addWidget(_radioZ);
    hbox->addWidget(_radioW);
    hbox->addStretch(1);

    QGroupBox* groupBox = new QGroupBox();
    groupBox->setLayout(hbox);

    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->addWidget(graphView);
    vbox->addWidget(groupBox);
    setLayout(vbox);

    connect(_radioX, &QRadioButton::clicked, graphView, &GraphView::onPathSelectedX);
    connect(_radioY, &QRadioButton::clicked, graphView, &GraphView::onPathSelectedY);
    connect(_radioZ, &QRadioButton::clicked, graphView, &GraphView::onPathSelectedZ);
    connect(_radioW, &QRadioButton::clicked, graphView, &GraphView::onPathSelectedW);
}

void GraphEditor::onSetPath()
{
    _radioX->setVisible(true);
    _radioY->setVisible(false);
    _radioZ->setVisible(false);
    _radioW->setVisible(false);
    _radioX->setChecked(true);
}

void GraphEditor::onSetPath4()
{
    _radioX->setVisible(true);
    _radioY->setVisible(true);
    _radioZ->setVisible(true);
    _radioW->setVisible(true);
}
