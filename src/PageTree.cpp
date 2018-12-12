#include "PageTree.h"
#include "node-editor/spark-nodes/SparkNodesRegistry.h"

#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMenu>
#include <QMouseEvent>

PageTree::PageTree(QWidget* parent) :
    QTreeWidget(parent)
{
    _sparkNodesRegistry = registerSparkNodesDataModels();

    setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);
    setDragDropMode(InternalMove);

    _createActions();

    connect(this, &PageTree::itemSelectionChanged, this, &PageTree::onSelectionChanged);
    connect(this, &PageTree::itemChanged, this, &PageTree::onItemChanged);
}

QSize PageTree::sizeHint() const
{
    return QSize(150, 0);
}

void PageTree::mousePressEvent(QMouseEvent* event)
{
    QModelIndex item = indexAt(event->pos());
    QTreeView::mousePressEvent(event);
    if ((item.row() == -1 && item.column() == -1))
    {
        clearSelection();
    }
}

QTreeWidgetItem* PageTree::_addPage(Page* page, QTreeWidgetItem *parent)
{
    if (!parent && selectedItems().size() == 1)
        parent = selectedItems().first();

    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    item->setText(0, page->name);
    item->setData(0, Qt::UserRole, qVariantFromValue<void*>(page));
    item->setFlags(item->flags()|Qt::ItemIsEditable);
    addTopLevelItem(item);

    if (parent)
        parent->setExpanded(true);

    return item;
}

void PageTree::_createActions()
{
    QAction *act = new QAction("Add new", this);
    act->setShortcut(QKeySequence("a"));
    act->setShortcutContext(Qt::WidgetShortcut);
    connect(act, &QAction::triggered, this, &PageTree::onAddPage);
    addAction(act);

    act = new QAction("Remove", this);
    act->setShortcut(QKeySequence::Delete);
    act->setShortcutContext(Qt::WidgetShortcut);
    connect(act, &QAction::triggered, this, &PageTree::onRemovePage);
    addAction(act);

    act = new QAction("Rename", this);
    act->setShortcut(QKeySequence("r"));
    act->setShortcutContext(Qt::WidgetShortcut);
    connect(act, &QAction::triggered, this, &PageTree::onRenamePage);
    addAction(act);

    act = new QAction(this);
    act->setSeparator(true);
    addAction(act);

    act = new QAction("Sort by name", this);
    connect(act, &QAction::triggered, this, &PageTree::onSortByName);
    addAction(act);
}

void PageTree::onAddPage()
{
    Page* page = new Page();
    page->flowScene = new CustomFlowScene();
    page->flowScene->setRegistry(_sparkNodesRegistry);
    page->name = QString("New page");
    _nodeFlowScenes.push_back(page);

    QTreeWidgetItem* newItem = _addPage(page);
    clearSelection();
    setItemSelected(newItem, true);

    Q_EMIT signalPageAdded(page);
}

void PageTree::onRemovePage()
{
    QList<QTreeWidgetItem*> items = selectedItems();

    Q_FOREACH (QTreeWidgetItem* item, items)
    {
        Page* page = (Page*)item->data(0, Qt::UserRole).value<void*>();
        int index = _nodeFlowScenes.indexOf(page);
        Q_ASSERT(index != -1);
        _nodeFlowScenes.removeAt(index);
        delete item;
    }

    clearSelection();
}

void PageTree::onItemChanged(QTreeWidgetItem *item, int column)
{
}

void PageTree::onSelectionChanged()
{
    Page* page = nullptr;
    if (selectedItems().size() > 0)
        page = (Page *)selectedItems().first()->data(0, Qt::UserRole).value<void*>();

    Q_EMIT signalPageSwitch(page);
}

void PageTree::onRenamePage()
{
}

void PageTree::onSortByName()
{
}
