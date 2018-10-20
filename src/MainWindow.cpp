#include "MainWindow.h"
#include "GPDevice.h"

#include <QMessageBox>
#include <QCloseEvent>
#include <QDebug>
#include <QMenuBar>



#include "Nodestyle.h"
#include "SpkZones.h"
#include "SpkSystem.h"
#include "SpkEmitters.h"
#include "SpkModifiers.h"
#include "GraphView.h"

#include <nodes/DataModelRegistry>
using QtNodes::DataModelRegistry;





static std::shared_ptr<DataModelRegistry> registerDataModels()
{
    auto ret = std::make_shared<DataModelRegistry>();

    ret->registerModel<NodeSparkTest>("test");

    ret->registerModel<NodeSparkZonePoint>("zones");
    ret->registerModel<NodeSparkZonePlane>("zones");
    ret->registerModel<NodeSparkZoneSphere>("zones");
    ret->registerModel<NodeSparkZoneBox>("zones");
    ret->registerModel<NodeSparkZoneCylinder>("zones");
    ret->registerModel<NodeSparkZoneRing>("zones");


    ret->registerModel<NodeSparkGroup>("system");
    ret->registerModel<NodeSparkSystem>("system");
    ret->registerModel<NodeSparkGroupList>("system");
    ret->registerModel<NodeSparkQuadRenderer>("renderer");

    ret->registerModel<NodeSparkEmitterList>("emitters");
    ret->registerModel<NodeSparkEmitterStatic>("emitters");
    ret->registerModel<NodeSparkEmitterSpheric>("emitters");
    ret->registerModel<NodeSparkEmitterRandom>("emitters");
    ret->registerModel<NodeSparkEmitterStraight>("emitters");
    ret->registerModel<NodeSparkEmitterNormal>("emitters");

    ret->registerModel<NodeSparkModifierList>("modifiers");
    ret->registerModel<NodeSparkModifierGravity>("modifiers");
    ret->registerModel<NodeSparkModifierFriction>("modifiers");
    ret->registerModel<NodeSparkModifierCollider>("modifiers");
    ret->registerModel<NodeSparkModifierDestroyer>("modifiers");
    ret->registerModel<NodeSparkModifierObstacle>("modifiers");
    ret->registerModel<NodeSparkModifierPointMass>("modifiers");
    ret->registerModel<NodeSparkModifierRandomForce>("modifiers");
    ret->registerModel<NodeSparkModifierRotator>("modifiers");
    ret->registerModel<NodeSparkModifierVortex>("modifiers");



    return ret;
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    //setNodeStyle();
    createActions();
    createMenus();

    _renderView = new RenderViewWidget(this);

    _dockView = new QDockWidget("View", this);
    _dockView->setWidget(_renderView);
    _dockView->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::TopDockWidgetArea, _dockView);

    GPDevice::get().createRenderWindow((void*)_renderView->winId());

    _nodeScene = new CustomFlowScene(registerDataModels());
    _nodeView = new FlowView(_nodeScene);
    _nodeView->setWindowTitle("Node-based flow editor");
    _nodeView->resize(800, 600);
    _nodeView->show();
    _nodeView->scale(0.9, 0.9);

    _dockNodeGraph = new QDockWidget("NodeGraph", this);
    _dockNodeGraph->setWidget(_nodeView);
    _dockNodeGraph->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, _dockNodeGraph);

    _dockGraph = new QDockWidget("Graph", this);
    _dockGraph->setWidget(new GraphView(this));
    _dockGraph->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::TopDockWidgetArea, _dockGraph);

    connect(_nodeScene, &FlowScene::nodeDoubleClicked, this, &MainWindow::showNode);
    connect(_nodeScene, &FlowScene::nodeCreated, this, &MainWindow::initNode);
    connect(_renderView, &RenderViewWidget::windowResized, this, &MainWindow::resizeRenderView);

    // connect to FlowView deleteSelectionAction a method to delete comments graphics items.
    QAction* deleteAction = _nodeView->deleteSelectionAction();
    connect(deleteAction, &QAction::triggered, _nodeScene, &CustomFlowScene::deleteSelectedComments);

    _gameLoopTimerId = startTimer(0);
}

MainWindow::~MainWindow()
{
    delete _nodeScene;
    delete _nodeView;
    delete _renderView;
    delete _dockView;
    delete _dockNodeGraph;
}

void MainWindow::createActions()
{
    newAct = new QAction(tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("New"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);

    openAct = new QAction(tr("&Open"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);

    saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
}

void MainWindow::newFile()
{
    _nodeScene->clearScene();
    _nodeScene->clearComments();
}

void MainWindow::open()
{
    _nodeScene->load();
}

void MainWindow::save()
{
    _nodeScene->save();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "APP_NAME",
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
    GPDevice::get().stop();
}

void MainWindow::resizeRenderView(const QSize& size)
{
    GPDevice::get().resizeRenderView(size.width(), size.height());
}

void MainWindow::timerEvent(QTimerEvent* event)
{
    QWidget::timerEvent(event);
    GPDevice::get().frame();
}

void MainWindow::showNode(QtNodes::Node& node)
{
    NodeSparkSystem* systemNode = dynamic_cast<NodeSparkSystem*>(node.nodeDataModel());
    if(systemNode)
    {
        if(systemNode->getResult().get() != nullptr)
        {
           GPDevice::get().setCurentParticleSystem(systemNode->getResult());
        }
    }
}

void MainWindow::initNode(QtNodes::Node& node)
{
    BaseNode* baseNode = dynamic_cast<BaseNode*>(node.nodeDataModel());
    baseNode->init();
}
