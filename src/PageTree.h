#ifndef PAGETREE_H
#define PAGETREE_H

#include <QTreeWidget>

class PageTree : public QTreeWidget
{
public:
    PageTree(QWidget* parent);
    QSize sizeHint() const;

private Q_SLOTS:
    void _onAddPage();
    void _onRemovePage();
    void _onItemChanged(QTreeWidgetItem *item, int column);
    void _onSelectionChanged();
    void _onSortByName();
    void _onRenamePage();

private:
    void _createActions();
    void _makeConnections();
};

#endif // PAGETREE_H
