#include "PageList.h"
#include "node-editor/spark-nodes/SparkNodesRegistry.h"

#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMenu>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QMouseEvent>

PageList::PageList(QWidget* parent) :
    QListWidget(parent)
{
    _sparkNodesRegistry = registerSparkNodesDataModels();

    setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);
    setDragDropMode(InternalMove);

    _createActions();

    connect(this, &PageList::itemSelectionChanged, this, &PageList::onSelectionChanged);
    connect(this, &PageList::itemChanged, this, &PageList::onItemChanged);
}

QSize PageList::sizeHint() const
{
    return QSize(250, 0);
}

void PageList::mousePressEvent(QMouseEvent* event)
{
    QModelIndex item = indexAt(event->pos());
    QListWidget::mousePressEvent(event);
    if ((item.row() == -1 && item.column() == -1))
    {
        clearSelection();
    }
}

QListWidgetItem* PageList::_addPage(Page* page, QListWidgetItem *parent)
{
    if (!parent && selectedItems().size() == 1)
        parent = selectedItems().first();

    QListWidgetItem* item = new QListWidgetItem();
    item->setText(page->flowScene->getName());
    item->setData(Qt::UserRole, qVariantFromValue<void*>(page));
    item->setFlags(item->flags()|Qt::ItemIsEditable);
    addItem(item);

    return item;
}

void PageList::_createActions()
{
    QAction *act = new QAction("Add new", this);
    act->setShortcut(QKeySequence("a"));
    act->setShortcutContext(Qt::WidgetShortcut);
    connect(act, &QAction::triggered, this, &PageList::onAddPage);
    addAction(act);

    act = new QAction("Remove", this);
    act->setShortcut(QKeySequence::Delete);
    act->setShortcutContext(Qt::WidgetShortcut);
    connect(act, &QAction::triggered, this, &PageList::onRemovePage);
    addAction(act);

    act = new QAction(this);
    act->setSeparator(true);
    addAction(act);

    act = new QAction("Sort by name", this);
    connect(act, &QAction::triggered, this, &PageList::onSortByName);
    addAction(act);
}

void PageList::_clearAll()
{
    // remove all items on each scene
    Q_FOREACH (Page* page, _nodeFlowScenes)
    {
        page->flowScene->clearComments();
        page->flowScene->clearScene();
    }

    // clear items in QListWidget
    clear();

    // clear flowscene list
    _nodeFlowScenes.clear();
}

void PageList::onNewProject()
{
    _clearAll();
}

void PageList::onAddPage()
{
    Page* page = new Page();
    page->flowScene = new CustomFlowScene();
    page->flowScene->setRegistry(_sparkNodesRegistry);
    page->flowScene->setName("New page");
    _nodeFlowScenes.push_back(page);

    QListWidgetItem* newItem = _addPage(page);

    clearSelection();
    setItemSelected(newItem, true);

    Q_EMIT signalPageAdded(page);
}

void PageList::onRemovePage()
{
    QList<QListWidgetItem*> items = selectedItems();

    Q_FOREACH (QListWidgetItem* item, items)
    {
        Page* page = (Page*)item->data(Qt::UserRole).value<void*>();
        int index = _nodeFlowScenes.indexOf(page);
        Q_ASSERT(index != -1);
        _nodeFlowScenes.removeAt(index);
        delete item;
    }

    clearSelection();
}

void PageList::onItemChanged(QListWidgetItem *item)
{
    Page* page = (Page*)item->data(Qt::UserRole).value<void*>();
    page->flowScene->setName(item->text());
}

void PageList::onSelectionChanged()
{
    Page* page = nullptr;
    if (selectedItems().size() > 0)
        page = (Page *)selectedItems().first()->data(Qt::UserRole).value<void*>();

    Q_EMIT signalPageSwitch(page);
}

void PageList::onSortByName()
{
}

void PageList::load(const QJsonObject &json)
{
    _clearAll();

    QJsonArray pageArray = json["pages"].toArray();
    for (int pageIndex = 0; pageIndex < pageArray.size(); ++pageIndex)
    {
        // create new page
        onAddPage();

        // get last created page
        int lastIndex = count()-1;
        Page* page = (Page*)item(lastIndex)->data(Qt::UserRole).value<void*>();

        // load page
        QJsonObject pageJson = pageArray[pageIndex].toObject();
        page->flowScene->load(pageJson);
        item(lastIndex)->setText(page->flowScene->getName());
    }

    // select first page
    setCurrentRow(0);
}

void PageList::save(QJsonObject &json)
{
    QJsonArray pageArray;
    Q_FOREACH (Page* page, _nodeFlowScenes)
    {
        QJsonObject jsonPageData;
        page->flowScene->save(jsonPageData);
        pageArray.append(jsonPageData);
    }
    json["pages"] = pageArray;
}

