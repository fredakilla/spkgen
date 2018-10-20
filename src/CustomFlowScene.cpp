#include "CustomFlowScene.h"
#include "CommentGraphicsItem.h"

#include <QAction>
#include <QMouseEvent>
#include <QFileDialog>
#include <QByteArray>
#include <QBuffer>
#include <QDataStream>
#include <QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QDebug>


CustomFlowScene::CustomFlowScene(std::shared_ptr<DataModelRegistry> registry, QObject * parent) :
    FlowScene(registry, parent)
{
    _mousePos = QPoint(0,0);
    _commentLayer = new QGraphicsItemLayer();
}

CustomFlowScene::CustomFlowScene(QObject * parent) :
    FlowScene(parent)
{
    _mousePos = QPoint(0,0);
    _commentLayer = new QGraphicsItemLayer();
}

CustomFlowScene::~CustomFlowScene()
{

}

void CustomFlowScene::keyPressEvent(QKeyEvent *event)
{
    // if a comment is being editing bypass event
    Q_FOREACH(eCommentItem* ci, _commentList)
    {
        if(ci->isEdited())
        {
            FlowScene::keyPressEvent(event);
            return;
        }
    }

    switch (event->key())
    {
    case Qt::Key_O:
        _addComment(_mousePos);
        break;
    }

    FlowScene::keyPressEvent(event);
}

void CustomFlowScene::keyReleaseEvent(QKeyEvent *event)
{
    FlowScene::keyReleaseEvent(event);
}

void CustomFlowScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    _mousePos = event->scenePos();
    FlowScene::mouseMoveEvent(event);
}

eCommentItem* CustomFlowScene::_addComment(const QPointF &pos)
{
    eCommentItem* comment = new eCommentItem(_commentLayer);
    comment->setPos(_mousePos);
    addItem(comment);

    _commentList.append(comment);

    return comment;
}

void CustomFlowScene::deleteSelectedComments()
{
    for (QGraphicsItem* item : selectedItems())
    {
        if (auto c = qgraphicsitem_cast<eCommentItem*>(item))
        {
            removeItem(c);
            _commentList.removeAll(c);
            delete c;
        }
    }
}

void CustomFlowScene::save() const
{
    // original code from FlowScene
    // modified to append comments to json

    QString fileName = QFileDialog::getSaveFileName(nullptr,
                                         tr("Open Flow Scene"),
                                         QDir::homePath(),
                                         tr("Flow Scene Files (*.flow)"));

    if (!fileName.isEmpty())
    {
        if (!fileName.endsWith("flow", Qt::CaseInsensitive))
            fileName += ".flow";

        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly))
        {
            // get nodes and connections data
            QByteArray data = saveToMemory();

            // intercept json result and append comments
            QJsonObject json = QJsonDocument::fromJson(data).object();
            saveCommentsToJson(json);

            // finally write all in file
            QJsonDocument document(json);
            QByteArray dataWithComments = document.toJson();
            file.write(dataWithComments);
        }
    }
}

void CustomFlowScene::load()
{
    // original code from FlowScene

    QString fileName = QFileDialog::getOpenFileName(nullptr,
                                         tr("Open Flow Scene"),
                                         QDir::homePath(),
                                         tr("Flow Scene Files (*.flow)"));

    if (!QFileInfo::exists(fileName))
        return;

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
        return;

    clearScene();       // clear scene here, not like original.
    clearComments();

    QByteArray wholeFile = file.readAll();

    loadFromMemory(wholeFile);

    // additionnal code to load comments
    loadCommentsFromMemory(wholeFile);
}

void CustomFlowScene::clearComments()
{
    Q_FOREACH(eCommentItem * i, _commentList)
    {
        removeItem(i);
        delete i;
    }
    _commentList.clear();
}

void CustomFlowScene::saveCommentsToJson(QJsonObject &json) const
{
    QJsonArray commentsJsonArray;
    for (auto const & commentItem : _commentList)
    {
        commentsJsonArray.append(commentItem->save());
    }
    json["comments"] = commentsJsonArray;
}

void CustomFlowScene::loadCommentsFromMemory(const QByteArray& data)
{
    QJsonObject const jsonDocument = QJsonDocument::fromJson(data).object();

    // check for errors
    //QJsonParseError jsonError;
    //QJsonDocument configJsonDoc = QJsonDocument::fromJson(data, &jsonError);
    //if( jsonError.error != QJsonParseError::NoError )
    //     qDebug() << QString("Json error: %1").arg(jsonError.errorString());
    //else if( configJsonDoc .isNull() )
    //     qDebug() << "Null JsonDocument";

    QJsonArray commentsJsonArray = jsonDocument["comments"].toArray();
    for (int i=0; i<commentsJsonArray.size(); ++i)
    {
        eCommentItem* comment = _addComment(QPointF(0,0));
        comment->restore(commentsJsonArray[i].toObject());
    }
}
