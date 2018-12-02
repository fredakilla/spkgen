#include "../core/Base.h"
#include "../core/Platform.h"
#include "../core/Game.h"

#include <bgfx/platform.h>

#include "../renderer/Renderer.h"
#include "../renderer/BGFXImGui.h"

#include <QX11Info>
#include <QDebug>
#include <QThread>
#include <QWidget>
#include <QApplication>

#include "QtImGui.h"
#include "../GplayDevice.h"


#include "PlatformQt.h"
#include <QEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>


//------------------------------------------------------------------------------------------------------
// static var used here
//------------------------------------------------------------------------------------------------------

int __app_argc = 0;
char** __app_argv = nullptr;

static QWidget* __widget;
static GPlayWidgetEventFilter __widgetEventFilter;

// timer
static double __timeAbsolute;
static std::chrono::time_point<std::chrono::high_resolution_clock> __timeStart;

// mouse input
static bool __mouseCaptured = false;
static float __mouseCapturePointX = 0;
static float __mouseCapturePointY = 0;
static bool __cursorVisible = true;
static QPoint __mouseCapturePoint = QPoint(0,0);




//------------------------------------------------------------------------------------------------------
// key utilities to convert Qt key to gplay key
//------------------------------------------------------------------------------------------------------
static QMap<Qt::Key, gplay::Keyboard::Key> __keymap;
static void __initKeyMap();
static gplay::Keyboard::Key __convertQtKeyToGplay(Qt::Key qtKey);


//------------------------------------------------------------------------------------------------------
// map keys Qt/gplay
//------------------------------------------------------------------------------------------------------
void __initKeyMap()
{
    __keymap[Qt::Key_unknown]     = gplay::Keyboard::KEY_NONE;
    __keymap[Qt::Key_Escape]        = gplay::Keyboard::KEY_ESCAPE;
    __keymap[Qt::Key_Tab]           = gplay::Keyboard::KEY_TAB;
    __keymap[Qt::Key_Backspace]     = gplay::Keyboard::KEY_BACKSPACE;
    __keymap[Qt::Key_Return]        = gplay::Keyboard::KEY_RETURN;
    __keymap[Qt::Key_Enter]         = gplay::Keyboard::KEY_KP_ENTER;
    __keymap[Qt::Key_Insert]        = gplay::Keyboard::KEY_INSERT;
    __keymap[Qt::Key_Delete]        = gplay::Keyboard::KEY_DELETE;
    __keymap[Qt::Key_Pause]         = gplay::Keyboard::KEY_PAUSE;
    __keymap[Qt::Key_Print]         = gplay::Keyboard::KEY_PRINT;
    __keymap[Qt::Key_SysReq]        = gplay::Keyboard::KEY_SYSREQ;
    __keymap[Qt::Key_Home]          = gplay::Keyboard::KEY_HOME;
    __keymap[Qt::Key_End]           = gplay::Keyboard::KEY_END;
    __keymap[Qt::Key_Left]          = gplay::Keyboard::KEY_LEFT_ARROW;
    __keymap[Qt::Key_Right]         = gplay::Keyboard::KEY_RIGHT_ARROW;
    __keymap[Qt::Key_Up]            = gplay::Keyboard::KEY_UP_ARROW;
    __keymap[Qt::Key_Down]          = gplay::Keyboard::KEY_DOWN_ARROW;
    __keymap[Qt::Key_PageUp]        = gplay::Keyboard::KEY_PG_UP;
    __keymap[Qt::Key_PageDown]      = gplay::Keyboard::KEY_PG_DOWN;
    __keymap[Qt::Key_Shift]         = gplay::Keyboard::KEY_SHIFT;
    __keymap[Qt::Key_Control]       = gplay::Keyboard::KEY_CTRL;
    __keymap[Qt::Key_Alt]           = gplay::Keyboard::KEY_ALT;
    __keymap[Qt::Key_CapsLock]      = gplay::Keyboard::KEY_CAPS_LOCK;
    __keymap[Qt::Key_NumLock]       = gplay::Keyboard::KEY_NUM_LOCK;
    __keymap[Qt::Key_ScrollLock]    = gplay::Keyboard::KEY_SCROLL_LOCK;
    __keymap[Qt::Key_F1]            = gplay::Keyboard::KEY_F1;
    __keymap[Qt::Key_F2]            = gplay::Keyboard::KEY_F2;
    __keymap[Qt::Key_F3]            = gplay::Keyboard::KEY_F3;
    __keymap[Qt::Key_F4]            = gplay::Keyboard::KEY_F4;
    __keymap[Qt::Key_F5]            = gplay::Keyboard::KEY_F5;
    __keymap[Qt::Key_F6]            = gplay::Keyboard::KEY_F6;
    __keymap[Qt::Key_F7]            = gplay::Keyboard::KEY_F7;
    __keymap[Qt::Key_F8]            = gplay::Keyboard::KEY_F8;
    __keymap[Qt::Key_F9]            = gplay::Keyboard::KEY_F9;
    __keymap[Qt::Key_F10]           = gplay::Keyboard::KEY_F10;
    __keymap[Qt::Key_F11]           = gplay::Keyboard::KEY_F11;
    __keymap[Qt::Key_F12]           = gplay::Keyboard::KEY_F12;
    __keymap[Qt::Key_Menu]          = gplay::Keyboard::KEY_MENU;

    __keymap[Qt::Key_A]             = gplay::Keyboard::KEY_A;
    __keymap[Qt::Key_B]             = gplay::Keyboard::KEY_B;
    __keymap[Qt::Key_C]             = gplay::Keyboard::KEY_C;
    __keymap[Qt::Key_D]             = gplay::Keyboard::KEY_D;
    __keymap[Qt::Key_E]             = gplay::Keyboard::KEY_E;
    __keymap[Qt::Key_F]             = gplay::Keyboard::KEY_F;
    __keymap[Qt::Key_G]             = gplay::Keyboard::KEY_G;
    __keymap[Qt::Key_H]             = gplay::Keyboard::KEY_H;
    __keymap[Qt::Key_I]             = gplay::Keyboard::KEY_I;
    __keymap[Qt::Key_J]             = gplay::Keyboard::KEY_J;
    __keymap[Qt::Key_K]             = gplay::Keyboard::KEY_K;
    __keymap[Qt::Key_L]             = gplay::Keyboard::KEY_L;
    __keymap[Qt::Key_M]             = gplay::Keyboard::KEY_M;
    __keymap[Qt::Key_N]             = gplay::Keyboard::KEY_N;
    __keymap[Qt::Key_O]             = gplay::Keyboard::KEY_O;
    __keymap[Qt::Key_P]             = gplay::Keyboard::KEY_P;
    __keymap[Qt::Key_Q]             = gplay::Keyboard::KEY_Q;
    __keymap[Qt::Key_R]             = gplay::Keyboard::KEY_R;
    __keymap[Qt::Key_S]             = gplay::Keyboard::KEY_S;
    __keymap[Qt::Key_T]             = gplay::Keyboard::KEY_T;
    __keymap[Qt::Key_U]             = gplay::Keyboard::KEY_U;
    __keymap[Qt::Key_V]             = gplay::Keyboard::KEY_V;
    __keymap[Qt::Key_W]             = gplay::Keyboard::KEY_W;
    __keymap[Qt::Key_X]             = gplay::Keyboard::KEY_X;
    __keymap[Qt::Key_Y]             = gplay::Keyboard::KEY_Y;
    __keymap[Qt::Key_Z]             = gplay::Keyboard::KEY_Z;

    __keymap[Qt::Key_0]             = gplay::Keyboard::KEY_ZERO;
    __keymap[Qt::Key_1]             = gplay::Keyboard::KEY_ONE;
    __keymap[Qt::Key_2]             = gplay::Keyboard::KEY_TWO;
    __keymap[Qt::Key_3]             = gplay::Keyboard::KEY_THREE;
    __keymap[Qt::Key_4]             = gplay::Keyboard::KEY_FOUR;
    __keymap[Qt::Key_5]             = gplay::Keyboard::KEY_FIVE;
    __keymap[Qt::Key_6]             = gplay::Keyboard::KEY_SIX;
    __keymap[Qt::Key_7]             = gplay::Keyboard::KEY_SEVEN;
    __keymap[Qt::Key_8]             = gplay::Keyboard::KEY_EIGHT;
    __keymap[Qt::Key_9]             = gplay::Keyboard::KEY_NINE;
}

gplay::Keyboard::Key __convertQtKeyToGplay(Qt::Key qtKey)
{
    gplay::Keyboard::Key gplayKey = __keymap.value(Qt::Key(qtKey));

    if(gplayKey == 0)
        gplay::print("Warning: Key %d not mapped", qtKey);

    return gplayKey;
}



//------------------------------------------------------------------------------------------------------
// GPlayWidgetEventFilter
//------------------------------------------------------------------------------------------------------

bool GPlayWidgetEventFilter::eventFilter(QObject *watched, QEvent *event)
{
    switch (event->type())
    {
    case QEvent::MouseButtonPress:
        this->onMousePress(static_cast<QMouseEvent*>(event));
        break;

    case QEvent::MouseButtonRelease:
        this->onMouseRelease(static_cast<QMouseEvent*>(event));
        break;

    case QEvent::MouseMove:
        this->onMouseMove(static_cast<QMouseEvent*>(event));
        break;

    case QEvent::Wheel:
        this->onWheel(static_cast<QWheelEvent*>(event));
        break;

    case QEvent::KeyPress:
    case QEvent::KeyRelease:
        this->onKeyEvent(static_cast<QKeyEvent*>(event));
        break;

    case QEvent::Resize:
        {
            QResizeEvent* resizeEvent = static_cast<QResizeEvent*>(event);
            GplayDevice::getInstance()->resizeRenderView(resizeEvent->size().width(), resizeEvent->size().height());
        }
        break;

    default:
        break;
    }
    return QObject::eventFilter(watched, event);
}

void GPlayWidgetEventFilter::onWheel(QWheelEvent* event)
{
    if(ImGui::GetIO().WantCaptureMouse)
        return;

    int wheelDelta = event->delta()/120;
    gplay::Platform::mouseEventInternal(gplay::Mouse::MOUSE_WHEEL, 0, 0, wheelDelta);
}

void GPlayWidgetEventFilter::onMousePress(QMouseEvent* event)
{
    if(ImGui::GetIO().WantCaptureMouse)
        return;

    gplay::Mouse::MouseEvent mouseEvt;
    switch (event->button())
    {
    case Qt::LeftButton:
        mouseEvt = gplay::Mouse::MOUSE_PRESS_LEFT_BUTTON;
        break;
    case Qt::RightButton:
        mouseEvt = gplay::Mouse::MOUSE_PRESS_RIGHT_BUTTON;
        break;
    case Qt::MidButton:
        mouseEvt = gplay::Mouse::MOUSE_PRESS_MIDDLE_BUTTON;
        break;
    default:
        break;
    }

    int xMousePos = event->pos().x();
    int yMmousePos = event->pos().y();

    if (!gplay::Platform::mouseEventInternal(mouseEvt, xMousePos, yMmousePos, 0))
    {
        gplay::Platform::touchEventInternal(gplay::Touch::TOUCH_PRESS, xMousePos, yMmousePos, 0, true);
    }
}

void GPlayWidgetEventFilter::onMouseRelease(QMouseEvent* event)
{
    if(ImGui::GetIO().WantCaptureMouse)
        return;

    gplay::Mouse::MouseEvent mouseEvt;
    switch (event->button())
    {
    case Qt::LeftButton:
        mouseEvt = gplay::Mouse::MOUSE_RELEASE_LEFT_BUTTON;
        break;
    case Qt::RightButton:
        mouseEvt = gplay::Mouse::MOUSE_RELEASE_RIGHT_BUTTON;
        break;
    case Qt::MidButton:
        mouseEvt = gplay::Mouse::MOUSE_RELEASE_MIDDLE_BUTTON;
        break;
    default:
        break;
    }

    int xMousePos = event->pos().x();
    int yMmousePos = event->pos().y();

    if (!gplay::Platform::mouseEventInternal(mouseEvt, xMousePos, yMmousePos, 0))
    {
        gplay::Platform::touchEventInternal(gplay::Touch::TOUCH_RELEASE, xMousePos, yMmousePos, 0, true);
    }
}

void GPlayWidgetEventFilter::onMouseMove(QMouseEvent* event)
{
    if(ImGui::GetIO().WantCaptureMouse)
        return;

    int x = event->pos().x();
    int y = event->pos().y();

    if (__mouseCaptured)
    {
        /*// Convert to deltas
        x -= __mouseCapturePoint.x();
        y -= __mouseCapturePoint.y();

        __mouseCapturePoint = event->pos();

        // center mouse
        int x = __widget->width() / 2.0f;
        int y = __widget->height() / 2.0f;
        QPoint center(x,y);
        QCursor::setPos(__widget->mapToGlobal((center)));*/
    }

    if (!gplay::Platform::mouseEventInternal(gplay::Mouse::MOUSE_MOVE, x, y, 0))
    {
        if (event->button() == Qt::LeftButton)
        {
            gplay::Platform::touchEventInternal(gplay::Touch::TOUCH_MOVE, x, y, 0, true);
        }
    }
}

void GPlayWidgetEventFilter::onKeyEvent(QKeyEvent* event)
{
    if(ImGui::GetIO().WantTextInput)
        return;

    int key = __convertQtKeyToGplay( Qt::Key(event->key()) );
    gplay::Keyboard::KeyEvent keyEventType;

    switch (event->type())
    {
    case QKeyEvent::KeyPress:
        keyEventType = gplay::Keyboard::KeyEvent::KEY_PRESS;
        break;
    case QKeyEvent::KeyRelease:
        keyEventType = gplay::Keyboard::KeyEvent::KEY_RELEASE;
        break;
    default:
        Q_ASSERT(0);
        break;
    }
    gplay::Platform::keyEventInternal(keyEventType, key);


    // special mapping for F1, F2, F7
    if(event->type() == QKeyEvent::KeyPress)
    {
        switch (key)
        {
        case gplay::Keyboard::KEY_F1:
            gplay::Renderer::getInstance().toggleDebugStats();
            break;
        case gplay::Keyboard::KEY_F3:
            gplay::Renderer::getInstance().toggleWireFrame();
            break;
        case  gplay::Keyboard::KEY_F7:
            gplay::Renderer::getInstance().toggleVSync();
            break;
        }
    }
}



//-------------------------------------------------------------------------------------------------------------
// Custom gplay Platform implementation using Qt
//-------------------------------------------------------------------------------------------------------------

namespace gplay {


void updateWindowSize()
{
    Q_ASSERT(__widget);

    int width = __widget->width();
    int height = __widget->height();

    Renderer::getInstance().resize(width, height);
}


Platform::Platform(Game* game) :
    _game(game)
{
    Q_ASSERT(__app_argc);
    Q_ASSERT(__app_argv);

    __initKeyMap();
}

Platform::~Platform()
{
}

Platform* Platform::create(Game* game, void* externalWindow)
{
    Q_ASSERT(game);
    Q_ASSERT(externalWindow);

    FileSystem::setResourcePath("./");
    Platform* platform = new Platform(game);

    QWidget* renderWidget = (QWidget*)externalWindow;
    __widget = renderWidget;

    bgfx::PlatformData pd;
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    pd.ndt          = QX11Info::display();
    pd.nwh          = (void*)renderWidget->winId();
#elif BX_PLATFORM_WINDOWS
#endif // BX_PLATFORM_
    pd.context      = NULL;
    pd.backBuffer   = NULL;
    pd.backBufferDS = NULL;
    bgfx::setPlatformData(pd);


    bgfx::Init init;
    init.type = bgfx::RendererType::OpenGL;
    bgfx::init(init);

    uint32_t debug = BGFX_DEBUG_TEXT;
    uint32_t reset = BGFX_RESET_VSYNC;

    bgfx::reset(renderWidget->width(), renderWidget->height(), reset);
    bgfx::setDebug(debug);

    BGFXRenderer::initInstance();
    Renderer::getInstance().queryCaps();

    game->setVsync(true);
    game->setViewport(Rectangle(0,0,renderWidget->width(), renderWidget->height()));

    updateWindowSize();

    // Create ImGui context and init
    QtImGui::initialize(renderWidget);

    renderWidget->installEventFilter(&__widgetEventFilter);


    return platform;
}

void Platform::start()
{
    GP_ASSERT(_game);

    // Get the initial time.
    __timeStart = std::chrono::high_resolution_clock::now();
    __timeAbsolute = 0L;

    // Run the game.
    _game->run();
}

void Platform::stop()
{
    // shutdow bgfx
    bgfx::shutdown();
}

void Platform::frame()
{
    // nothing here, see: GplayDevice
}

int Platform::processEvents()
{
    // nothing here, see: GPlayWidgetEventFilter
    return 1;
}

void Platform::swapBuffers()
{
    Renderer::getInstance().endFrame();
}

void Platform::signalShutdown()
{
    GP_ERROR("Fix me !");
}

bool Platform::canExit()
{
    return true;
}

unsigned int Platform::getDisplayWidth()
{
    return __widget->width();
}

unsigned int Platform::getDisplayHeight()
{
    return __widget->height();
}

double Platform::getAbsoluteTime()
{
    auto now = std::chrono::high_resolution_clock::now();
    typedef std::chrono::duration<double, std::milli> duration;
    duration elapsed = now - __timeStart;
    __timeAbsolute = elapsed.count();
    return __timeAbsolute;
}

void Platform::setAbsoluteTime(double time)
{
    __timeAbsolute = time;
}

bool Platform::isVsync()
{
    return Renderer::getInstance().isVSync();
}

void Platform::setVsync(bool enable)
{
     Renderer::getInstance().setVSync(enable);
}

void Platform::sleep(long ms)
{
    QThread::sleep(ms);
}

void Platform::setMultiSampling(bool enabled)
{
    GP_ERROR("Fix me !");
}

bool Platform::isMultiSampling()
{
    GP_ERROR("Fix me !");
    return false;
}

void Platform::setMultiTouch(bool enabled)
{
}

bool Platform::isMultiTouch()
{
    return false;
}

bool Platform::hasMouse()
{
    return true;
}

void Platform::setMouseCaptured(bool captured)
{
    if (captured != __mouseCaptured)
    {
        if (captured)
        {
            // Hide the cursor and warp it to the center of the screen
            //__mouseCapturePointX = getDisplayWidth() / 2.0f;
            //__mouseCapturePointY = getDisplayHeight() / 2.0f;

            __mouseCapturePoint = QCursor::pos();

            __widget->grabMouse();
            setCursorVisible(false);

            /*int x = __widget->width() / 2.0f;
            int y = __widget->height() / 2.0f;
            QPoint center(x,y);
            QCursor::setPos(__widget->mapToGlobal((center)));*/
        }
        else
        {
            // recenter cursor
            int x = __widget->width() / 2.0f;
            int y = __widget->height() / 2.0f;
            QPoint center(x,y);
            QCursor::setPos(__widget->mapToGlobal((center)));

            // Restore cursor
            __widget->releaseMouse();
            setCursorVisible(true);
        }

        __mouseCaptured = captured;
    }
}

bool Platform::isMouseCaptured()
{
    return __mouseCaptured;
}

void Platform::setCursorVisible(bool visible)
{
    if (visible != __cursorVisible)
    {
        if (visible==false)
        {
            QCursor cursor(Qt::BlankCursor);
            QApplication::setOverrideCursor(cursor);
            QApplication::changeOverrideCursor(cursor);
        }
        else
        {
            QCursor cursor(Qt::ArrowCursor);
            QApplication::setOverrideCursor(cursor);
            QApplication::changeOverrideCursor(cursor);
        }

        __cursorVisible = visible;
    }
}

bool Platform::isCursorVisible()
{
    return __cursorVisible;
}

bool Platform::hasAccelerometer()
{
    return false;
}

void Platform::getAccelerometerValues(float* pitch, float* roll)
{
    *pitch = 0;
    *roll = 0;
}

void Platform::getSensorValues(float* accelX, float* accelY, float* accelZ, float* gyroX, float* gyroY, float* gyroZ)
{
    GP_ERROR("Fix me !");
}

void Platform::setArguments(int* argc, char*** argv)
{
    __app_argc = *argc;
    __app_argv = *argv;
}

void Platform::getArguments(int* argc, char*** argv)
{
    if (argc)
        *argc = __app_argc;
    if (argv)
        *argv = __app_argv;
}

void Platform::displayKeyboard(bool display)
{

}

bool Platform::isGestureSupported(Gesture::GestureEvent evt)
{
    GP_ERROR("Fix me !");
}

void Platform::registerGesture(Gesture::GestureEvent evt)
{
    GP_ERROR("Fix me !");
}

void Platform::unregisterGesture(Gesture::GestureEvent evt)
{
    GP_ERROR("Fix me !");
}

bool Platform::isGestureRegistered(Gesture::GestureEvent evt)
{
    GP_ERROR("Fix me !");
    return false;
}

bool Platform::launchURL(const char* url)
{
    GP_ERROR("Fix me !");
    return false;
}

std::string Platform::displayFileDialog(size_t mode, const char* title, const char* filterDescription, const char* filterExtensions, const char* initialDirectory)
{
    GP_ERROR("Fix me !");
    return "";
}

void Platform::pollGamepadState(Gamepad* gamepad)
{
    GP_ERROR("Fix me !");
}

void Platform::setWindowSize(int width, int height)
{
    updateWindowSize();
    resizeEventInternal(width, height);
}

void* Platform::getWindowHandle()
{
    return __widget;
}



} //end namespace gplay
