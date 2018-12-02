#include "CustomWidgets.h"
#include "Color.h"

#include <QtGui/QAbstractTextDocumentLayout>
#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QIntValidator>
#include <QtGui/QPainter>

#include <QtWidgets/QWidgetAction>
#include <QtWidgets/QColorDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QMenu>
#include <QtWidgets/QFileDialog>



//------------------------------------------------------------------------------------------------------------------------------
// eParamTrackEdit2
//------------------------------------------------------------------------------------------------------------------------------

eParamTrackEdit::eParamTrackEdit(eF32 &flt, Parameter &param, QWidget *parent) :
    eTrackEdit(flt, param.min, param.max, parent),
    m_indEnabled(false)
{
    connect(this, SIGNAL(onValueChanged()), this, SLOT(_onValueChanged()));
}

eParamTrackEdit::eParamTrackEdit(eInt &intgr, Parameter &param, QWidget *parent) :
    eTrackEdit(intgr, param.min, param.max, parent),
    m_indEnabled(false)
{
    connect(this, SIGNAL(onValueChanged()), this, SLOT(_onValueChanged()));
}

eParamTrackEdit::eParamTrackEdit(eU8 &byte, Parameter &param, QWidget *parent) :
    eTrackEdit(byte, param.min, param.max, parent),
    m_indEnabled(false)
{
    connect(this, SIGNAL(onValueChanged()), this, SLOT(_onValueChanged()));
}

void eParamTrackEdit::_onValueChanged()
{
    Q_EMIT onParameterChanged();
}

void eParamTrackEdit::setIndicatorColor(const QColor &indCol)
{
    m_indCol = indCol;
    m_indEnabled = true;
    repaint();
}

void eParamTrackEdit::setIndicatorEnabled(eBool indEnabled)
{
    m_indEnabled = indEnabled;
    repaint();
}

float getBrightness(QColor color)
{
    return (color.red() * 299 + color.green() * 587 + color.blue() * 114) / 1000;
}

void eParamTrackEdit::paintEvent(QPaintEvent *pe)
{
    eTrackEdit::paintEvent(pe);

    QPainter p(this);

    if (m_indEnabled)
    {
        // background color
        p.setPen(m_indCol);
        p.setBrush(m_indCol);
        p.drawRect(4,4,width()-8,height()-8);

        // automatic foreground contrast from background
        QColor textColor = Qt::black;
        float brightness = getBrightness(m_indCol);
        if (brightness < 123)
            textColor = Qt::white;

        // foreground text
        p.setPen(textColor);
        p.drawText(rect().adjusted(0, 0, -4, 0), Qt::AlignVCenter|Qt::AlignRight, text());
    }
}

//------------------------------------------------------------------------------------------------------------------------------
// push button
//------------------------------------------------------------------------------------------------------------------------------
eButton::eButton(Parameter &param, QWidget *parent) : QPushButton(parent),
    m_param(param)
{
    Q_ASSERT(param.getType() == EPT_BUTTON);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setText(m_param.getValueAsString());
    connect(this, SIGNAL(clicked()), this, SLOT(_onClicked()));
    param.setUserData(0, (QObjectUserData *)this);
}

void eButton::_onClicked()
{
    //m_param.getBaseValue().get<bool>() = !m_param.getBaseValue().get<bool>();
    //m_param.setChanged();
    Q_EMIT onParameterChanged(m_param);
}



//------------------------------------------------------------------------------------------------------------------------------
// boolean button
//------------------------------------------------------------------------------------------------------------------------------

eBoolButton::eBoolButton(Parameter &param, QWidget *parent) : QToolButton(parent),
    m_param(param)
{
    Q_ASSERT(param.getType() == EPT_BOOL);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    _updateCaption();
    connect(this, SIGNAL(clicked()), this, SLOT(_onClicked()));
    connect(&m_param, &Parameter::parameterUpdated, this, &eBoolButton::_updateCaption);
}

void eBoolButton::_updateCaption()
{
    setText(m_param.getBaseValue().get<bool>() ? "Yes" : "No");
}

void eBoolButton::_onClicked()
{
    m_param.getBaseValue().get<bool>() = !m_param.getBaseValue().get<bool>();
    _updateCaption();
    m_param.setChanged();
    Q_EMIT onParameterChanged(m_param);
}


//------------------------------------------------------------------------------------------------------------------------------
// combo box
//------------------------------------------------------------------------------------------------------------------------------

eComboBox::eComboBox(Parameter &param, QWidget *parent) : QComboBox(parent),
    m_param(param)
{
    const QStringList descrItems = QString(param.getDescription()).split("|");
    Q_ASSERT(descrItems.size() > 0);
    Q_ASSERT(param.getValueAsEnum() < descrItems.size());

    for (eInt i=0; i<descrItems.size(); i++)
        addItem(descrItems[i]);

    setCurrentIndex(param.getValueAsEnum());
    connect(this, SIGNAL(activated(int)), this, SLOT(_onActivated(int)));
    connect(&m_param, &Parameter::parameterUpdated, this, &eComboBox::_updateSelection);
}

void eComboBox::_updateSelection()
{
    int index = m_param.getValueAsEnum();
    setCurrentIndex(index);
}

void eComboBox::_onActivated(int index)
{
    m_param.baseValue = (int)index;
    m_param.setChanged();
    Q_EMIT onParameterChanged(m_param);
}


//------------------------------------------------------------------------------------------------------------------------------
// flag button
//------------------------------------------------------------------------------------------------------------------------------

eFlagButton::eFlagButton(Parameter &param, const QString &caption, eU32 flagIndex, QWidget *parent) : QToolButton(parent),
    m_param(param),
    m_flagIndex(flagIndex)
{
    Q_ASSERT(param.getType() == EPT_FLAGS);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setText(caption);
    setCheckable(true);
    _updateDownState();

    connect(this, SIGNAL(clicked()), this, SLOT(_onClicked()));
    connect(&m_param, &Parameter::parameterUpdated, this, &eFlagButton::_updateDownState);
}

void eFlagButton::_updateDownState()
{
    setChecked(eGetBit(m_param.getValueAsFlags(), m_flagIndex));
}

void eFlagButton::_onClicked()
{
    unsigned char flag = m_param.getValueAsFlags();
    eToggleBit(flag, m_flagIndex);
    m_param.baseValue = (unsigned char)flag;

    _updateDownState();
    m_param.setChanged();
    Q_EMIT onParameterChanged(m_param);
}


//------------------------------------------------------------------------------------------------------------------------------
// text edit (multi-lines)
//------------------------------------------------------------------------------------------------------------------------------

eTextEdit::eTextEdit(Parameter &param, QWidget *parent) :
    m_param(param)
{
    Q_ASSERT(param.getType() == EPT_TEXT);

    setFixedHeight(135);
    setLineWrapMode(QTextEdit::NoWrap);
    setTabStopWidth(8);
    setPlainText(m_param.getValueAsString());

    connect(this, SIGNAL(textChanged()), this, SLOT(_onTextChanged()));
}

void eTextEdit::_onTextChanged()
{
    m_param.baseValue = toPlainText();
    m_param.setChanged();
    Q_EMIT onParameterChanged(m_param);
}


//------------------------------------------------------------------------------------------------------------------------------
// line edit (single-line)
//------------------------------------------------------------------------------------------------------------------------------

eLineEdit::eLineEdit(Parameter &param, QWidget *parent) :
    m_param(param)
{
    Q_ASSERT(param.getType() == EPT_STRING || param.getType() == EPT_FILE);

    setText(m_param.getValueAsString());

    connect(this, SIGNAL(textChanged(const QString &)), this, SLOT(_onTextChanged(const QString &)));
    connect(&m_param, &Parameter::parameterUpdated, this, &eLineEdit::_updateCaption);
}

void eLineEdit::_updateCaption()
{
    setText(m_param.getValueAsString());
}

void eLineEdit::_onTextChanged(const QString &text)
{
    m_param.baseValue = text;
    m_param.setChanged();
    Q_EMIT onParameterChanged(m_param);
}



//------------------------------------------------------------------------------------------------------------------------------
// file frame with line edit + button for open file dialog
//------------------------------------------------------------------------------------------------------------------------------

eFileFrame::eFileFrame(Parameter &param, QWidget *parent) : QWidget(parent),
    m_param(param)
{
    Q_ASSERT(param.getType() == EPT_FILE);

    QHBoxLayout *hbl = new QHBoxLayout;
    hbl->setContentsMargins(0, 0, 0, 0);

    m_lineEdit = new eLineEdit(param, this);
    hbl->addWidget(m_lineEdit);

    QToolButton *selBtn = new QToolButton(this);
    selBtn->setText("...");
    selBtn->setFixedWidth(25);
    hbl->addWidget(selBtn);
    connect(selBtn, SIGNAL(clicked()), this, SLOT(_onSelectLocally()));
    connect(&m_param, &Parameter::parameterUpdated, this, &eFileFrame::_updateCaption);


    setLayout(hbl);
}

eFileFrame::~eFileFrame()
{
}

void eFileFrame::_updateCaption()
{
    m_lineEdit->setText(m_param.getValueAsString());
}

void eFileFrame::_onSelectLocally()
{
    /*QFileDialog fileDialog(0, tr("Load File"), ".", tr("all files (*.*)"));
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);
    if (QDialog::Accepted != fileDialog.exec())
        return;
    const QString filename = fileDialog.selectedFiles().first();*/

    const QString filename = QFileDialog::getOpenFileName(0, tr("Load File"), ".", tr("all files (*.*)"));

    if(!filename.isEmpty())
    {
        QDir dir;
        QString relativePath = dir.relativeFilePath(filename);

        m_lineEdit->setText(relativePath);
        m_param.setChanged();
        Q_EMIT onParameterChanged(m_param);
    }
}

//------------------------------------------------------------------------------------------------------------------------------
// color frame
//------------------------------------------------------------------------------------------------------------------------------

eColorFrame::eColorFrame(Parameter &param, QWidget *parent) : QWidget(parent),
    m_param(param)
{
    Q_ASSERT(param.getType() == EPT_RGB || param.getType() == EPT_RGBA);

    QHBoxLayout *hbl = new QHBoxLayout;
    hbl->setContentsMargins(0, 0, 0, 0);

    for (eU32 i=0; i<param.getComponentCount(); i++)
    {
        eU8& var = param.getBaseValue().get<eColor>()[i];
        m_edits[i] = new eParamTrackEdit(var, param);
        hbl->addWidget(m_edits[i]);
        //connect(m_edits[i], SIGNAL(onParameterChanged(const Parameter &)), this, SLOT(_updateEditColors()));
        //connect(m_edits[i], SIGNAL(onParameterChanged(const Parameter &)), this, SIGNAL(onParameterChanged(const Parameter &)));
        connect(m_edits[i], &eParamTrackEdit::onParameterChanged, this, &eColorFrame::_updateEditColors);
        connect(m_edits[i], &eParamTrackEdit::onParameterChanged, this, &eColorFrame::onParameterChanged);
        connect(&m_param, &Parameter::parameterUpdated, this, &eColorFrame::_updateEditColors);
    }

    QToolButton *selBtn = new QToolButton(this);
    selBtn->setText("...");
    selBtn->setFixedWidth(25);
    hbl->addWidget(selBtn);
    connect(selBtn, SIGNAL(clicked()), this, SLOT(_onSelectLocally()));

    setLayout(hbl);
    _updateEditColors();
}

eColorFrame::~eColorFrame()
{
}

void eColorFrame::timerEvent(QTimerEvent *te)
{
    QWidget::timerEvent(te);
    _updateEditColors();
}

void eColorFrame::_onSelectLocally()
{
    eColor &pc = m_param.getBaseValue().get<eColor>();
    const QColor col = QColorDialog::getColor(QColor(pc.toArgb()));

    if (col.isValid())
    {
        pc.r = col.red();
        pc.g = col.green();
        pc.b = col.blue();

        m_edits[0]->setText(eIntToStr(pc.r));
        m_edits[1]->setText(eIntToStr(pc.g));
        m_edits[2]->setText(eIntToStr(pc.b));

        m_param.setChanged();
        _updateEditColors();
        Q_EMIT onParameterChanged();
    }
}

void eColorFrame::_updateEditColors()
{
    for (eU32 i=0; i<m_param.getComponentCount(); i++)
        m_edits[i]->setIndicatorColor(QColor(m_param.getValueAsColor().toArgb()));
}
