#include "GraphView.h"
#include <QtCore/QtMath>
#include <QtCore/QDebug>
#include <QLabel>

QT_CHARTS_USE_NAMESPACE

GraphView::GraphView(QWidget *parent)
    : QChartView(new QChart(), parent),
      _lineCount(1),
      _currentSelectedLine(0),
      m_mouseCoordX(nullptr),
      m_mouseCoordY(nullptr)
{
    for (unsigned int i=0; i<MAX_LINES; i++)
    {
        m_lines[i] = nullptr;
        m_scatter[i] = nullptr;
    }

    setRenderHint(QPainter::Antialiasing);
    setInteractive(true);
    //setRubberBand(RectangleRubberBand);

    m_zoomFactorX = 1.0f;
    m_zoomFactorY = 1.0f;

    _isPointSelected = false;
    _isClicked = false;
    _splineResolution = 250.0f;

    chart()->setTitle("Path Editor");

    const QColor pathCols[4] = {Qt::red, Qt::green, Qt::blue, Qt::magenta};

    for (unsigned int i=0; i<MAX_LINES; i++)
    {
        // create line serie for drawing curve
        m_lines[i] = new QLineSeries();
        m_lines[i]->setName("path");
        m_lines[i]->setColor(Qt::red + i);
        QPen pen(pathCols[i]);
        pen.setWidth(2);
        m_lines[i]->setPen(pen);

         // create scatter serie for drawing key points on curve
        m_scatter[i] = new QScatterSeries();
        m_scatter[i]->setName("keys");
        m_scatter[i]->setColor(Qt::red + i);
        m_scatter[i]->setMarkerSize(10.0);
        m_scatter[i]->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    }

    // create scatter serie for drawing selected keys
    m_scatterSelected = new QScatterSeries();
    m_scatterSelected->setName("keys selected");
    m_scatterSelected->setColor(Qt::black);
    m_scatterSelected->setMarkerSize(12.0);
    m_scatterSelected->setMarkerShape(QScatterSeries::MarkerShapeCircle);

    chart()->legend()->hide();
    for (unsigned int i=0; i<MAX_LINES; i++)
        chart()->addSeries(m_scatter[i]);
    chart()->addSeries(m_scatterSelected);
    chart()->createDefaultAxes();
    chart()->axisX()->setRange(0.0f, 5.0f);
    chart()->axisY()->setRange(-1.0f, 1.0f);

    m_zoom = chart()->plotArea();

    m_mouseCoordX = new QGraphicsSimpleTextItem(chart());
    m_mouseCoordX->setText("X: ");
    m_mouseCoordX->font().setPointSize(6);
    m_mouseCoordX->setBrush(Qt::white);

    m_mouseCoordY = new QGraphicsSimpleTextItem(chart());
    m_mouseCoordY->setText("Y: ");
    m_mouseCoordY->font().setPointSize(6);
    m_mouseCoordY->setBrush(Qt::white);

    //connect(m_scatter, &QScatterSeries::clicked, this, &ChartView::handleClickedPoint);

    setStyle();
}

GraphView::~GraphView()
{
}

void GraphView::setStyle()
{
    chart()->setMargins(QMargins(0,0,0,0));
    chart()->setBackgroundRoundness(0.0f);

    // Customize chart title
    QFont font;
    font.setPixelSize(12);
    chart()->setTitleFont(font);
    chart()->setTitleBrush(QBrush(Qt::white));
    chart()->setTitle("Graph");

    // Customize chart background
    QLinearGradient backgroundGradient;
    backgroundGradient.setStart(QPointF(0, 0));
    backgroundGradient.setFinalStop(QPointF(0, 1));
    backgroundGradient.setColorAt(0.0, QRgb(0x353535));
    backgroundGradient.setColorAt(1.0, QRgb(0x2c2c2c));
    backgroundGradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    chart()->setBackgroundBrush(backgroundGradient);

    // Customize plot area background
    QLinearGradient plotAreaGradient;
    plotAreaGradient.setStart(QPointF(0, 1));
    plotAreaGradient.setFinalStop(QPointF(1, 0));
    plotAreaGradient.setColorAt(0.0, QRgb(0x222222));
    plotAreaGradient.setColorAt(1.0, QRgb(0x555555));
    plotAreaGradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    chart()->setPlotAreaBackgroundBrush(plotAreaGradient);
    chart()->setPlotAreaBackgroundVisible(true);

    setAxisStyle();
}

void GraphView::setAxisStyle()
{
    QAbstractAxis* axisX = chart()->axisX();
    QAbstractAxis* axisY = chart()->axisY();

    QFont labelsFont;
    labelsFont.setPixelSize(12);
    axisX->setLabelsFont(labelsFont);
    axisY->setLabelsFont(labelsFont);
    axisX->setLabelsColor(Qt::white);
    axisY->setLabelsColor(Qt::white);

    // Customize axis colors
    QPen axisPen(QRgb(0xd18952));
    axisPen.setWidth(1);
    axisX->setLinePen(axisPen);
    axisY->setLinePen(axisPen);

    // Customize axis label colors
    QBrush axisBrush(Qt::white);
    axisX->setLabelsBrush(axisBrush);
    axisY->setLabelsBrush(axisBrush);

    // Customize grid lines and shades
    axisX->setGridLineVisible(true);
    axisY->setGridLineVisible(true);
    axisY->setLinePen(axisPen);
    //axisY->setShadesPen(Qt::NoPen);
    //axisY->setShadesBrush(QBrush(QColor(0x99, 0xcc, 0xcc, 0x55)));
    //axisY->setShadesVisible(true);

    axisX->setLabelsColor(Qt::white);
    axisY->setLabelsColor(Qt::white);
    axisX->setGridLineColor(QColor(128,128,128,128));
    axisY->setGridLineColor(QColor(128,128,128,128));
}

void GraphView::setPathNode(NodePath* node)
{
    Q_ASSERT(node);

    _pathNode = node;
    _currentPath = node->getResult();
    _lineCount = 1;
    _currentSelectedLine = 0;

    m_scatter[0]->clear();
    m_scatterSelected->clear();

    for(size_t i=0; i<_currentPath->getKeyCount(); i++)
    {
        QScatterSeries* scatter = m_scatter[0];
        PathKey pathkey = _currentPath->getKeyByIndex(i);
        *scatter << QPointF(pathkey.time, pathkey.value);
    }

    plot();
}

void GraphView::setPath4Node(NodePath4* node)
{
    Q_ASSERT(node);

    _pathNode4 = node;
    Path4* path4 = node->getResult();
    _currentPath = &path4->getSubPath(0);
    _currentSelectedLine = 0;
    _lineCount = 4;

    m_scatter[0]->clear();
    m_scatter[1]->clear();
    m_scatter[2]->clear();
    m_scatter[3]->clear();
    m_scatterSelected->clear();

    for(size_t j=0; j<_lineCount; j++)
    {
        for(size_t i=0; i<path4->getSubPath(j).getKeyCount(); i++)
        {
            QScatterSeries* scatter = m_scatter[j];
            PathKey pathkey = path4->getSubPath(j).getKeyByIndex(i);
            *scatter << QPointF(pathkey.time, pathkey.value);
        }
    }

    plot();
}

void GraphView::resizeEvent(QResizeEvent *event)
{
    if (scene())
    {
        //scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
        //chart()->resize(event->size());
        m_mouseCoordX->setPos(20, 0);
        m_mouseCoordY->setPos(70, 0);
    }
    QChartView::resizeEvent(event);
}

void GraphView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Plus:
        chart()->zoomIn();
        break;
    case Qt::Key_Minus:
        //chart()->zoomOut();
        deleteSelectedKeys();
        break;
    case Qt::Key_Delete:
        deleteSelectedKeys();
        break;
    }
}

void GraphView::keyReleaseEvent(QKeyEvent *event)
{

}

static bool xPointLessThan(const QPointF &p1, const QPointF &p2)
{
    return p1.x() < p2.x();
}

void GraphView::addNewPoint(QPointF newPoint)
{
    if(!_currentPath)
        return;

    // don't add new point behind x origin
    if(newPoint.x() <= 0.0f)
        return;

    m_scatter[_currentSelectedLine]->append(newPoint);

    rebuildKeys();
    plot();
}

void GraphView::rebuildKeys()
{
    QScatterSeries* scatter = m_scatter[_currentSelectedLine];

    // sort points by x value
    QList<QPointF> listPoints = scatter->points();
    std::sort(listPoints.begin(), listPoints.end(), xPointLessThan);

    // clear and replace all points
    scatter->clear();
    *scatter << listPoints;

    // rebuild spline data

    _currentPath->clear();
    for(int i=0; i<scatter->count(); ++i)
    {
        _currentPath->addKey(scatter->at(i).x(), scatter->at(i).y());
    }
}

void GraphView::mousePressEvent(QMouseEvent *event)
{
    QChartView::mousePressEvent(event);

    // add new key
    if ((event->modifiers() == Qt::ControlModifier))
    {
        addNewPoint(chart()->mapToValue(event->pos()));
        return;
    }

    m_scatterSelected->clear();
    _isClicked = true;
    handleClickedPoint(chart()->mapToValue(event->pos()));
}

void GraphView::mouseMoveEvent(QMouseEvent *event)
{
    QScatterSeries* scatter = m_scatter[_currentSelectedLine];

    QString sx = QString::number(chart()->mapToValue(event->pos()).x(), 'f', 2);
    QString sy = QString::number(chart()->mapToValue(event->pos()).y(), 'f', 2);
    m_mouseCoordX->setText(QString("X: %1").arg(sx));
    m_mouseCoordY->setText(QString("Y: %1").arg(sy));

    if(_isPointSelected && _isClicked)
    {
        // get index of selected point
        int pointIndex = scatter->points().indexOf(_selectedPoint);
        if(pointIndex == -1)
            return;

        // get new point coord
        QPointF newPoint = chart()->mapToValue(event->pos());

        // stick first key at 0.0f on x axis
        if(pointIndex == 0)
            newPoint.setX(0.0f);

        if(isKeyMovable(newPoint.x(), pointIndex) == false)
            return;

        // replace point at index
       _currentPath->setAtIndex(pointIndex, newPoint.x(), newPoint.y());

        // replace point (simulate moving)
        scatter->replace(_selectedPoint, newPoint);
        m_scatterSelected->replace(0, newPoint);

        _selectedPoint = newPoint;

        plot();
    }

    QChartView::mouseMoveEvent(event);
}

void GraphView::mouseReleaseEvent(QMouseEvent *event)
{
    QChartView::mouseReleaseEvent(event);

    _isClicked = false;
    _isPointSelected = false;
}

void GraphView::zoom()
{
    chart()->zoomReset();

    QRectF rect = chart()->plotArea();
    QPointF center = chart()->plotArea().center();
    qreal left = chart()->plotArea().left();

    rect.setWidth(m_zoomFactorX*rect.width());
    rect.setHeight(m_zoomFactorY*rect.height());

    rect.moveCenter(center);
    rect.moveLeft(left);

    chart()->zoomIn(rect);
    m_zoom = rect;
}

void GraphView::wheelEvent(QWheelEvent *event)
{
    // zoom

    float delta = event->angleDelta().y() > 0 ? 0.5f : 2.0f;
    if (event->modifiers() || event->modifiers()&Qt::ControlModifier)
        m_zoomFactorX *= delta;
    else
        m_zoomFactorY *= delta;

    zoom();

    QChartView::wheelEvent(event);
}


void GraphView::handleClickedPoint(const QPointF &point)
{
    _isPointSelected = false;

    QPointF clickedPoint = point;
    // Find the closest point from series 1
    QPointF closest(INT_MAX, INT_MAX);
    qreal distance(INT_MAX);
    const auto points = m_scatter[_currentSelectedLine]->points();
    for (const QPointF &currentPoint : points)
    {
        qreal currentDistance = qSqrt((currentPoint.x() - clickedPoint.x())
                                      * (currentPoint.x() - clickedPoint.x())
                                      + (currentPoint.y() - clickedPoint.y())
                                      * (currentPoint.y() - clickedPoint.y()));
        if (currentDistance < distance)
        {
            distance = currentDistance;
            closest = currentPoint;

            _isPointSelected = true;
            _selectedPoint = closest;

            // add point to selection
            m_scatterSelected->clear();
            m_scatterSelected->append(closest);
        }
    }
}

bool GraphView::isKeyMovable(double newTime, int index)
{
    QScatterSeries* scatter = m_scatter[_currentSelectedLine];
    Q_ASSERT(index >= 0 || index < scatter->count()-1);

    const double keytime = scatter->at(index).x();
    for (int i=0; i<scatter->count(); i++)
    {
        const double curKeytime = scatter->at(i).x();
        if (keytime != curKeytime)
        {
            if ((keytime <= curKeytime && newTime >= curKeytime) ||
                (keytime >= curKeytime && newTime <= curKeytime)
               )
            {
                return false;
            }
        }
    }

    return true;
}

void GraphView::plot1()
{
    Q_ASSERT(_lineCount == 1);

    if(!_pathNode || !_currentPath || _currentPath->getKeyCount() <= 1)
        return;

    // rebuild path
    _currentPath->build();

    // regenerate spline for drawing
    QLineSeries* line = m_lines[0];
    line->clear();
    chart()->removeSeries(line);
    for(size_t i=0; i<_splineResolution; i++)
    {
        double max = _currentPath->getEndTime();
        double x = i*max/_splineResolution;
        *line << QPointF( x, _currentPath->evaluate(x));
    }
    chart()->addSeries(line);

    // show line and scatter
    m_lines[0]->show();
    m_scatter[0]->show();

    // need to setup axes after new added serie
    chart()->createDefaultAxes();
    chart()->axisX()->setRange(0.0f, 5.0f);
    chart()->axisY()->setRange(-1.0f, 1.0f);
    setAxisStyle();

    chart()->zoomIn(m_zoom);

    // trigger node connections and updates in flow graph
    _pathNode->dataUpdated(0);
}

void GraphView::plot4()
{
    Q_ASSERT(_lineCount == 4);

    if (!_pathNode4)
        return;

    Path4* path4 = _pathNode4->getResult();

    // rebuild path
    path4->getSubPath(0).build();
    path4->getSubPath(1).build();
    path4->getSubPath(2).build();
    path4->getSubPath(3).build();

    // regenerate spline for drawing
    for (unsigned int j=0; j<_lineCount; j++)
    {
        QLineSeries* line = m_lines[j];

        line->clear();
        chart()->removeSeries(line);
        for(size_t i=0; i<_splineResolution; i++)
        {
            double max = path4->getSubPath(j).getEndTime();
            double x = i*max/_splineResolution;
            *line << QPointF( x, path4->getSubPath(j).evaluate(x));
        }
        chart()->addSeries(line);

        // show lines and scatters
        m_lines[j]->show();
        m_scatter[j]->show();
    }


    // need to setup axes after new added serie
    chart()->createDefaultAxes();
    chart()->axisX()->setRange(0.0f, 5.0f);
    chart()->axisY()->setRange(-1.0f, 1.0f);
    setAxisStyle();

    chart()->zoomIn(m_zoom);

    // trigger node connections and updates in flow graph
    _pathNode4->dataUpdated(0);
}

void GraphView::plot()
{
    // hide all lines and scatters
    for (unsigned int i=0; i<MAX_LINES; i++)
    {
        m_lines[i]->hide();
        m_scatter[i]->hide();
    }

    if(_lineCount == 1)
        plot1();
    else if(_lineCount == 4)
        plot4();
    else
    {
        // should not be reached.
        Q_ASSERT(0);
    }
}

void GraphView::deleteSelectedKeys()
{
    if(!_currentPath)
        return;

    QScatterSeries* scatter = m_scatter[_currentSelectedLine];

    for(int i=0; i<m_scatterSelected->count(); ++i)
    {
        QPointF p = m_scatterSelected->at(i);
        int index = scatter->points().indexOf(p);
        if(index > 0 && index < scatter->count()-1)
        {
            scatter->remove(index);
        }
    }

    m_scatterSelected->clear();

    rebuildKeys();
    plot();
}
