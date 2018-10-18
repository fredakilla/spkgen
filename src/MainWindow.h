#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>

#include <nodes/Node>
#include <nodes/FlowScene>
#include <nodes/FlowView>

#include "RenderViewWidget.h"

using QtNodes::Node;
using QtNodes::FlowView;
using QtNodes::FlowScene;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private Q_SLOTS:
    void resizeRenderView(const QSize& size);
    void showNode(Node& node);
    void newFile();
    void open();
    void save();
    void initNode(QtNodes::Node& node);

private:
    void shutdown();
    void createActions();
    void createMenus();
    void closeEvent(QCloseEvent* event) override;
    void timerEvent(QTimerEvent* event) override;

    QMenu *fileMenu;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;

    FlowScene* _nodeScene;
    FlowView* _nodeView;
    RenderViewWidget* _renderView;
    QDockWidget* _dockView;
    QDockWidget* _dockNodeGraph;
    QDockWidget* _dockGraph;
    int _gameLoopTimerId;
};

#endif // MAINWINDOW_H
