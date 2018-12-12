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
class NodePath4;

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

    void save(QJsonObject &json) const;
    void load(const QJsonObject &json);
    void clearComments();

    QString getName() { return _name; }
    void setName(QString name) { _name = name; }

Q_SIGNALS:
    void signalShowPathNode(NodePath*);
    void signalShowPath4Node(NodePath4*);

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
    QString _name;
};


#endif // CUSTOMFLOWSCENE_H
