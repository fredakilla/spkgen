#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>

#include "node-editor/common/CustomFlowScene.h"
#include "GraphView.h"
#include "GraphEditor.h"
#include "RenderWidget.h"
#include "PageList.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private Q_SLOTS:
    void onNewProject();
    void onOpen();
    void onSaveAs();
    void onSave();
    void onRenderViewResized(int width, int height);
    void onPageSwitch(Page* page);
    void onPageAdd(Page* page);

private:
    void shutdown();
    void createWidgets();
    void createActions();
    void createMenus();
    void addDefaultPage();
    void setCurrentFile(const QString &filePath);
    void closeEvent(QCloseEvent* event) override;
    void timerEvent(QTimerEvent* event) override;


    // GUI stuff

    QString _currentFile;
    QMenu* _fileMenu;
    FlowView* _nodeFlowView;
    QWidget* _viewportContainer;
    RenderWidget* _renderView;
    GraphView* _pathView;
    GraphEditor* _pathEditor;
    PageList* _pageList;
    QDockWidget* _dockView;
    QDockWidget* _dockNodeFlowView;
    QDockWidget* _dockGraph;
    QDockWidget* _dockPageList;
    int _gameLoopTimerId;
};

#endif // MAINWINDOW_H
