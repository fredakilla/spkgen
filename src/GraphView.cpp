#include "GraphView.h"
#include <QtCore/QtMath>
#include <QtCore/QDebug>
#include <QLabel>

QT_CHARTS_USE_NAMESPACE

GraphView::GraphView(QWidget *parent)
    : QChartView(new QChart(), parent),
      m_scatter(nullptr),
      m_lines(nullptr),
      m_mouseCoordX(nullptr),
      m_mouseCoordY(nullptr)
{
    setRenderHint(QPainter::Antialiasing);
    setInteractive(true);
    //setRubberBand(RectangleRubberBand);

    m_zoomFactorX = 1.0f;
    m_zoomFactorY = 1.0f;

    _isPointSelected = false;
    _isClicked = false;
    _splineResolution = 250.0f;

    chart()->setTitle("Path Editor");

    // create line serie for drawing curve
    m_lines = new QLineSeries();
    m_lines->setName("path");
    m_lines->setColor(Qt::red);
    QPen pen(Qt::red);
    pen.setWidth(2);
    m_lines->setPen(pen);

    // create scatter serie for drawing key points on curve
    m_scatter = new QScatterSeries();
    m_scatter->setName("keys");
    m_scatter->setColor(Qt::red);
    m_scatter->setMarkerSize(10.0);
    m_scatter->setMarkerShape(QScatterSeries::MarkerShapeCircle);

    // create scatter serie for drawing selected keys
    m_scatterSelected = new QScatterSeries();
    m_scatterSelected->setName("keys selected");
    m_scatterSelected->setColor(Qt::black);
    m_scatterSelected->setMarkerSize(12.0);
    m_scatterSelected->setMarkerShape(QScatterSeries::MarkerShapeCircle);

    chart()->legend()->hide();
    chart()->addSeries(m_scatter);
    chart()->addSeries(m_scatterSelected);
    chart()->createDefaultAxes();
    chart()->axisX()->setRange(0.0f, 10.0f);
    chart()->axisY()->setRange(-10.0f, 10.0f);

    m_zoom = chart()->plotArea();

    m_mouseCoordX = new QGraphicsSimpleTextItem(chart());
    m_mouseCoordX->setPos(chart()->size().width()/2 - 50, chart()->size().height());
    m_mouseCoordX->setText("X: ");
    m_mouseCoordX->font().setPointSize(8);

    m_mouseCoordY = new QGraphicsSimpleTextItem(chart());
    m_mouseCoordY->setPos(chart()->size().width()/2 + 50, chart()->size().height());
    m_mouseCoordY->setText("Y: ");
    m_mouseCoordY->font().setPointSize(8);




    //connect(m_scatter, &QScatterSeries::clicked, this, &ChartView::handleClickedPoint);


    chart()->setMargins(QMargins(0,0,0,0));
    chart()->setBackgroundRoundness(0.0f);

/*

    // Customize chart title
    QFont font;
    font.setPixelSize(18);
    chart()->setTitleFont(font);
    chart()->setTitleBrush(QBrush(Qt::white));
    chart()->setTitle("Customchart example");

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
    plotAreaGradient.setColorAt(0.0, QRgb(0x555555));
    plotAreaGradient.setColorAt(1.0, QRgb(0x55aa55));
    plotAreaGradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    chart()->setPlotAreaBackgroundBrush(plotAreaGradient);
    chart()->setPlotAreaBackgroundVisible(true);


    QAbstractAxis *axisX = chart()->axisX();
    QAbstractAxis *axisY = chart()->axisY();

    QFont labelsFont;
    labelsFont.setPixelSize(12);
    axisX->setLabelsFont(labelsFont);
    axisY->setLabelsFont(labelsFont);

    // Customize axis colors
    QPen axisPen(QRgb(0xd18952));
    axisPen.setWidth(2);
    axisX->setLinePen(axisPen);
    axisY->setLinePen(axisPen);

    // Customize axis label colors
    QBrush axisBrush(Qt::white);
    axisX->setLabelsBrush(axisBrush);
    axisY->setLabelsBrush(axisBrush);

    // Customize grid lines and shades
    axisX->setGridLineVisible(false);
    axisY->setGridLineVisible(false);
    axisY->setShadesPen(Qt::NoPen);
    axisY->setShadesBrush(QBrush(QColor(0x99, 0xcc, 0xcc, 0x55)));
    axisY->setShadesVisible(true);*/
}

GraphView::~GraphView()
{
}

void GraphView::setPath(Path* path)
{
    Q_ASSERT(path);

    _currentPath = path;

    m_scatter->clear();
    m_scatterSelected->clear();

    for(size_t i=0; i<_currentPath->getKeyCount(); i++)
    {
        PathKey pathkey = _currentPath->getKeyByIndex(i);
        *m_scatter << QPointF(pathkey.time, pathkey.value);
    }

    plot();
}

void GraphView::resizeEvent(QResizeEvent *event)
{
    if (scene())
    {
        //scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
        //chart()->resize(event->size());
        m_mouseCoordX->setPos(chart()->size().width()/2 - 50, chart()->size().height() - 20);
        m_mouseCoordY->setPos(chart()->size().width()/2 + 50, chart()->size().height() - 20);
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

    m_scatter->append(newPoint);

    rebuildKeys();
    plot();
}

void GraphView::rebuildKeys()
{
    // sort points by x value
    QList<QPointF> listPoints = m_scatter->points();
    std::sort(listPoints.begin(), listPoints.end(), xPointLessThan);

    // clear and replace all points
    m_scatter->clear();
    *m_scatter << listPoints;

    // rebuild spline data

    _currentPath->clear();
    //_currentPath.resize(m_scatter->count());
    for(int i=0; i<m_scatter->count(); ++i)
    {
        _currentPath->addKey(m_scatter->at(i).x(), m_scatter->at(i).y());
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
    m_mouseCoordX->setText(QString("X: %1").arg(chart()->mapToValue(event->pos()).x()));
    m_mouseCoordY->setText(QString("Y: %1").arg(chart()->mapToValue(event->pos()).y()));

    if(_isPointSelected && _isClicked)
    {
        // get index of selected point
        int pointIndex = m_scatter->points().indexOf(_selectedPoint);
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
        m_scatter->replace(_selectedPoint, newPoint);
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
    const auto points = m_scatter->points();
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
    Q_ASSERT(index >= 0 || index < m_scatter->count()-1);

    const double keytime = m_scatter->at(index).x();
    for (int i=0; i<m_scatter->count(); i++)
    {
        const double curKeytime = m_scatter->at(i).x();
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

void GraphView::plot()
{
    if(!_currentPath || _currentPath->getKeyCount() <= 1)
        return;

    // rebuild path
    _currentPath->build();

    // regenerate spline for drawing
    m_lines->clear();
    chart()->removeSeries(m_lines);
    for(size_t i=0; i<_splineResolution; i++)
    {
        double max = _currentPath->getEndTime();
        double x = i*max/_splineResolution;
        *m_lines << QPointF( x, _currentPath->evaluate(x));
    }
    chart()->addSeries(m_lines);

    // need to setup axes after new added serie
    chart()->createDefaultAxes();
    chart()->axisX()->setRange(0.0f, 10.0f);
    chart()->axisY()->setRange(-10.0f, 10.0f);

    chart()->zoomIn(m_zoom);
}

void GraphView::deleteSelectedKeys()
{
    if(!_currentPath)
        return;

    for(int i=0; i<m_scatterSelected->count(); ++i)
    {
        QPointF p = m_scatterSelected->at(i);
        int index = m_scatter->points().indexOf(p);
        if(index > 0 && index < m_scatter->count()-1)
        {
            m_scatter->remove(index);
        }
    }

    m_scatterSelected->clear();

    rebuildKeys();
    plot();
}
