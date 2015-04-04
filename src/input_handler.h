#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include "common.h"
#include "SDL.h"

class InputDialog;

enum ControlType
{
    Keyboard,
    Joystick_1,
    Joystick_2
};

enum ButtonType
{
    Up,
    Down,
    Left,
    Right,
    Select,
    Start,
    B,
    A
};

class InputHandler
{
public:
    InputHandler();
    ~InputHandler();

    QStringList InputHandler::joystickNames();

    void addKeyboardEvent(SDL_EventType type, int key);
    void flushEvents();
    void clearJoystickInputs();
    QString captureButton(InputDialog* dialog, ButtonType button);
    void handleSDLInputEvents(std::vector<unsigned char>& button_down_events, std::vector<unsigned char>& button_up_events);
    SDL_Keycode InputHandler::SDLKeyFromQtKey(int key);

private:
    int _num_joysticks;
    std::vector<SDL_Joystick*> _joysticks;

    std::map<SDL_Keycode, unsigned char> _keyboard_map_down;
    std::map<SDL_Keycode, unsigned char> _keyboard_map_up;
    std::map<Uint8, unsigned char> _joy_map_down_1;
    std::map<Uint8, unsigned char> _joy_map_up_1;
};

#endif
// INPUT_HANDLER_H
