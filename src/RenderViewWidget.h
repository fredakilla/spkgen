#ifndef RENDERVIEWWIDGET_H
#define RENDERVIEWWIDGET_H

#include <QWidget>

class RenderViewWidget : public QWidget
{
    Q_OBJECT
public:
    RenderViewWidget(QWidget* parent = nullptr);

    void resizeEvent(QResizeEvent* event);

Q_SIGNALS:
    void windowResized(const QSize& size);
};

#endif // RENDERVIEWWIDGET_H
