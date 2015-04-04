#include "input_handler.h"
#include "input_dialog.h"

InputHandler::InputHandler()
{
    SDL_Init(SDL_INIT_JOYSTICK);

    SDL_JoystickEventState(SDL_ENABLE);

    _num_joysticks = SDL_NumJoysticks();

    for (int i = 0; i < _num_joysticks; i++)
    {
        _joysticks.push_back(SDL_JoystickOpen(i));
    }

    // This must be set in order to detect joystick input
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
}

InputHandler::~InputHandler()
{
    for (auto joysticks_it = _joysticks.begin(); joysticks_it != _joysticks.end(); ++joysticks_it)
        SDL_JoystickClose(*joysticks_it);
}

QStringList InputHandler::joystickNames()
{
    QStringList joystick_names;

    for (int i = 0; i < _num_joysticks; i++)
    {
        joystick_names.push_back(SDL_JoystickName(_joysticks.at(i)));
    }

    return joystick_names;
}

void InputHandler::addKeyboardEvent(SDL_EventType type, int key)
{
    SDL_Event key_event;
    key_event.type = type;
    key_event.key.keysym.sym = SDLKeyFromQtKey(key);

    SDL_PushEvent(&key_event);
}

void InputHandler::flushEvents()
{
    SDL_Event input_events;
    while(SDL_PollEvent(&input_events) != 0) {};
}

void InputHandler::clearJoystickInputs()
{
    _joy_map_down_1.clear();
    _joy_map_up_1.clear();
}

QString InputHandler::captureButton(InputDialog* dialog, ButtonType button)
{
    // Wait for joystick or keyboard input
    SDL_Event input_events;
    while(SDL_PollEvent(&input_events) == 0) { dialog->forceProcessEvents(); };

    // TODO: Add keyboard input
    // TODO: Cancel Assignment on 'Escape'

    std::string input_type = "INVALID";

    bool found = false;
    bool is_key = false;
    while(SDL_PollEvent(&input_events) != 0 || !found)
    {
        switch(input_events.type)
        {
        // Keyboard
        case SDL_KEYDOWN:
            {
                input_type = "key_";

                if (button == Up)
                {
                    _keyboard_map_down[input_events.key.keysym.sym] = 0x10;
                }
                else if (button == Down)
                {
                    _keyboard_map_down[input_events.key.keysym.sym] = 0x20;
                }
                else if (button == Left)
                {
                    _keyboard_map_down[input_events.key.keysym.sym] = 0x40;
                }
                else if (button == Right)
                {
                    _keyboard_map_down[input_events.key.keysym.sym] = 0x80;
                }
                else if (button == Select)
                {
                    _keyboard_map_down[input_events.key.keysym.sym] = 0x04;
                }
                else if (button == Start)
                {
                    _keyboard_map_down[input_events.key.keysym.sym] = 0x08;
                }
                else if (button == B)
                {
                    _keyboard_map_down[input_events.key.keysym.sym] = 0x02;
                }
                else if (button == A)
                {
                    _keyboard_map_down[input_events.key.keysym.sym] = 0x01;
                }
            }
            break;
        case SDL_KEYUP:
            {
                input_type = "key_";

                if (button == Up)
                {
                    _keyboard_map_up[input_events.key.keysym.sym] = 0xEF;
                }
                else if (button == Down)
                {
                    _keyboard_map_up[input_events.key.keysym.sym] = 0xDF;
                }
                else if (button == Left)
                {
                    _keyboard_map_up[input_events.key.keysym.sym] = 0xBF;
                }
                else if (button == Right)
                {
                    _keyboard_map_up[input_events.key.keysym.sym] = 0x7F;
                }
                else if (button == Select)
                {
                    _keyboard_map_up[input_events.key.keysym.sym] = 0xFB;
                }
                else if (button == Start)
                {
                    _keyboard_map_up[input_events.key.keysym.sym] = 0xF7;
                }
                else if (button == B)
                {
                    _keyboard_map_up[input_events.key.keysym.sym] = 0xFD;
                }
                else if (button == A)
                {
                    _keyboard_map_up[input_events.key.keysym.sym] = 0xFE;
                }

                found = true;
                is_key = true;
            }
            break;

        // Joysticks
        case SDL_JOYBUTTONDOWN:
            {
                input_type = "joy_";

                if (button == Up)
                {
                    _joy_map_down_1[input_events.jbutton.button] = 0x10;
                }
                else if (button == Down)
                {
                    _joy_map_down_1[input_events.jbutton.button] = 0x20;
                }
                else if (button == Left)
                {
                    _joy_map_down_1[input_events.jbutton.button] = 0x40;
                }
                else if (button == Right)
                {
                    _joy_map_down_1[input_events.jbutton.button] = 0x80;
                }
                else if (button == Select)
                {
                    _joy_map_down_1[input_events.jbutton.button] = 0x04;
                }
                else if (button == Start)
                {
                    _joy_map_down_1[input_events.jbutton.button] = 0x08;
                }
                else if (button == B)
                {
                    _joy_map_down_1[input_events.jbutton.button] = 0x02;
                }
                else if (button == A)
                {
                    _joy_map_down_1[input_events.jbutton.button] = 0x01;
                }
            }
            break;
        case SDL_JOYBUTTONUP:
            {
                input_type = "joy_";

                if (button == Up)
                {
                    _joy_map_up_1[input_events.jbutton.button] = 0xEF;
                }
                else if (button == Down)
                {
                    _joy_map_up_1[input_events.jbutton.button] = 0xDF;
                }
                else if (button == Left)
                {
                    _joy_map_up_1[input_events.jbutton.button] = 0xBF;
                }
                else if (button == Right)
                {
                    _joy_map_up_1[input_events.jbutton.button] = 0x7F;
                }
                else if (button == Select)
                {
                    _joy_map_up_1[input_events.jbutton.button] = 0xFB;
                }
                else if (button == Start)
                {
                    _joy_map_up_1[input_events.jbutton.button] = 0xF7;
                }
                else if (button == B)
                {
                    _joy_map_up_1[input_events.jbutton.button] = 0xFD;
                }
                else if (button == A)
                {
                    _joy_map_up_1[input_events.jbutton.button] = 0xFE;
                }

                found = true;
            }
            break;
        case SDL_JOYAXISMOTION:
            {
            }
            break;
        case SDL_HAT_DOWN:
            {
                if (input_events.jhat.value & SDL_HAT_LEFT)
                {
                }
            }
            break;
        case SDL_HAT_UP:
            {
                if (input_events.jhat.value & SDL_HAT_LEFT)
                {
                }
            }
            break;
        default:
            break;
        }

        if (found)
            break;
    }

    QString ret;

    if (is_key)
        ret = QString::fromStdString(input_type) + QString(SDL_GetKeyName(input_events.key.keysym.sym));
    else
        ret = (QString::fromStdString(input_type) + QString::number(input_events.jbutton.which) + "_btn" + QString::number(input_events.jbutton.button));

    return ret;
}

void InputHandler::handleSDLJoystickEvents(std::vector<unsigned char>& button_down_events, std::vector<unsigned char>& button_up_events)
{
    SDL_Event input_events;

    while(SDL_PollEvent(&input_events) != 0)
    {
        switch(input_events.type)
        {
        // Keyboard
        case SDL_KEYDOWN:
            {
                if (_keyboard_map_down.find(input_events.key.keysym.sym) != _keyboard_map_down.end())
                    button_down_events.push_back(_keyboard_map_down[input_events.key.keysym.sym]);
            }
            break;
        case SDL_KEYUP:
            {
                if (_keyboard_map_up.find(input_events.key.keysym.sym) != _keyboard_map_up.end())
                    button_up_events.push_back(_keyboard_map_up[input_events.key.keysym.sym]);
            }
            break;

        // Joysticks
        case SDL_JOYBUTTONDOWN:
            {
                if (_joy_map_down_1.find(input_events.jbutton.button) != _joy_map_down_1.end())
                    button_down_events.push_back(_joy_map_down_1[input_events.jbutton.button]);
            }
            break;
        case SDL_JOYBUTTONUP:
            {
                if (_joy_map_up_1.find(input_events.jbutton.button) != _joy_map_up_1.end())
                    button_up_events.push_back(_joy_map_up_1[input_events.jbutton.button]);
            }
            break;
        case SDL_HAT_DOWN:
            {
                if (input_events.jhat.value & SDL_HAT_LEFT)
                {
                }
            }
            break;
        case SDL_HAT_UP:
            {
                if (input_events.jhat.value & SDL_HAT_LEFT)
                {
                }
            }
            break;
        default:
            break;
        }
    }
}

SDL_Keycode InputHandler::SDLKeyFromQtKey(int key)
{
    Qt::Key_Enter;

    switch (key)
    {
    // Numbers
    case Qt::Key_0:
        return SDLK_0;
    case Qt::Key_1:
        return SDLK_1;
    case Qt::Key_2:
        return SDLK_2;
    case Qt::Key_3:
        return SDLK_3;
    case Qt::Key_4:
        return SDLK_4;
    case Qt::Key_5:
        return SDLK_5;
    case Qt::Key_6:
        return SDLK_6;
    case Qt::Key_7:
        return SDLK_7;
    case Qt::Key_8:
        return SDLK_8;
    case Qt::Key_9:
        return SDLK_9;

    // Alphabet
    case Qt::Key_A:
        return SDLK_a;
    case Qt::Key_B:
        return SDLK_b;
    case Qt::Key_C:
        return SDLK_c;
    case Qt::Key_D:
        return SDLK_d;
    case Qt::Key_E:
        return SDLK_e;
    case Qt::Key_F:
        return SDLK_f;
    case Qt::Key_G:
        return SDLK_g;
    case Qt::Key_H:
        return SDLK_h;
    case Qt::Key_I:
        return SDLK_i;
    case Qt::Key_J:
        return SDLK_j;
    case Qt::Key_K:
        return SDLK_k;
    case Qt::Key_L:
        return SDLK_l;
    case Qt::Key_M:
        return SDLK_m;
    case Qt::Key_N:
        return SDLK_n;
    case Qt::Key_O:
        return SDLK_o;
    case Qt::Key_P:
        return SDLK_p;
    case Qt::Key_Q:
        return SDLK_q;
    case Qt::Key_R:
        return SDLK_r;
    case Qt::Key_S:
        return SDLK_s;
    case Qt::Key_T:
        return SDLK_t;
    case Qt::Key_U:
        return SDLK_u;
    case Qt::Key_V:
        return SDLK_v;
    case Qt::Key_W:
        return SDLK_w;
    case Qt::Key_X:
        return SDLK_x;
    case Qt::Key_Y:
        return SDLK_y;
    case Qt::Key_Z:
        return SDLK_z;

    default:
        return SDLK_QUESTION;
    }

    //Key_Escape = 0x01000000,                // misc keys
    //Key_Tab = 0x01000001,
    //Key_Backtab = 0x01000002,
    //Key_Backspace = 0x01000003,
    //Key_Return = 0x01000004,
    //Key_Enter = 0x01000005,
    //Key_Insert = 0x01000006,
    //Key_Delete = 0x01000007,
    //Key_Pause = 0x01000008,
    //Key_Print = 0x01000009,
    //Key_SysReq = 0x0100000a,
    //Key_Clear = 0x0100000b,
    //Key_Home = 0x01000010,                // cursor movement
    //Key_End = 0x01000011,
    //Key_Left = 0x01000012,
    //Key_Up = 0x01000013,
    //Key_Right = 0x01000014,
    //Key_Down = 0x01000015,
    //Key_PageUp = 0x01000016,
    //Key_PageDown = 0x01000017,
    //Key_Shift = 0x01000020,                // modifiers
    //Key_Control = 0x01000021,
    //Key_Meta = 0x01000022,
    //Key_Alt = 0x01000023,
    //Key_CapsLock = 0x01000024,
    //Key_NumLock = 0x01000025,
    //Key_ScrollLock = 0x01000026

    //Key_F1 = 0x01000030,                // function keys
    //Key_F2 = 0x01000031,
    //Key_F3 = 0x01000032,
    //Key_F4 = 0x01000033,
    //Key_F5 = 0x01000034,
    //Key_F6 = 0x01000035,
    //Key_F7 = 0x01000036,
    //Key_F8 = 0x01000037,
    //Key_F9 = 0x01000038,
    //Key_F10 = 0x01000039,
    //Key_F11 = 0x0100003a,
    //Key_F12 = 0x0100003b

    //Key_Space = 0x20,                // 7 bit printable ASCII
    //Key_Exclam = 0x21,
    //Key_QuoteDbl = 0x22,
    //Key_NumberSign = 0x23,
    //Key_Dollar = 0x24,
    //Key_Percent = 0x25,
    //Key_Ampersand = 0x26,
    //Key_Apostrophe = 0x27,
    //Key_ParenLeft = 0x28,
    //Key_ParenRight = 0x29,
    //Key_Asterisk = 0x2a,
    //Key_Plus = 0x2b,
    //Key_Comma = 0x2c,
    //Key_Minus = 0x2d,
    //Key_Period = 0x2e,
    //Key_Slash = 0x2f,

    //Key_0 = 0x30,
    //Key_1 = 0x31,
    //Key_2 = 0x32,
    //Key_3 = 0x33,
    //Key_4 = 0x34,
    //Key_5 = 0x35,
    //Key_6 = 0x36,
    //Key_7 = 0x37,
    //Key_8 = 0x38,
    //Key_9 = 0x39,

    //Key_Colon = 0x3a,
    //Key_Semicolon = 0x3b,
    //Key_Less = 0x3c,
    //Key_Equal = 0x3d,
    //Key_Greater = 0x3e,
    //Key_Question = 0x3f,
    //Key_At = 0x40,

    //Key_A = 0x41,
    //Key_B = 0x42,
    //Key_C = 0x43,
    //Key_D = 0x44,
    //Key_E = 0x45,
    //Key_F = 0x46,
    //Key_G = 0x47,
    //Key_H = 0x48,
    //Key_I = 0x49,
    //Key_J = 0x4a,
    //Key_K = 0x4b,
    //Key_L = 0x4c,
    //Key_M = 0x4d,
    //Key_N = 0x4e,
    //Key_O = 0x4f,
    //Key_P = 0x50,
    //Key_Q = 0x51,
    //Key_R = 0x52,
    //Key_S = 0x53,
    //Key_T = 0x54,
    //Key_U = 0x55,
    //Key_V = 0x56,
    //Key_W = 0x57,
    //Key_X = 0x58,
    //Key_Y = 0x59,
    //Key_Z = 0x5a,

    //Key_BracketLeft = 0x5b,
    //Key_Backslash = 0x5c,
    //Key_BracketRight = 0x5d,
    //Key_AsciiCircum = 0x5e,
    //Key_Underscore = 0x5f,
    //Key_QuoteLeft = 0x60,
    //Key_BraceLeft = 0x7b,
    //Key_Bar = 0x7c,
    //Key_BraceRight = 0x7d,
    //Key_AsciiTilde = 0x7e,
}