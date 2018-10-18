#include "RenderViewWidget.h"
#include <QResizeEvent>

RenderViewWidget::RenderViewWidget(QWidget* parent) :
    QWidget(parent)
{
    //setAttribute(Qt::WA_NativeWindow);
    //setUpdatesEnabled(false);
    //setGeometry(0,0,200,200);
}

void RenderViewWidget::resizeEvent(QResizeEvent* event)
{
    Q_EMIT(windowResized(event->size()));
}
