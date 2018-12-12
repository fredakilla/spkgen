#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>

#include "node-editor/common/CustomFlowScene.h"
#include "GraphView.h"
#include "GraphEditor.h"
#include "RenderWidget.h"
#include "PageTree.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private Q_SLOTS:
    void newFile();
    void open();
    void save();
    void onRenderViewResized(int width, int height);
    void onPageSwitch(Page* page);

private:
    void shutdown();
    void createWidgets();
    void createActions();
    void createMenus();
    void _addDefaultPage();
    void closeEvent(QCloseEvent* event) override;
    void timerEvent(QTimerEvent* event) override;


    std::shared_ptr<DataModelRegistry> _sparkNodesRegistry;

    // GUI stuff

    QMenu* _fileMenu;
    QAction* _newAct;
    QAction* _openAct;
    QAction* _saveAct;

    CustomFlowScene* _nodeFlowScene;
    FlowView* _nodeFlowView;
    QWidget* _viewportContainer;
    RenderWidget* _renderView;
    GraphView* _pathView;
    GraphEditor* _pathEditor;
    PageTree* _pageTree;
    QDockWidget* _dockView;
    QDockWidget* _dockNodeFlowView;
    QDockWidget* _dockGraph;
    QDockWidget* _dockPageTree;
    int _gameLoopTimerId;
};

#endif // MAINWINDOW_H
