#ifndef TRACK_EDIT_HPP
#define TRACK_EDIT_HPP
#pragma once

#include <QtWidgets/QLineEdit>
#include "Types.h"

// basic track edit for floats, integer, and bytes
class eTrackEdit : public QLineEdit
{
    Q_OBJECT

public:
    eTrackEdit(eF32 &flt, eF32 min, eF32 max, QWidget *parent);
    eTrackEdit(eInt &intgr, eInt min, eInt max, QWidget *parent);
    eTrackEdit(eU8 &byte, eU8 min, eU8 max, QWidget *parent);
    virtual ~eTrackEdit();

    virtual void        wheelEvent(QWheelEvent *we);
    virtual void        keyPressEvent(QKeyEvent *ke);
    virtual void        keyReleaseEvent(QKeyEvent *ke);
    virtual void        mouseMoveEvent(QMouseEvent *me);
    virtual void        mousePressEvent(QMouseEvent *me);
    virtual void        mouseReleaseEvent(QMouseEvent *me);
    virtual void        timerEvent(QTimerEvent *te);
    virtual void        focusInEvent(QFocusEvent *fe);
    virtual void        focusOutEvent(QFocusEvent *fe);

Q_SIGNALS:
    void                onValueChanged();

private Q_SLOTS:
    void                _onTextEdited(const QString &text);

private:
    eBool               _changeValue(eF32 factor);

public:
    static const eInt   FLOAT_PRECISION = 3;

private:
    eF32                m_curFlt;
    eInt                m_curInt;
    eU8                 m_curByte;
    eF32 *              m_refFlt;
    eInt *              m_refInt;
    eU8 *               m_refByte;
    eBool               m_ctrlDown;
    eBool               m_shiftDown;
    eBool               m_valChanged;
    QPoint              m_mouseDownPos;
    eInt                m_timerId;
};

#endif // TRACK_EDIT_HPP
