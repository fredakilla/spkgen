#ifndef PLATFORMQT_H
#define PLATFORMQT_H

#include <QObject>

class QMouseEvent;
class QWheelEvent;
class QKeyEvent;

/**
 * event filter for gplay widget
 */
class GPlayWidgetEventFilter : public QObject
{
    Q_OBJECT
public:
    bool eventFilter(QObject* watched, QEvent* event);

private:
    void onWheel(QWheelEvent* event);
    void onMousePress(QMouseEvent* event);
    void onMouseRelease(QMouseEvent* event);
    void onMouseMove(QMouseEvent* event);
    void onKeyEvent(QKeyEvent* event);

};

#endif // PLATFORMQT_H
