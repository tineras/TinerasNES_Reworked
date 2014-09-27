#include "input_handler.h"

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

void InputHandler::flushEvents()
{
    while(SDL_PollEvent(&_joystick_event) != 0) {};
}

QString InputHandler::captureButton(ButtonType button)
{
    // Wait for joystick or keyboard input
    while(SDL_PollEvent(&_joystick_event) == 0) {};

    // TODO: Add keyboard input
    // TODO: Cancel Assignment on 'Escape'

    bool found = false;
    while(SDL_PollEvent(&_joystick_event) != 0 || !found)
    {
        switch(_joystick_event.type)
        {
        case SDL_JOYBUTTONDOWN:
            {
                if (button == Up)
                {
                    _joy_map_down_1.insert(std::make_pair(_joystick_event.jbutton.button, 0x10));
                }
                else if (button == Down)
                {
                    _joy_map_down_1.insert(std::make_pair(_joystick_event.jbutton.button, 0x20));
                }
                else if (button == Left)
                {
                    _joy_map_down_1.insert(std::make_pair(_joystick_event.jbutton.button, 0x40));
                }
                else if (button == Right)
                {
                    _joy_map_down_1.insert(std::make_pair(_joystick_event.jbutton.button, 0x80));
                }
                else if (button == Select)
                {
                    _joy_map_down_1.insert(std::make_pair(_joystick_event.jbutton.button, 0x04));
                }
                else if (button == Start)
                {
                    _joy_map_down_1.insert(std::make_pair(_joystick_event.jbutton.button, 0x08));
                }
                else if (button == B)
                {
                    _joy_map_down_1.insert(std::make_pair(_joystick_event.jbutton.button, 0x02));
                }
                else if (button == A)
                {
                    _joy_map_down_1.insert(std::make_pair(_joystick_event.jbutton.button, 0x01));
                }
            }
            break;
        case SDL_JOYBUTTONUP:
            {
                if (button == Up)
                {
                    _joy_map_up_1.insert(std::make_pair(_joystick_event.jbutton.button, 0xEF));
                }
                else if (button == Down)
                {
                    _joy_map_up_1.insert(std::make_pair(_joystick_event.jbutton.button, 0xDF));
                }
                else if (button == Left)
                {
                    _joy_map_up_1.insert(std::make_pair(_joystick_event.jbutton.button, 0xBF));
                }
                else if (button == Right)
                {
                    _joy_map_up_1.insert(std::make_pair(_joystick_event.jbutton.button, 0x7F));
                }
                else if (button == Select)
                {
                    _joy_map_up_1.insert(std::make_pair(_joystick_event.jbutton.button, 0xFB));
                }
                else if (button == Start)
                {
                    _joy_map_up_1.insert(std::make_pair(_joystick_event.jbutton.button, 0xF7));
                }
                else if (button == B)
                {
                    _joy_map_up_1.insert(std::make_pair(_joystick_event.jbutton.button, 0xFD));
                }
                else if (button == A)
                {
                    _joy_map_up_1.insert(std::make_pair(_joystick_event.jbutton.button, 0xFE));
                }

                found = true;
            }
            break;
        case SDL_JOYAXISMOTION:
            {
                break;
            }
        case SDL_HAT_DOWN:
            {
                if (_joystick_event.jhat.value & SDL_HAT_LEFT)
                {
                }
            }
        case SDL_HAT_UP:
            {
                if (_joystick_event.jhat.value & SDL_HAT_LEFT)
                {
                }
            }
        default:
            break;
        }

        if (found)
            break;
    }

    return QString("joy_" + QString::number(_joystick_event.jbutton.which) + "_btn" + QString::number(_joystick_event.jbutton.button));
}

void InputHandler::handleSDLJoystickEvents(std::vector<unsigned char>& button_down_events, std::vector<unsigned char>& button_up_events)
{
    while(SDL_PollEvent(&_joystick_event) != 0)
    {
        switch(_joystick_event.type)
        {
        case SDL_JOYBUTTONDOWN:
            {
                if (_joy_map_down_1.find(_joystick_event.jbutton.button) != _joy_map_down_1.end())
                    button_down_events.push_back(_joy_map_down_1[_joystick_event.jbutton.button]);
            }
            break;
        case SDL_JOYBUTTONUP:
            {
                if (_joy_map_up_1.find(_joystick_event.jbutton.button) != _joy_map_up_1.end())
                    button_up_events.push_back(_joy_map_up_1[_joystick_event.jbutton.button]);
            }
            break;
        case SDL_HAT_DOWN:
            {
                if (_joystick_event.jhat.value & SDL_HAT_LEFT)
                {
                }
            }
        case SDL_HAT_UP:
            {
                if (_joystick_event.jhat.value & SDL_HAT_LEFT)
                {
                }
            }
        default:
            break;
        }
    }
}
