#ifndef PAGELIST_H
#define PAGELIST_H

#include <QListWidget>

#include "node-editor/common/CustomFlowScene.h"


struct Page
{
    CustomFlowScene* flowScene;
};


class PageList : public QListWidget
{
    Q_OBJECT

public:
    PageList(QWidget* parent);
    QSize sizeHint() const override;
    void mousePressEvent(QMouseEvent* event) override;
    void load(const QJsonObject &json);
    void save(QJsonObject &json);

Q_SIGNALS:
    void signalPageAdded(Page* page);
    void signalPageSwitch(Page* page);

public Q_SLOTS:
    void onNewProject();
    void onAddPage();
    void onRemovePage();
    void onItemChanged(QListWidgetItem *item);
    void onSelectionChanged();
    void onSortByName();

private:
    void _createActions();
    QListWidgetItem* _addPage(Page *page, QListWidgetItem *parent=nullptr);
    void _clearAll();

    std::shared_ptr<DataModelRegistry> _sparkNodesRegistry;
    QList<Page*> _nodeFlowScenes;

};

#endif // PAGELIST
