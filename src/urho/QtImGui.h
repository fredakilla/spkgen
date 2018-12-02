#pragma once

#include <QObject>
#include <QPoint>
#include <memory>

class QMouseEvent;
class QWheelEvent;
class QKeyEvent;

namespace QtImGui {

void initialize(QWidget *window);
void newFrame();
void endFrame();

class WindowWrapper
{
public:
    virtual ~WindowWrapper() {}
    virtual void installEventFilter(QObject *object) = 0;
    virtual QSize size() const = 0;
    virtual qreal devicePixelRatio() const = 0;
    virtual bool isActive() const = 0;
    virtual QPoint mapFromGlobal(const QPoint &p) const = 0;
};

class ImGuiRenderer : public QObject
{
    Q_OBJECT
public:
    void initialize(WindowWrapper *window);
    void newFrame();
    void endFrame();
    bool eventFilter(QObject *watched, QEvent *event);
    static ImGuiRenderer *instance();

private:
    ImGuiRenderer();
    void onMousePressedChange(QMouseEvent *event);
    void onWheel(QWheelEvent *event);
    void onKeyPressRelease(QKeyEvent *event);

    std::unique_ptr<WindowWrapper> m_window;
    double       g_Time = 0.0f;
    bool         g_MousePressed[3] = { false, false, false };
    float        g_MouseWheel;
    float        g_MouseWheelH;
};

}
