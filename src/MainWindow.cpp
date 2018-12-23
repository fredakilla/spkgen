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


static QString EDITOR_CAPTION = "Spkgen";

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    //setNodeStyle();
    createWidgets();
    createActions();
    addDefaultPage();
    setCurrentFile("");

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

void MainWindow::setCurrentFile(const QString &filePath)
{
    QString filePathFinal = filePath;

    if (filePathFinal != "")
    {
        QFile file(filePathFinal+".orig");
        if (file.open(QIODevice::ReadOnly))
        {
            filePathFinal = QString(file.readLine());
            file.close();
        }
    }

    _currentFile = filePathFinal;
    setWindowModified(false);

    // update recent files list in settings
    if (filePathFinal != "")
    {
        setWindowTitle(EDITOR_CAPTION+" - ["+filePathFinal+"[*]]");
    }
    else
        setWindowTitle(EDITOR_CAPTION+" - [NewSpkgenProject.flow[*]]");
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
    _nodeFlowView->setScene(nullptr);
    resetFlowViewTransform();
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

void MainWindow::resetFlowViewTransform()
{
    Q_ASSERT(_nodeFlowView);
    _nodeFlowView->resetTransform();
    _nodeFlowView->scale(0.8, 0.8);
    _nodeFlowView->centerOn(0,0);
}

void MainWindow::createActions()
{
    QAction* action;

    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));

    action = new QAction(tr("&New"), this);
    action->setShortcuts(QKeySequence::New);
    action->setStatusTip(tr("New"));
    connect(action, &QAction::triggered, this, &MainWindow::onNewProject);
    fileMenu->addAction(action);

    action = new QAction(tr("&Open"), this);
    action->setShortcuts(QKeySequence::Open);
    action->setStatusTip(tr("Open"));
    connect(action, &QAction::triggered, this, &MainWindow::onOpen);
    fileMenu->addAction(action);

    action = new QAction(tr("&Save as"), this);
    action->setStatusTip(tr("Save as"));
    connect(action, &QAction::triggered, this, &MainWindow::onSaveAs);
    fileMenu->addAction(action);

    action = new QAction(tr("&Save"), this);
    action->setShortcuts(QKeySequence::Save);
    action->setStatusTip(tr("Save"));
    connect(action, &QAction::triggered, this, &MainWindow::onSave);
    fileMenu->addAction(action);


    QMenu* sceneMenu = menuBar()->addMenu(tr("&Scene"));

    QActionGroup* showSceneGroup = new QActionGroup(this);

    action = new QAction(tr("&Grid scene"), this);
    action->setCheckable(true);
    action->setChecked(true);
    connect(action, &QAction::triggered, this, &MainWindow::onShowSceneGrid);
    connect(action, &QAction::triggered, action, &QAction::setChecked);
    sceneMenu->addAction(action);
    showSceneGroup->addAction(action);

    action = new QAction(tr("&Scene 1"), this);
    action->setCheckable(true);
    connect(action, &QAction::triggered, this, &MainWindow::onShowScene1);
    connect(action, &QAction::triggered, action, &QAction::setChecked);
    sceneMenu->addAction(action);
    showSceneGroup->addAction(action);

    sceneMenu->addSeparator();

    action = new QAction(tr("Show debug shapes"), this);
    action->setCheckable(true);
    action->setChecked(true);
    connect(action, &QAction::triggered, this, &MainWindow::onShowDebugShapes);
    connect(action, &QAction::triggered, action, &QAction::setChecked);
    sceneMenu->addAction(action);

}

void MainWindow::onNewProject()
{
    _pageList->onNewProject();
    addDefaultPage();
    setCurrentFile("");
    resetFlowViewTransform();
}

void MainWindow::onOpen()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr,
                                         tr("Open Flow Scene"),
                                         QDir::currentPath(),
                                         tr("Flow Scene Files (*.flow)"));

    if (!QFileInfo::exists(fileName))
        return;

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
        return;

    // load file content and populates pages list
    QByteArray fileData = file.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(fileData));
    _pageList->load(loadDoc.object());

    // don't show any spark system when loading a file
    SPK::Ref<SPK::System> nullSystem;
    nullSystem.reset();
    UrhoDevice::getInstance()->setCurentParticleSystem(nullSystem);

    // keep track of current file name
    setCurrentFile(fileName);

    resetFlowViewTransform();
}

void MainWindow::onSaveAs()
{
    QString fileName = QFileDialog::getSaveFileName(nullptr,
                                         tr("Open Flow Scene"),
                                         QDir::currentPath(),
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

        setCurrentFile(fileName);
    }
}

void MainWindow::onSave()
{
    if (!_currentFile.isEmpty())
    {
        QFile file(_currentFile);
        if (file.open(QIODevice::WriteOnly))
        {
            QJsonObject json;
            _pageList->save(json);
            QJsonDocument saveDoc(json);
            file.write(saveDoc.toJson());
        }
    }
    else
    {
        onSaveAs();
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
        _nodeFlowView->centerOn(0,0);
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

void MainWindow::onShowSceneGrid()
{
    UrhoDevice::getInstance()->setScene(0);
}

void MainWindow::onShowScene1()
{
    UrhoDevice::getInstance()->setScene(1);
}

void MainWindow::onShowDebugShapes(bool enabled)
{
    UrhoDevice::getInstance()->showDebugShapes(enabled);
}
