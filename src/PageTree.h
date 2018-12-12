#ifndef PAGETREE_H
#define PAGETREE_H

#include <QTreeWidget>

#include "node-editor/common/CustomFlowScene.h"


struct Page
{
    CustomFlowScene* flowScene;
};


class PageTree : public QTreeWidget
{
    Q_OBJECT

public:
    PageTree(QWidget* parent);
    QSize sizeHint() const override;
    void mousePressEvent(QMouseEvent* event) override;
    void load(const QJsonObject &json);
    void save(QJsonObject &json);

Q_SIGNALS:
    void signalPageAdded(Page* page);
    void signalPageSwitch(Page* page);

public Q_SLOTS:
    void onAddPage();
    void onRemovePage();
    void onItemChanged(QTreeWidgetItem *item, int column);
    void onSelectionChanged();
    void onSortByName();
    void onRenamePage();

private:
    void _createActions();
    QTreeWidgetItem* _addPage(Page *page, QTreeWidgetItem *parent=nullptr);

    std::shared_ptr<DataModelRegistry> _sparkNodesRegistry;
    QList<Page*> _nodeFlowScenes;

};

#endif // PAGETREE_H
