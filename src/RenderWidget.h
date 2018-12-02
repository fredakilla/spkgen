#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QWidget>
#include <Urho3D/Core/Object.h>

class RenderWidget : public QWidget, public Urho3D::Object
{
    Q_OBJECT
    URHO3D_OBJECT(RenderWidget, Urho3D::Object)

public:

    RenderWidget(QWidget* parent);
    virtual ~RenderWidget();

    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

Q_SIGNALS:
    void resized(int width, int height);

private:
    QPoint _lastMousePos;
    QPoint _mouseDownPos;

};
#endif // RENDERWIDGET_H
