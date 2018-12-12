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

#include "../spark-nodes/spark-nodes.h"
#include "../../UrhoDevice.h"


CustomFlowScene::CustomFlowScene(std::shared_ptr<DataModelRegistry> registry, QObject * parent) :
    FlowScene(registry, parent)
  , _name("New page")
{
    _initialize();
}

CustomFlowScene::CustomFlowScene(QObject * parent) :
    FlowScene(parent)
{
    _initialize();
}

CustomFlowScene::~CustomFlowScene()
{
    delete _commentLayer;
}

void CustomFlowScene::_initialize()
{
    _mousePos = QPoint(0,0);
    _commentLayer = new QGraphicsItemLayer();

    connect(this, &FlowScene::nodeDoubleClicked, this, &CustomFlowScene::showNode);
    connect(this, &FlowScene::nodeCreated, this, &CustomFlowScene::initNode);
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

void CustomFlowScene::showNode(QtNodes::Node& node)
{
    // is a system node ?
    NodeSparkSystem* systemNode = dynamic_cast<NodeSparkSystem*>(node.nodeDataModel());
    if(systemNode)
    {
        if(systemNode->getResult().get() != nullptr)
        {
           UrhoDevice::getInstance()->setCurentParticleSystem(systemNode->getResult());
        }

        return;
    }

    // is a Path node ?
    NodePath* pathNode = dynamic_cast<NodePath*>(node.nodeDataModel());
    if(pathNode)
    {
        if(pathNode->getResult() != nullptr)
        {
            Q_EMIT signalShowPathNode(pathNode);
        }
        return;
    }

    // is a Path4 node ?
    NodePath4* path4Node = dynamic_cast<NodePath4*>(node.nodeDataModel());
    if(path4Node)
    {
        if(path4Node->getResult() != nullptr)
        {
            Q_EMIT signalShowPath4Node(path4Node);
        }
        return;
    }
}

void CustomFlowScene::initNode(QtNodes::Node& node)
{
    BaseNode* baseNode = dynamic_cast<BaseNode*>(node.nodeDataModel());
    baseNode->init();
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

void CustomFlowScene::save(QJsonObject &json) const
{
    // nodes and connections
    QByteArray data = saveToMemory();
    json = QJsonDocument::fromJson(data).object();

    // page name
    json["Name"] = _name;

    // comments
    QJsonArray commentsJsonArray;
    for (auto const & commentItem : _commentList)
    {
        commentsJsonArray.append(commentItem->save());
    }
    json["comments"] = commentsJsonArray;

}

void CustomFlowScene::load(const QJsonObject& json)
{
    clearScene();
    clearComments();

    // page name
    _name = json["Name"].toString();

    // nodes
    QJsonArray nodesJsonArray = json["nodes"].toArray();
    for (QJsonValueRef node : nodesJsonArray)
    {
        restoreNode(node.toObject());
    }

    // connections
    QJsonArray connectionJsonArray = json["connections"].toArray();
    for (QJsonValueRef connection : connectionJsonArray)
    {
        restoreConnection(connection.toObject());
    }

    // comments
    QJsonArray commentsJsonArray = json["comments"].toArray();
    for (int i=0; i<commentsJsonArray.size(); ++i)
    {
        eCommentItem* comment = _addComment(QPointF(0,0));
        comment->restore(commentsJsonArray[i].toObject());
    }
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
