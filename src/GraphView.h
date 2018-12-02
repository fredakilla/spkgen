/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Charts module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef PATHEDITOR_H
#define PATHEDITOR_H

#include <QtCharts/QChartGlobal>
#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QCategoryAxis>

#include "node-editor/spark-nodes/SpkInterpolators.h"


QT_CHARTS_USE_NAMESPACE

class GraphView : public QChartView
{
    Q_OBJECT

public:
    GraphView(QWidget *parent = 0);
    ~GraphView();

    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

public Q_SLOTS:
    void setPathNode(NodePath* node);

private Q_SLOTS:
    void handleClickedPoint(const QPointF &point);

private:
    void zoom();
    void addNewPoint(QPointF newPoint);
    void deleteSelectedKeys();
    void rebuildKeys();
    bool isKeyMovable(double newTime, int index);
    void plot();

    NodePath* _pathNode;
    Path* _currentPath;
    QScatterSeries* m_scatter;
    QScatterSeries* m_scatterSelected;
    QLineSeries* m_lines;
    QRectF m_zoom;
    qreal m_zoomFactorX;
    qreal m_zoomFactorY;
    QGraphicsSimpleTextItem* m_mouseCoordX;
    QGraphicsSimpleTextItem* m_mouseCoordY;

    bool _isPointSelected;
    bool _isClicked;
    QPointF _selectedPoint;
    double _splineResolution;
};

#endif // PATHEDITOR_H
