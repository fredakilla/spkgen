#include "CommentGraphicsItem.h"

#include <QTextOption>
#include <QAbstractTextDocumentLayout>
#include <QGraphicsBlurEffect>
#include <QCursor>
#include <QPainter>
#include <QPixmapCache>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QColorDialog>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

#define COMMENT_MODE_TEXT 0
#define COMMENT_MODE_HTML 1

#define Z_LAYER_COMMENT    -10
#define Z_LAYER_SELECTED    10


//-----------------------------------------------------------------------------------------
// constructor
//-----------------------------------------------------------------------------------------
eCommentItem::eCommentItem(QGraphicsItem* parent) :
    QGraphicsTextItem(parent)
{
    m_realText = "";
    m_mode = COMMENT_MODE_TEXT;
    m_colorBack = QColor(200,200,200,255);
    m_colorText = QColor(0,0,0);
    m_isResizing = eFALSE;
    m_edited = eFALSE;

    // set tab width
    QTextOption opt = document()->defaultTextOption();
    opt.setTabStop(10);
    document()->setDefaultTextOption(opt);

    setFont(QFont("Helvetica", 8, QFont::Normal));

    _init();

    // set init size
    m_width = 124;
    m_height = 35;

    // connect doc size change to update size
    connect(document()->documentLayout(), SIGNAL(documentSizeChanged(QSizeF)), this, SLOT(_onUpdateSize(QSizeF)));

    // add a shadow effect
    QGraphicsDropShadowEffect * effect = new QGraphicsDropShadowEffect ();
    effect->setBlurRadius(15);
    effect->setColor(QColor(0,0,0,150));
    effect->setOffset(3,3);
    setGraphicsEffect(effect);
}

//-----------------------------------------------------------------------------------------
// update size
//-----------------------------------------------------------------------------------------
void eCommentItem::_onUpdateSize(const QSizeF newSize)
{
    if(m_width < newSize.width())
        m_width = newSize.width();

    if(m_height < newSize.height())
        m_height = newSize.height();
}

//-----------------------------------------------------------------------------------------
// get bounding rect
//-----------------------------------------------------------------------------------------
QRectF eCommentItem::boundingRect() const
{
    return QRectF(0,0,m_width, m_height);
}

//-----------------------------------------------------------------------------------------
// get shape
//-----------------------------------------------------------------------------------------
QPainterPath eCommentItem::shape() const
{
    QPainterPath path;
    path.addRect(QRectF(0,0,m_width,m_height));
    return path;
}

//-----------------------------------------------------------------------------------------
// initialisation
//-----------------------------------------------------------------------------------------
void eCommentItem::_init()
{
    setTextInteractionFlags(Qt::NoTextInteraction);
    setFlags( QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable);
    document()->setDocumentMargin(10);
    setDefaultTextColor(m_colorText);
    setPlainText(m_realText);
    setZValue(Z_LAYER_COMMENT);
}

//-----------------------------------------------------------------------------------------
// format ouput text or html
//-----------------------------------------------------------------------------------------
void eCommentItem::_formatOutput()
{
    if(m_edited)
        m_realText = toPlainText();

    if(m_mode == COMMENT_MODE_HTML)
    {
        setHtml( m_realText );
    }
    else
    {
        setPlainText( m_realText );
    }

    setCursor(Qt::ArrowCursor);
    setTextInteractionFlags(Qt::NoTextInteraction);

    // clear existing selection before exit edit mode
    QTextCursor c = textCursor();
    c.clearSelection();
    setTextCursor(c);

    m_edited = eFALSE;
}

//-----------------------------------------------------------------------------------------
// paint item
//-----------------------------------------------------------------------------------------
void eCommentItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);

    if(m_edited)
    {
        setDefaultTextColor(QColor(0,0,0));
        painter->setBrush(QBrush(QColor(255,255,255)));
    }
    else
    {
        setDefaultTextColor(m_colorText);
        painter->setBrush(QBrush(m_colorBack));
    }

    painter->setPen( QPen(QColor(10,10,10),2) );
    painter->drawRoundedRect(boundingRect(),5,5);
    QGraphicsTextItem::paint(painter, option, widget);
}

void eCommentItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *me)
{
    // enter in edit mode
    if (me->button() == Qt::LeftButton)
    {
        setTextInteractionFlags(Qt::TextEditorInteraction);
        setCursor(Qt::IBeamCursor);
        setFocus();

        // clear any html formating
        document()->setHtml("");

        m_edited = eTRUE;
        setPlainText(m_realText);
    }
}



//-----------------------------------------------------------------------------------------
// mousePressEvent
//-----------------------------------------------------------------------------------------
void eCommentItem::mousePressEvent(QGraphicsSceneMouseEvent *me)
{
    setZValue(Z_LAYER_SELECTED);

    // if click inside the resize area, enable resizing
    if (me->button() == Qt::LeftButton && (me->pos().x() >= boundingRect().width()-RESIZE_AREA || me->pos().y() >= boundingRect().height()-RESIZE_AREA)  )
    {
        m_isResizing = eTRUE;
        m_mouseDownPos = me->pos();
        me->accept();
        QGraphicsTextItem::mousePressEvent(me);
        return;
    }

    // enter in edit mode
    if (me->button() == Qt::LeftButton && me->modifiers() == Qt::ControlModifier)
    {
        setTextInteractionFlags(Qt::TextEditorInteraction);
        setCursor(Qt::IBeamCursor);
        setFocus();

        // clear any html formating
        document()->setHtml("");

        m_edited = eTRUE;
        setPlainText(m_realText);
    }

    // contextual menu
    if (me->button() == Qt::RightButton)
    {
        QMenu contextMenu;
        contextMenu.setObjectName("stackContextMenu");

        QAction * actionHtml =  contextMenu.addAction("html");
        QAction * actionText = contextMenu.addAction("text");
        contextMenu.addSeparator();
        contextMenu.addAction("color");

        if(m_mode == COMMENT_MODE_TEXT)
        {
            actionText->setCheckable(true);
            actionText->setChecked(true);
        }
        else
        {
            actionHtml->setCheckable(true);
            actionHtml->setChecked(true);
        }

        QPointF pos = me->screenPos();
        QAction* selectedItem = contextMenu.exec( pos.toPoint() );

        if (selectedItem)
        {
            if(selectedItem->text() == "html")
            {
                m_mode = COMMENT_MODE_HTML;
            }

            if(selectedItem->text() == "text")
            {
                m_mode = COMMENT_MODE_TEXT;
            }

            if(selectedItem->text() == "color")
            {
                QColorDialog cd;
                m_colorBack = cd.getColor(m_colorBack.toRgb(),nullptr,"Comment color", QColorDialog::ShowAlphaChannel);

                // auto adjust text color contrast according background color
                m_colorText = ( m_colorBack.rgb() > 0xFF808080) ? Qt::black : Qt::white;
                setDefaultTextColor (m_colorText);
            }

            _formatOutput();

        }
        else
        {
            // nothing was chosen
            //qDebug() << "context menu canceled";
        }
    }

    QGraphicsTextItem::mousePressEvent(me);

}

//---------------------------------------------------------------------------------------------
// mouse move event
//---------------------------------------------------------------------------------------------
void eCommentItem::mouseMoveEvent(QGraphicsSceneMouseEvent *me)
{
    if(m_isResizing)
    {
        qreal diffx = me->pos().x() - m_mouseDownPos.x();
        qreal diffy = me->pos().y() - m_mouseDownPos.y();

        if(abs((float)diffx) < 1 && abs((float)diffy) < 1)
            return;

        prepareGeometryChange();
        m_width = m_width + diffx;
        m_height = m_height + diffy ;

        m_mouseDownPos = me->pos();

        me->accept();
    }
    else
    {
        me->ignore();
        QGraphicsTextItem::mouseMoveEvent(me);
    }
}

//---------------------------------------------------------------------------------------------
// mouse release event
//---------------------------------------------------------------------------------------------
void eCommentItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *me)
{
    m_isResizing = false;
    setCursor(Qt::ArrowCursor);
    QGraphicsTextItem::mouseReleaseEvent(me);
}

//-----------------------------------------------------------------------------------------
// leave focus, switch to non-edited mode
//-----------------------------------------------------------------------------------------
void eCommentItem::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);

    _formatOutput();
    setZValue(Z_LAYER_COMMENT);
}

//-----------------------------------------------------------------------------------------
// hover enter event
//-----------------------------------------------------------------------------------------
void eCommentItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if(m_isResizing)
    {
        event->ignore();
        QGraphicsTextItem::hoverEnterEvent(event);
        return;
    }

    //setZValue(Z_LAYER_SELECTED);
}

//-----------------------------------------------------------------------------------------
// hover move event
//-----------------------------------------------------------------------------------------
void eCommentItem::hoverMoveEvent(QGraphicsSceneHoverEvent * me)
{
    if ( me->pos().x() >= boundingRect().width()-RESIZE_AREA && me->pos().y() >= boundingRect().height()-RESIZE_AREA )
        setCursor(Qt::SizeFDiagCursor);
    else if (me->pos().x() >= boundingRect().width()-RESIZE_AREA )
        setCursor(Qt::SizeHorCursor);
    else if (me->pos().y() >= boundingRect().height()-RESIZE_AREA )
        setCursor(Qt::SizeVerCursor);
    else
        setCursor(Qt::ArrowCursor);

}

//-----------------------------------------------------------------------------------------
// hover leave event
//-----------------------------------------------------------------------------------------
void eCommentItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);

    setZValue(Z_LAYER_COMMENT);
}



QJsonObject eCommentItem::save() const
{
    QJsonObject nodeJson;

    nodeJson["mode"] = m_mode;
    nodeJson["text"] = m_realText.toStdString().c_str();
    nodeJson["backcolor"] = m_colorBack.name();
    nodeJson["textcolor"] = m_colorText.name();
    nodeJson["width"] = m_width;
    nodeJson["height"] = m_height;

    QJsonObject objpos;
    objpos["x"] = pos().x();
    objpos["y"] = pos().y();
    nodeJson["position"] = objpos;

    return nodeJson;
}

void eCommentItem::restore(QJsonObject const& json)
{
    m_mode = json["mode"].toInt();
    m_realText = json["text"].toString();
    m_colorBack.setNamedColor(json["backcolor"].toString());
    m_colorText.setNamedColor(json["textcolor"].toString());
    m_width = json["width"].toDouble();
    m_height = json["height"].toDouble();

    QJsonObject positionJson = json["position"].toObject();
    QPointF point(positionJson["x"].toDouble(), positionJson["y"].toDouble());
    setPos(point);

    _formatOutput();
}

/*
void eCommentItem::saveToXml(QDomElement &node) const
{
    QDomDocument xml = node.ownerDocument();
    QDomElement opEl = xml.createElement("comment");
    opEl.setAttribute("mode", m_mode);
    opEl.setAttribute("backcolor", m_colorBack.name());
    opEl.setAttribute("textcolor", m_colorText.name());
    opEl.setAttribute("width", m_width);
    opEl.setAttribute("height", m_height);
    opEl.setAttribute("xpos", pos().x());
    opEl.setAttribute("ypos", pos().y());
    opEl.setAttribute("text", m_realText.toStdString().c_str());
    node.appendChild(opEl);
}

void eCommentItem::loadFromXml(const QDomElement &node)
{
    m_mode = node.attribute("mode").toInt();
    m_width = node.attribute("width").toFloat();
    m_height = node.attribute("height").toFloat();
    m_realText = QString(node.attribute("text"));
    m_colorBack.setNamedColor(node.attribute("backcolor"));
    m_colorText.setNamedColor(node.attribute("textcolor"));

    _formatOutput();
}
*/


