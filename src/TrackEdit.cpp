#include <QtGui/QDoubleValidator>
#include <QtGui/QWheelEvent>

#include "Trackedit.h"

eTrackEdit::eTrackEdit(eF32 &flt, eF32 min, eF32 max, QWidget *parent) : QLineEdit(parent),
    m_curFlt(flt),
    m_refFlt(&flt),
    m_refInt(nullptr),
    m_refByte(nullptr),
    m_ctrlDown(eFALSE),
    m_shiftDown(eFALSE),
    m_valChanged(eFALSE)
{
    QDoubleValidator *dv = new QDoubleValidator(min, max, 3, this);
    dv->setNotation(QDoubleValidator::StandardNotation);
    dv->setLocale(QLocale::C);
    setValidator(dv);
    setText(QString::number(m_curFlt, 'f', FLOAT_PRECISION));
    connect(this, SIGNAL(textEdited(const QString &)), this, SLOT(_onTextEdited(const QString &)));
    m_timerId = startTimer(100);
}

eTrackEdit::eTrackEdit(eInt &intgr, eInt min, eInt max, QWidget *parent) : QLineEdit(parent),
    m_curInt(intgr),
    m_refFlt(nullptr),
    m_refInt(&intgr),
    m_refByte(nullptr),
    m_ctrlDown(eFALSE),
    m_shiftDown(eFALSE),
    m_valChanged(eFALSE)
{
    setValidator(new QIntValidator(min, max, this));
    setText(eIntToStr(m_curInt));
    connect(this, SIGNAL(textEdited(const QString &)), this, SLOT(_onTextEdited(const QString &)));
    m_timerId = startTimer(100);
}

eTrackEdit::eTrackEdit(eU8 &byte, eU8 min, eU8 max, QWidget *parent) : QLineEdit(parent),
    m_curByte(byte),
    m_refFlt(nullptr),
    m_refInt(nullptr),
    m_refByte(&byte),
    m_ctrlDown(eFALSE),
    m_shiftDown(eFALSE),
    m_valChanged(eFALSE)
{
    setValidator(new QIntValidator(min, max, this));
    setText(eIntToStr(m_curByte));
    connect(this, SIGNAL(textEdited(const QString &)), this, SLOT(_onTextEdited(const QString &)));
    m_timerId = startTimer(100);
}

eTrackEdit::~eTrackEdit()
{
    killTimer(m_timerId);
}

void eTrackEdit::wheelEvent(QWheelEvent *we)
{
    if (hasFocus())
    {
        _changeValue(eSign(we->delta()));
        we->accept();
    }
    else
        QLineEdit::wheelEvent(we);
}

void eTrackEdit::keyPressEvent(QKeyEvent *ke)
{
    QLineEdit::keyPressEvent(ke);

    if (ke->key() == Qt::Key_Control)
        m_ctrlDown = eTRUE;
    else if (ke->key() == Qt::Key_Shift)
        m_shiftDown = eTRUE;
}

void eTrackEdit::keyReleaseEvent(QKeyEvent *ke)
{
    QLineEdit::keyReleaseEvent(ke);
    m_ctrlDown = eFALSE;
    m_shiftDown = eFALSE;
}

void eTrackEdit::mouseMoveEvent(QMouseEvent *me)
{
    //@@eFUNCDELAY(25U);

    QLineEdit::mouseMoveEvent(me);

    if (me->buttons()&Qt::RightButton)
    {
        const QPoint diff = me->pos()-m_mouseDownPos;

        if (eAbs(diff.x()) > 1 && _changeValue(diff.x()/2))
        {
            QCursor::setPos(mapToGlobal(m_mouseDownPos));
            m_valChanged = eTRUE;
        }
    }
}

void eTrackEdit::mousePressEvent(QMouseEvent *me)
{
    m_mouseDownPos = me->pos();
    setContextMenuPolicy(Qt::PreventContextMenu);
    QLineEdit::mousePressEvent(me);
}

void eTrackEdit::mouseReleaseEvent(QMouseEvent *me)
{
    QLineEdit::mouseReleaseEvent(me);

    if (m_valChanged)
        m_valChanged = eFALSE;
    else if (me->button() == Qt::RightButton)
    {
        QContextMenuEvent ce(QContextMenuEvent::Mouse, me->pos());
        setContextMenuPolicy(Qt::DefaultContextMenu);
        contextMenuEvent(&ce);
    }
}

void eTrackEdit::timerEvent(QTimerEvent *te)
{
    QLineEdit::timerEvent(te);

    if (m_refFlt && !eAreFloatsEqual(m_curFlt, *m_refFlt))
    {
        m_curFlt = *m_refFlt;
        setText(QString::number(m_curFlt, 'f', FLOAT_PRECISION));
    }
    else if (m_refInt && m_curInt != *m_refInt)
    {
        m_curInt = *m_refInt;
        setText(eIntToStr(m_curInt));
    }
    else if (m_refByte && m_curByte != *m_refByte)
    {
        m_curByte = *m_refByte;
        setText(eIntToStr(m_curByte));
    }
}

void eTrackEdit::focusInEvent(QFocusEvent *fe)
{
    QLineEdit::focusInEvent(fe);
    setFocusPolicy(Qt::WheelFocus);
}

void eTrackEdit::focusOutEvent(QFocusEvent *fe)
{
    QLineEdit::focusOutEvent(fe);
    setFocusPolicy(Qt::StrongFocus);
}

void eTrackEdit::_onTextEdited(const QString &text)
{
    if (hasAcceptableInput())
    {
        if (m_refFlt)
        {
            m_curFlt = text.toFloat();
            *m_refFlt = m_curFlt;
        }
        else if (m_refInt)
        {
            m_curInt = text.toInt();
            *m_refInt = m_curInt;
        }
        else if (m_refByte)
        {
            m_curByte = text.toInt();
            *m_refByte = m_curByte;
        }

        Q_EMIT onValueChanged();
    }
}

eBool eTrackEdit::_changeValue(eF32 factor)
{
    if (m_ctrlDown) // [control] => change slow (0.1x)
        factor *= 0.1f; 
    else if (m_shiftDown) // [shift] => change fast (10x)
        factor *= 10.0f;

    // calculate new parameter value
    QString newText;
    eInt newInt = 0;
    eF32 newFloat = 0.0f;

    if (m_refFlt)
    {
        newFloat = m_curFlt+0.01f*factor;
        newText.setNum(newFloat, 'f', FLOAT_PRECISION);
    }
    else
    {
        // value to small for change?
        if (eAbs(factor) < 1.0f)
            return eFALSE;

        newInt = (m_refInt ? m_curInt : m_curByte)+(eInt)factor;
        newText.setNum(newInt);
    }

    // validate format and range
    eInt pos;
    if (validator()->validate(newText, pos) == QValidator::Acceptable)
    {
        if (m_refFlt)
        {
            m_curFlt = newFloat;
            *m_refFlt = newFloat;
        }
        else if (m_refInt)
        {
            m_curInt = newInt;
            *m_refInt = newInt;
        }
        else if (m_refByte)
        {
            m_curByte = newInt;
            *m_refByte = newInt;
        }

        setText(newText);
    }

    Q_EMIT textEdited(text());
    return eTRUE;
}

