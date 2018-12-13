#include "MainWindow.h"
#include "UrhoDevice.h"

#include <QMessageBox>
#include <QCloseEvent>
#include <QDebug>
#include <QMenuBar>
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QGroupBox>
#include <QLabel>
#include <QFileDialog>
#include <QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

#include "node-editor/common/Nodestyle.h"
#include "node-editor/spark-nodes/SparkNodesRegistry.h"


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    //setNodeStyle();
    createWidgets();
    createActions();
    createMenus();
    addDefaultPage();

    // create render view
    UrhoDevice::getInstance()->createRenderWindow((void*)_renderView->winId());

    _gameLoopTimerId = startTimer(0);
}

MainWindow::~MainWindow()
{
    //delete _nodeFlowView;
    //delete _renderView;
    //delete _viewportContainer;
    //delete _dockView;
    //delete _dockNodeFlowView;
}

void MainWindow::createWidgets()
{
    // Create render view
    _renderView = new RenderWidget(this);
    _renderView->setMouseTracking(true);
    _renderView->setFocusPolicy(Qt::StrongFocus);

    _viewportContainer = new QWidget(this);
    _viewportContainer->setLayout(new QVBoxLayout());
    _viewportContainer->layout()->addWidget(_renderView);

    _dockView = new QDockWidget("Viewport", this);
    _dockView->setWidget(_viewportContainer);
    _dockView->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::LeftDockWidgetArea, _dockView);

    // Create flow view
    _nodeFlowView = new FlowView(this);
    _nodeFlowView->setWindowTitle("Node-based flow editor");
    _nodeFlowView->show();
    _nodeFlowView->scale(0.9, 0.9);
    setCentralWidget(_nodeFlowView);

    // Create path editor
    _pathView = new GraphView(this);
    _pathEditor = new GraphEditor(_pathView, this);
    _dockGraph = new QDockWidget("Path editor", this);
    _dockGraph->setWidget(_pathEditor);
    _dockGraph->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::LeftDockWidgetArea, _dockGraph);

    // Create page tree
    _pageList = new PageList(this);
    _dockPageList = new QDockWidget("Pages", this);
    _dockPageList->setWidget(_pageList);
    _dockPageList->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::LeftDockWidgetArea, _dockPageList);

    // make some connections
    connect(_renderView, &RenderWidget::signalResized, this, &MainWindow::onRenderViewResized);
    connect(_pageList, &PageList::signalPageAdded, this, &MainWindow::onPageAdd);
    connect(_pageList, &PageList::signalPageSwitch, this, &MainWindow::onPageSwitch);
}

void MainWindow::createActions()
{
    _newAct = new QAction(tr("&New"), this);
    _newAct->setShortcuts(QKeySequence::New);
    _newAct->setStatusTip(tr("New"));
    connect(_newAct, &QAction::triggered, this, &MainWindow::onNewProject);

    _openAct = new QAction(tr("&Open"), this);
    _openAct->setShortcuts(QKeySequence::Open);
    _openAct->setStatusTip(tr("Open"));
    connect(_openAct, &QAction::triggered, this, &MainWindow::onOpen);

    _saveAct = new QAction(tr("&Save"), this);
    _saveAct->setShortcuts(QKeySequence::Save);
    _saveAct->setStatusTip(tr("Save"));
    connect(_saveAct, &QAction::triggered, this, &MainWindow::onSave);
}

void MainWindow::createMenus()
{
    _fileMenu = menuBar()->addMenu(tr("&File"));
    _fileMenu->addAction(_newAct);
    _fileMenu->addAction(_openAct);
    _fileMenu->addAction(_saveAct);
}

void MainWindow::onNewProject()
{
    //_nodeFlowScene->clearScene();
    //_nodeFlowScene->clearComments();
    _pageList->onNewProject();
    addDefaultPage();
}

void MainWindow::onOpen()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr,
                                         tr("Open Flow Scene"),
                                         QDir::homePath(),
                                         tr("Flow Scene Files (*.flow)"));

    if (!QFileInfo::exists(fileName))
        return;

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
        return;

    QByteArray fileData = file.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(fileData));
    _pageList->load(loadDoc.object());
}

void MainWindow::onSave()
{
    QString fileName = QFileDialog::getSaveFileName(nullptr,
                                         tr("Open Flow Scene"),
                                         QDir::homePath(),
                                         tr("Flow Scene Files (*.flow)"));

    if (!fileName.isEmpty())
    {
        if (!fileName.endsWith("flow", Qt::CaseInsensitive))
            fileName += ".flow";

        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly))
        {
            QJsonObject json;
            _pageList->save(json);

            QJsonDocument saveDoc(json);
            file.write(saveDoc.toJson());
        }
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, qApp->applicationName(),
                                                                    tr("Are you sure?\n"),
                                                                    QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::Yes);
    if(resBtn != QMessageBox::Yes)
    {
        event->ignore();
    }
    else
    {
        event->accept();
        shutdown();
    }
}

void MainWindow::shutdown()
{
    killTimer(_gameLoopTimerId);
    UrhoDevice::getInstance()->stop();
}

void MainWindow::timerEvent(QTimerEvent* event)
{
    QMainWindow::timerEvent(event);
    UrhoDevice::getInstance()->runFrame();
}

void MainWindow::onRenderViewResized(int width, int height)
{
    UrhoDevice::getInstance()->onResize(width, height);
}

void MainWindow::addDefaultPage()
{
    _pageList->onAddPage();
}

void MainWindow::onPageSwitch(Page* page)
{
    if(page)
    {
        _nodeFlowView->setScene(page->flowScene);
    }
}

void MainWindow::onPageAdd(Page* page)
{
    if(page)
    {
        // create required connections for each new page
        connect(page->flowScene, &CustomFlowScene::signalShowPathNode, _pathView, &GraphView::onSetPathNode);
        connect(page->flowScene, &CustomFlowScene::signalShowPath4Node, _pathView, &GraphView::onSetPath4Node);
        connect(page->flowScene, &CustomFlowScene::signalShowPathNode, _pathEditor, &GraphEditor::onSetPath);
        connect(page->flowScene, &CustomFlowScene::signalShowPath4Node, _pathEditor, &GraphEditor::onSetPath4);

        // connect to FlowView deleteSelectionAction a method to delete comments graphics items.
        QAction* deleteAction = _nodeFlowView->deleteSelectionAction();
        connect(deleteAction, &QAction::triggered, page->flowScene, &CustomFlowScene::deleteSelectedComments);
    }
}
