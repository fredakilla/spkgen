#include "QtImGui.h"
#include <QDateTime>
#include <QGuiApplication>
#include <QMouseEvent>
#include <QClipboard>
#include <QCursor>
#include <QDebug>
#include <QWidget>

#include "../renderer/BGFXImGui.h"

// bgfx imgui
#include <thirdparty/bgfxcommon/common.h>
#include <thirdparty/bgfxcommon/imgui/bgfximgui.h>
entry::MouseState m_mouseState;

namespace QtImGui {


class QWidgetWindowWrapper : public WindowWrapper
{
public:
    QWidgetWindowWrapper(QWidget *w) : w(w) {}
    void installEventFilter(QObject *object) override
    {
        return w->installEventFilter(object);
    }
    QSize size() const override
    {
        return w->size();
    }
    qreal devicePixelRatio() const override
    {
        return w->devicePixelRatioF();
    }
    bool isActive() const override
    {
        return w->isActiveWindow();
    }
    QPoint mapFromGlobal(const QPoint &p) const override
    {
        return w->mapFromGlobal(p);
    }
private:
    QWidget *w;
};

void initialize(QWidget *window)
{
    ImGuiRenderer::instance()->initialize(new QWidgetWindowWrapper(window));
}

void newFrame()
{
    ImGuiRenderer::instance()->newFrame();
}

void endFrame()
{
    ImGuiRenderer::instance()->endFrame();
}



namespace {

QHash<int, ImGuiKey> keyMap = {
    { Qt::Key_Tab, ImGuiKey_Tab },
    { Qt::Key_Left, ImGuiKey_LeftArrow },
    { Qt::Key_Right, ImGuiKey_RightArrow },
    { Qt::Key_Up, ImGuiKey_UpArrow },
    { Qt::Key_Down, ImGuiKey_DownArrow },
    { Qt::Key_PageUp, ImGuiKey_PageUp },
    { Qt::Key_PageDown, ImGuiKey_PageDown },
    { Qt::Key_Home, ImGuiKey_Home },
    { Qt::Key_End, ImGuiKey_End },
    { Qt::Key_Delete, ImGuiKey_Delete },
    { Qt::Key_Backspace, ImGuiKey_Backspace },
    { Qt::Key_Enter, ImGuiKey_Enter },
    { Qt::Key_Escape, ImGuiKey_Escape },
    { Qt::Key_A, ImGuiKey_A },
    { Qt::Key_C, ImGuiKey_C },
    { Qt::Key_V, ImGuiKey_V },
    { Qt::Key_X, ImGuiKey_X },
    { Qt::Key_Y, ImGuiKey_Y },
    { Qt::Key_Z, ImGuiKey_Z },
};

QByteArray g_currentClipboardText;

}

ImGuiRenderer::ImGuiRenderer()
{

}

void ImGuiRenderer::initialize(WindowWrapper *window)
{
    m_window.reset(window);

    ImGui::CreateContext();

    // Create bgfx imgui
    ///gplay::GPImGui::getInstance()->imguiInit();
    imguiCreate();

    ImGuiIO &io = ImGui::GetIO();
    for (ImGuiKey key : keyMap.values()) {
        io.KeyMap[key] = key;
    }

    ///io.RenderDrawListsFn = [](ImDrawData *drawData) {
    ///    gplay::GPImGui::getInstance()->imguiRender(drawData);
    ///};

    io.SetClipboardTextFn = [](void *user_data, const char *text) {
        Q_UNUSED(user_data);
        QGuiApplication::clipboard()->setText(text);
    };

    io.GetClipboardTextFn = [](void *user_data) {
        Q_UNUSED(user_data);
        g_currentClipboardText = QGuiApplication::clipboard()->text().toUtf8();
        return (const char *)g_currentClipboardText.data();
    };

    window->installEventFilter(this);
}

void ImGuiRenderer::endFrame()
{
    imguiEndFrame();
}

void ImGuiRenderer::newFrame()
{
    ImGuiIO& io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    io.DisplaySize = ImVec2(m_window->size().width(), m_window->size().height());
    io.DisplayFramebufferScale = ImVec2(m_window->devicePixelRatio(), m_window->devicePixelRatio());

    // Setup time step
    double current_time =  QDateTime::currentMSecsSinceEpoch() / double(1000);
    io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f/60.0f);
    g_Time = current_time;

    // Setup inputs
    // (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
    if (m_window->isActive())
    {
        auto pos = m_window->mapFromGlobal(QCursor::pos());
        io.MousePos = ImVec2(pos.x(), pos.y());   // Mouse position in screen coordinates (set to -1,-1 if no mouse / on another screen, etc.)
    }
    else
    {
        io.MousePos = ImVec2(-1,-1);
    }

    for (int i = 0; i < 3; i++)
    {
        io.MouseDown[i] = g_MousePressed[i];
    }

    io.MouseWheelH = g_MouseWheelH;
    io.MouseWheel = g_MouseWheel;
    g_MouseWheelH = 0;
    g_MouseWheel = 0;

    // Hide OS mouse cursor if ImGui is drawing it
    // glfwSetInputMode(g_Window, GLFW_CURSOR, io.MouseDrawCursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);

    // bgfx imgui mouse state
    m_mouseState.m_mx = io.MousePos.x;
    m_mouseState.m_my = io.MousePos.y;
    m_mouseState.m_buttons[entry::MouseButton::Left] = io.MouseDown[0];
    m_mouseState.m_buttons[entry::MouseButton::Right] = io.MouseDown[1];
    m_mouseState.m_buttons[entry::MouseButton::Middle] = io.MouseDown[2];


    // Start the frame
    ///ImGui::NewFrame();
    imguiBeginFrame(
                m_mouseState.m_mx
                ,  m_mouseState.m_my
                , (m_mouseState.m_buttons[entry::MouseButton::Left] ? IMGUI_MBUT_LEFT : 0)
            | (m_mouseState.m_buttons[entry::MouseButton::Right] ? IMGUI_MBUT_RIGHT : 0)
            | (m_mouseState.m_buttons[entry::MouseButton::Middle] ? IMGUI_MBUT_MIDDLE : 0)
            ,  m_mouseState.m_mz
            , uint16_t(m_window->size().width())
            , uint16_t(m_window->size().height())
            );

}

void ImGuiRenderer::onMousePressedChange(QMouseEvent *event)
{
    g_MousePressed[0] = event->buttons() & Qt::LeftButton;
    g_MousePressed[1] = event->buttons() & Qt::RightButton;
    g_MousePressed[2] = event->buttons() & Qt::MiddleButton;
}

void ImGuiRenderer::onWheel(QWheelEvent *event)
{
    // 5 lines per unit
    g_MouseWheelH += event->delta() / (ImGui::GetTextLineHeight());
    g_MouseWheel += event->delta() / (5.0f * ImGui::GetTextLineHeight());
}

void ImGuiRenderer::onKeyPressRelease(QKeyEvent *event)
{
    ImGuiIO& io = ImGui::GetIO();
    if (keyMap.contains(event->key())) {
        io.KeysDown[keyMap[event->key()]] = event->type() == QEvent::KeyPress;
    }

    if (event->type() == QEvent::KeyPress) {
        QString text = event->text();
        if (text.size() == 1) {
            io.AddInputCharacter(text.at(0).unicode());
        }
    }

#ifdef Q_OS_MAC
    io.KeyCtrl = event->modifiers() & Qt::MetaModifier;
    io.KeyShift = event->modifiers() & Qt::ShiftModifier;
    io.KeyAlt = event->modifiers() & Qt::AltModifier;
    io.KeySuper = event->modifiers() & Qt::ControlModifier; // Comamnd key
#else
    io.KeyCtrl = event->modifiers() & Qt::ControlModifier;
    io.KeyShift = event->modifiers() & Qt::ShiftModifier;
    io.KeyAlt = event->modifiers() & Qt::AltModifier;
    io.KeySuper = event->modifiers() & Qt::MetaModifier;
#endif
}

bool ImGuiRenderer::eventFilter(QObject *watched, QEvent *event)
{
    switch (event->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
        this->onMousePressedChange(static_cast<QMouseEvent *>(event));
        break;
    case QEvent::Wheel:
        this->onWheel(static_cast<QWheelEvent *>(event));
        break;
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
        this->onKeyPressRelease(static_cast<QKeyEvent *>(event));
        break;
    default:
        break;
    }
    return QObject::eventFilter(watched, event);
}

ImGuiRenderer* ImGuiRenderer::instance()
{
    static ImGuiRenderer* instance = nullptr;
    if (!instance) {
        instance = new ImGuiRenderer();
    }
    return instance;
}


}

