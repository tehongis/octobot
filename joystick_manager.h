#ifndef JOYSTICK_MANAGER_H
#define JOYSTICK_MANAGER_H

#include <SDL2/SDL.h>
#include <cmath>

class JoystickManager {
private:
    SDL_Joystick *joystick;
    bool isConnected;
    
    // Dead zone threshold (0-32768)
    static constexpr int DEAD_ZONE = 8000;
    
public:
    JoystickManager();
    ~JoystickManager();
    
    // Initialize joystick (call once at startup)
    bool init();
    
    // Check if joystick is connected
    bool isJoystickConnected() const { return isConnected; }
    
    // Get analog stick input normalized to -1.0 to 1.0
    struct AnalogInput {
        float x;
        float y;
    };
    
    AnalogInput getLeftStick() const;
    AnalogInput getRightStick() const;
    
    // Get trigger values (0.0 to 1.0)
    float getLeftTrigger() const;
    float getRightTrigger() const;
    
    // Get rotation angle from stick input (0-360 degrees, or -1 if no input)
    float getRotationAngle() const;
    
    // Get throttle from triggers (0.0 to 1.0)
    float getThrottle() const;
    
    // Button states
    bool isButtonPressed(int button) const;
    bool isButtonDown(int button) const;
    
    // Get individual axis raw values
    int getAxisValue(int axis) const;
    
    // Joystick buttons (Xbox-style naming)
    static constexpr int BUTTON_A = 0;
    static constexpr int BUTTON_B = 1;
    static constexpr int BUTTON_X = 2;
    static constexpr int BUTTON_Y = 3;
    static constexpr int BUTTON_LB = 4;
    static constexpr int BUTTON_RB = 5;
    static constexpr int BUTTON_BACK = 6;
    static constexpr int BUTTON_START = 7;
    static constexpr int BUTTON_LEFT_STICK = 8;
    static constexpr int BUTTON_RIGHT_STICK = 9;
    
    // Joystick axes (SDL standard)
    static constexpr int AXIS_LEFT_X = 0;
    static constexpr int AXIS_LEFT_Y = 1;
    static constexpr int AXIS_RIGHT_X = 2;
    static constexpr int AXIS_RIGHT_Y = 3;
    static constexpr int AXIS_TRIGGER_LEFT = 4;
    static constexpr int AXIS_TRIGGER_RIGHT = 5;
};

#endif // JOYSTICK_MANAGER_H
