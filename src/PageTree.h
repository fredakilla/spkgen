#ifndef PAGETREE_H
#define PAGETREE_H

#include <QTreeWidget>

#include "node-editor/common/CustomFlowScene.h"


struct Page
{
    CustomFlowScene* flowScene;
    QString name;
};


class PageTree : public QTreeWidget
{
public:
    PageTree(QWidget* parent);
    QSize sizeHint() const;

Q_SIGNALS:
    /*void onPageAdd(eOperatorPage *&opPage);
    void onPageRemove(eOperatorPage *opPage);
    void onPageSwitch(eOperatorPage *opPage);
    void onPageRenamed(eOperatorPage *opPage);*/

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
