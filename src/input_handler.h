#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include "common.h"
#include "SDL.h"

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

class InputHandler : public QDialog
{
    Q_OBJECT

public:
    InputHandler();
    ~InputHandler();

    void flushEvents();
    QString captureButton(ButtonType button);
    void handleSDLJoystickEvents(std::vector<unsigned char>& button_down_events, std::vector<unsigned char>& button_up_events);

private:
    SDL_Event _joystick_event;
    SDL_Joystick* _joystick;

    std::map<Uint8, unsigned char> _joy_map_down_1;
    std::map<Uint8, unsigned char> _joy_map_up_1;
};

#endif
// INPUT_HANDLER_H
