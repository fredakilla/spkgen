#include "PageTree.h"

#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMenu>

PageTree::PageTree(QWidget* parent) : QTreeWidget(parent)
{
    _createActions();
    _makeConnections();
}

QSize PageTree::sizeHint() const
{
    return QSize(150, 0);
}

void PageTree::_createActions()
{
    QAction *act = new QAction("Add new", this);
    act->setShortcut(QKeySequence("a"));
    act->setShortcutContext(Qt::WidgetShortcut);
    connect(act, SIGNAL(triggered()), this, SLOT(_onAddPage()));
    addAction(act);

    act = new QAction("Remove", this);
    act->setShortcut(QKeySequence::Delete);
    act->setShortcutContext(Qt::WidgetShortcut);
    connect(act, SIGNAL(triggered()), this, SLOT(_onRemovePage()));
    addAction(act);

    act = new QAction("Rename", this);
    act->setShortcut(QKeySequence("r"));
    act->setShortcutContext(Qt::WidgetShortcut);
    connect(act, SIGNAL(triggered()), this, SLOT(_onRenamePage()));
    addAction(act);

    act = new QAction(this);
    act->setSeparator(true);
    addAction(act);

    act = new QAction("Sort by name", this);
    connect(act, SIGNAL(triggered()), this, SLOT(_onSortByName()));
    addAction(act);
}

void PageTree::_makeConnections()
{
    connect(this, &PageTree::itemSelectionChanged, this, &PageTree::_onSelectionChanged);
    connect(this, &PageTree::itemChanged, this, &PageTree::_onItemChanged);
}

void PageTree::_onAddPage()
{
}

void PageTree::_onRemovePage()
{
}

void PageTree::_onItemChanged(QTreeWidgetItem *item, int column)
{
}

void PageTree::_onSelectionChanged()
{
}
