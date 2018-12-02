#ifndef CUSTOMFLOWSCENE_H
#define CUSTOMFLOWSCENE_H

#include <nodes/Node>
#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <QGraphicsSceneMouseEvent>
#include "CommentGraphicsItem.h"

using QtNodes::Node;
using QtNodes::FlowView;
using QtNodes::FlowScene;
using QtNodes::DataModelRegistry;

class NodePath;

// empty graphic item used as parent container for comments items
class QGraphicsItemLayer : public QGraphicsItem
{
public:
    virtual QRectF boundingRect() const override
    {
        return QRectF(0,0,0,0);
    }

    virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) override
    {
    }
};



class CustomFlowScene : public FlowScene
{
    Q_OBJECT
public:
    CustomFlowScene(std::shared_ptr<DataModelRegistry> registry, QObject * parent = Q_NULLPTR);
    CustomFlowScene(QObject * parent = Q_NULLPTR);
    ~CustomFlowScene();

    void save() const;
    void load();
    void clearComments();
    void saveCommentsToJson(QJsonObject &json) const;
    void loadCommentsFromMemory(const QByteArray& data);

Q_SIGNALS:
    void showPathNodeRequest(NodePath*);

public Q_SLOTS:
    void deleteSelectedComments();

private Q_SLOTS:
    void showNode(Node& node);
    void initNode(QtNodes::Node& node);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

private:
    void _initialize();
    eCommentItem* _addComment(const QPointF &pos);

private:
    QGraphicsItemLayer* _commentLayer;
    QPointF _mousePos;
    QList<eCommentItem*> _commentList;
};


#endif // CUSTOMFLOWSCENE_H
