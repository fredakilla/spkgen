#ifndef PARAM_WIDGETS_HPP
#define PARAM_WIDGETS_HPP
#pragma once

#include <QtWidgets/QLabel>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QPushButton>

#include "Parameter.h"
#include "Trackedit.h"

//------------------------------------------------------------------------------------------------------------------------------
// eParamTrackEdit2
//------------------------------------------------------------------------------------------------------------------------------
class eParamTrackEdit : public eTrackEdit
{
    Q_OBJECT
public:
    eParamTrackEdit(eF32 &flt, Parameter &param, QWidget *parent=nullptr);
    eParamTrackEdit(eInt &intgr, Parameter &param, QWidget *parent=nullptr);
    eParamTrackEdit(eU8 &byte, Parameter &param, QWidget *parent=nullptr);

    void setIndicatorColor(const QColor &indCol);
    void setIndicatorEnabled(eBool indEnabled);

    virtual void paintEvent(QPaintEvent *pe);

Q_SIGNALS:
    void onParameterChanged();
private Q_SLOTS:
    void _onValueChanged();

private:
    QString m_text;
    QColor m_indCol;
    bool m_indEnabled;
};


//------------------------------------------------------------------------------------------------------------------------------
// push button
//------------------------------------------------------------------------------------------------------------------------------
class eButton : public QPushButton
{
    Q_OBJECT
public:
    eButton(Parameter &param, QWidget *parent=nullptr);
Q_SIGNALS:
    void onParameterChanged(const Parameter &param);
private Q_SLOTS:
    void _onClicked();
private:
    Parameter& m_param;
};


//------------------------------------------------------------------------------------------------------------------------------
// boolean button
//------------------------------------------------------------------------------------------------------------------------------
class eBoolButton : public QToolButton
{
    Q_OBJECT
public:
    eBoolButton(Parameter &param, QWidget *parent=nullptr);
Q_SIGNALS:
    void onParameterChanged(const Parameter &param);
private Q_SLOTS:
    void _updateCaption();
    void _onClicked();
private:
    Parameter& m_param;
};

//------------------------------------------------------------------------------------------------------------------------------
// combo box
//------------------------------------------------------------------------------------------------------------------------------
class eComboBox : public QComboBox
{
    Q_OBJECT
public:
    eComboBox(Parameter &param, QWidget *parent=nullptr);
Q_SIGNALS:
    void onParameterChanged(const Parameter &param);
private Q_SLOTS:
    void _updateSelection();
    void _onActivated(int index);
private:
    Parameter& m_param;
};

//------------------------------------------------------------------------------------------------------------------------------
// flag button
//------------------------------------------------------------------------------------------------------------------------------
class eFlagButton : public QToolButton
{
    Q_OBJECT
public:
    eFlagButton(Parameter &param, const QString &caption, eU32 flagIndex, QWidget *parent=nullptr);
Q_SIGNALS:
    void onParameterChanged(const Parameter &param);
private Q_SLOTS:
    void _updateDownState();
    void _onClicked();
private:
    Parameter& m_param;
    eU32 m_flagIndex;
};

//------------------------------------------------------------------------------------------------------------------------------
// text edit (multi-lines)
//------------------------------------------------------------------------------------------------------------------------------
class eTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    eTextEdit(Parameter &param, QWidget *parent=nullptr);
Q_SIGNALS:
    void onParameterChanged(const Parameter &param);
private Q_SLOTS:
    void _onTextChanged();
private:
    Parameter& m_param;
};

//------------------------------------------------------------------------------------------------------------------------------
// line edit (single-line)
//------------------------------------------------------------------------------------------------------------------------------
class eLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    eLineEdit(Parameter &param, QWidget *parent=nullptr);
Q_SIGNALS:
    void onParameterChanged(const Parameter &param);
private Q_SLOTS:
    void _updateCaption();
    void _onTextChanged(const QString &text);
private:
    Parameter& m_param;
};


//------------------------------------------------------------------------------------------------------------------------------
// file frame with line edit + button for open file dialog
//------------------------------------------------------------------------------------------------------------------------------
class eFileFrame : public QWidget
{
    Q_OBJECT
public:
    eFileFrame(Parameter &param, QWidget *parent=nullptr);
    virtual ~eFileFrame();
Q_SIGNALS:
    void onParameterChanged(const Parameter &param);
private Q_SLOTS:
    void _updateCaption();
    void _onSelectLocally();
private:
    Parameter& m_param;
    eLineEdit* m_lineEdit;
};


//------------------------------------------------------------------------------------------------------------------------------
// color frame
//------------------------------------------------------------------------------------------------------------------------------
class eColorFrame : public QWidget
{
    Q_OBJECT
public:
    eColorFrame(Parameter &param, QWidget *parent=nullptr);
    virtual ~eColorFrame();
private:
    virtual void timerEvent(QTimerEvent *te);
Q_SIGNALS:
    void onParameterChanged();
private Q_SLOTS:
    void _onSelectLocally();
    void _updateEditColors();
private:
    Parameter& m_param;
    eParamTrackEdit*  m_edits[4];
};


#endif // PARAM_WIDGETS_HPP
