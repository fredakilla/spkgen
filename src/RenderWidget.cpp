#include "RenderWidget.h"
#include "UrhoDevice.h"

#include <QResizeEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <SDL/SDL.h>

#include <Urho3D/Input/InputEvents.h>


//------------------------------------------------------------------------------------------------------
// key utilities to convert Qt key to SDL key
//------------------------------------------------------------------------------------------------------
static QMap<Qt::Key, SDL_Keycode> __keymap;
static void __initKeyMap();
static Uint16 __convertQtKeyModifierToSDL(Qt::KeyboardModifiers qtKeyModifiers);
static SDL_Keycode __convertQtKeyToSDL(Qt::Key qtKey);

//------------------------------------------------------------------------------------------------------
// map keys Qt/SDL
//------------------------------------------------------------------------------------------------------
void __initKeyMap()
{
    __keymap[Qt::Key_unknown]     = SDLK_UNKNOWN;
    __keymap[Qt::Key_Escape]      = SDLK_ESCAPE;
    __keymap[Qt::Key_Tab]         = SDLK_TAB;
    __keymap[Qt::Key_Backspace]   = SDLK_BACKSPACE;
    __keymap[Qt::Key_Return]      = SDLK_RETURN;
    __keymap[Qt::Key_Enter]       = SDLK_KP_ENTER;
    __keymap[Qt::Key_Insert]      = SDLK_INSERT;
    __keymap[Qt::Key_Delete]      = SDLK_DELETE;
    __keymap[Qt::Key_Pause]       = SDLK_PAUSE;
    __keymap[Qt::Key_Print]       = SDLK_PRINTSCREEN;
    __keymap[Qt::Key_SysReq]      = SDLK_SYSREQ;
    __keymap[Qt::Key_Home]        = SDLK_HOME;
    __keymap[Qt::Key_End]         = SDLK_END;
    __keymap[Qt::Key_Left]        = SDLK_LEFT;
    __keymap[Qt::Key_Right]       = SDLK_RIGHT;
    __keymap[Qt::Key_Up]          = SDLK_UP;
    __keymap[Qt::Key_Down]        = SDLK_DOWN;
    __keymap[Qt::Key_PageUp]      = SDLK_PAGEUP;
    __keymap[Qt::Key_PageDown]    = SDLK_PAGEDOWN;
    __keymap[Qt::Key_Shift]       = SDLK_LSHIFT;
    __keymap[Qt::Key_Control]     = SDLK_LCTRL;
    __keymap[Qt::Key_Alt]         = SDLK_LALT;
    __keymap[Qt::Key_CapsLock]    = SDLK_CAPSLOCK;
    __keymap[Qt::Key_NumLock]     = SDLK_NUMLOCKCLEAR;
    __keymap[Qt::Key_ScrollLock]  = SDLK_SCROLLLOCK;
    __keymap[Qt::Key_F1]          = SDLK_F1;
    __keymap[Qt::Key_F2]          = SDLK_F2;
    __keymap[Qt::Key_F3]          = SDLK_F3;
    __keymap[Qt::Key_F4]          = SDLK_F4;
    __keymap[Qt::Key_F5]          = SDLK_F5;
    __keymap[Qt::Key_F6]          = SDLK_F6;
    __keymap[Qt::Key_F7]          = SDLK_F7;
    __keymap[Qt::Key_F8]          = SDLK_F8;
    __keymap[Qt::Key_F9]          = SDLK_F9;
    __keymap[Qt::Key_F10]         = SDLK_F10;
    __keymap[Qt::Key_F11]         = SDLK_F11;
    __keymap[Qt::Key_F12]         = SDLK_F12;
    __keymap[Qt::Key_F13]         = SDLK_F13;
    __keymap[Qt::Key_F14]         = SDLK_F14;
    __keymap[Qt::Key_F15]         = SDLK_F15;
    __keymap[Qt::Key_Menu]        = SDLK_MENU;
    __keymap[Qt::Key_Help]        = SDLK_HELP;

    // A-Z
    for(int key='A'; key<='Z'; key++)
        __keymap[Qt::Key(key)] = key + 32;

    // 0-9
    for(int key='0'; key<='9'; key++)
        __keymap[Qt::Key(key)] = key;
}

//------------------------------------------------------------------------------------------------------
// get SDL key from Qt key
//------------------------------------------------------------------------------------------------------
SDL_Keycode __convertQtKeyToSDL(Qt::Key qtKey)
{
    SDL_Keycode sldKey = __keymap.value(Qt::Key(qtKey));

    if(sldKey == 0)
        qDebug("Warning: Key %d not mapped", qtKey);

    return sldKey;
}

//------------------------------------------------------------------------------------------------------
// get SDL key modifier from Qt key modifier
//------------------------------------------------------------------------------------------------------
Uint16 __convertQtKeyModifierToSDL(Qt::KeyboardModifiers qtKeyModifiers)
{
    Uint16 sdlModifiers = KMOD_NONE;

    if(qtKeyModifiers.testFlag(Qt::ShiftModifier))
        sdlModifiers |= KMOD_LSHIFT | KMOD_RSHIFT;
    if(qtKeyModifiers.testFlag(Qt::ControlModifier))
        sdlModifiers |= KMOD_LCTRL | KMOD_RCTRL;
    if(qtKeyModifiers.testFlag(Qt::AltModifier))
        sdlModifiers |= KMOD_LALT | KMOD_RALT;

    return sdlModifiers;
}


//------------------------------------------------------------------------------------------------------
// RenderWidget
//------------------------------------------------------------------------------------------------------

RenderWidget::RenderWidget(QWidget* parent) :
    QWidget(parent),
    Object(UrhoDevice::gUrhoContext),
    _lastMousePos(QPoint(0,0)),
    _mouseDownPos(QPoint(0,0))
{
    // init keymap for Qt > SDL keys conversion
    __initKeyMap();
}

RenderWidget::~RenderWidget()
{
}

void RenderWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    int width = event->size().width();
    int height = event->size().height();
    Q_EMIT (resized(width, height));
}

void RenderWidget::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);

    _lastMousePos = event->pos();
    _mouseDownPos = event->pos();

    /*SDL_Event sdlEvent;
    sdlEvent.type = SDL_MOUSEBUTTONDOWN;
    sdlEvent.button.state = SDL_PRESSED;
    if(event->buttons() & Qt::LeftButton)
        sdlEvent.button.button = SDL_BUTTON_LMASK;
    if(event->buttons() & Qt::RightButton)
        sdlEvent.button.button = SDL_BUTTON_RMASK;
    if(event->buttons() & Qt::MiddleButton)
        sdlEvent.button.button = SDL_BUTTON_LMASK;
    QPoint position = event->pos();
    sdlEvent.button.x = position.x();
    sdlEvent.button.y = position.y();
    SDL_PushEvent(&sdlEvent);*/


    Urho3D::VariantMap map;
    map[Urho3D::MouseButtonDown::P_BUTTON] = event->button();
    map[Urho3D::MouseButtonDown::P_BUTTONS] = event->buttons();
    map[Urho3D::MouseButtonDown::P_QUALIFIERS] = event->modifiers();
    SendEvent(Urho3D::E_MOUSEBUTTONDOWN, map);
}

void RenderWidget::mouseReleaseEvent(QMouseEvent* event)
{
    QWidget::mouseReleaseEvent(event);

    _lastMousePos = event->pos();

    /*SDL_Event sdlEvent;
    sdlEvent.type = SDL_MOUSEBUTTONUP;
    sdlEvent.button.state = SDL_RELEASED;
    if(event->buttons() & Qt::LeftButton)
        sdlEvent.button.button = SDL_BUTTON_LMASK;
    if(event->buttons() & Qt::RightButton)
        sdlEvent.button.button = SDL_BUTTON_RMASK;
    if(event->buttons() & Qt::MiddleButton)
        sdlEvent.button.button = SDL_BUTTON_LMASK;
    QPoint position = event->pos();
    sdlEvent.button.x = position.x();
    sdlEvent.button.y = position.y();
    SDL_PushEvent(&sdlEvent);*/


    Urho3D::VariantMap map;
    map[Urho3D::MouseButtonUp::P_BUTTON] = event->button();
    map[Urho3D::MouseButtonUp::P_BUTTONS] = event->buttons();
    map[Urho3D::MouseButtonUp::P_QUALIFIERS] = event->modifiers();
    SendEvent(Urho3D::E_MOUSEBUTTONUP, map);
}

void RenderWidget::mouseMoveEvent(QMouseEvent* event)
{
    QWidget::mouseMoveEvent(event);

    const QPoint move = _lastMousePos - event->pos();
    _lastMousePos = event->pos();

    SDL_Event sdlEvent;
    sdlEvent.type = SDL_MOUSEMOTION;
    Qt::MouseButton button = event->button();
    if( (button & Qt::LeftButton))
        sdlEvent.motion.state |= SDL_BUTTON_LMASK;
    if( (button & Qt::RightButton))
        sdlEvent.motion.state |= SDL_BUTTON_RMASK;
    if( (button & Qt::MidButton))
        sdlEvent.motion.state |= SDL_BUTTON_MMASK;
    QPoint position = event->pos();
    sdlEvent.motion.x = position.x();
    sdlEvent.motion.y = position.y();
    sdlEvent.motion.xrel = move.x();
    sdlEvent.motion.yrel = move.y();
    SDL_PushEvent(&sdlEvent);
}

void RenderWidget::wheelEvent(QWheelEvent* event)
{
    QWidget::wheelEvent(event);

    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;
    QPoint wheel;

    if (!numPixels.isNull())
    {
        wheel = numPixels;
    }
    else if (!numDegrees.isNull())
    {
        wheel = numDegrees / 15;
    }

    Urho3D::VariantMap map;
    map[Urho3D::MouseWheel::P_WHEEL] = -wheel.y();
    map[Urho3D::MouseWheel::P_BUTTONS] = event->buttons();
    map[Urho3D::MouseButtonUp::P_QUALIFIERS] = event->modifiers();
    SendEvent(Urho3D::E_MOUSEWHEEL, map);

    event->accept();
}

void RenderWidget::keyReleaseEvent(QKeyEvent* event)
{
    SDL_Event sdlEvent;
    sdlEvent.type = SDL_KEYUP;
    sdlEvent.key.keysym.sym = __convertQtKeyToSDL( Qt::Key(event->key()) );
    sdlEvent.key.keysym.mod = __convertQtKeyModifierToSDL(event->modifiers());
    SDL_PushEvent(&sdlEvent);
}

void RenderWidget::keyPressEvent(QKeyEvent* event)
{
    SDL_Event sdlEvent;
    sdlEvent.type = SDL_KEYDOWN;
    sdlEvent.key.keysym.sym = __convertQtKeyToSDL( Qt::Key(event->key()) );
    sdlEvent.key.keysym.mod = __convertQtKeyModifierToSDL(event->modifiers());
    SDL_PushEvent(&sdlEvent);
}

