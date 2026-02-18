#include "joystick_manager.h"
#include <iostream>

JoystickManager::JoystickManager()
    : joystick(nullptr), isConnected(false) {
}

JoystickManager::~JoystickManager() {
    if (joystick) {
        SDL_JoystickClose(joystick);
    }
}

bool JoystickManager::init() {
    // Initialize joystick subsystem
    if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0) {
        std::cerr << "Failed to initialize joystick subsystem: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Check for connected joysticks
    int numJoysticks = SDL_NumJoysticks();
    if (numJoysticks <= 0) {
        std::cout << "No joysticks detected" << std::endl;
        isConnected = false;
        return true;  // Return true but mark as not connected
    }
    
    // Open first joystick
    joystick = SDL_JoystickOpen(0);
    if (!joystick) {
        std::cerr << "Failed to open joystick: " << SDL_GetError() << std::endl;
        isConnected = false;
        return false;
    }
    
    isConnected = true;
    std::cout << "Joystick connected: " << SDL_JoystickName(joystick) << std::endl;
    std::cout << "  Axes: " << SDL_JoystickNumAxes(joystick) << std::endl;
    std::cout << "  Buttons: " << SDL_JoystickNumButtons(joystick) << std::endl;
    std::cout << "  Hats: " << SDL_JoystickNumHats(joystick) << std::endl;
    
    return true;
}

JoystickManager::AnalogInput JoystickManager::getLeftStick() const {
    AnalogInput input = {0.0f, 0.0f};
    
    if (!isConnected) return input;
    
    int x = SDL_JoystickGetAxis(joystick, AXIS_LEFT_X);
    int y = SDL_JoystickGetAxis(joystick, AXIS_LEFT_Y);
    
    // Apply dead zone
    if (std::abs(x) > DEAD_ZONE) {
        input.x = x / 32768.0f;
    }
    if (std::abs(y) > DEAD_ZONE) {
        input.y = y / 32768.0f;
    }
    
    // Clamp to -1.0 to 1.0
    if (input.x > 1.0f) input.x = 1.0f;
    if (input.x < -1.0f) input.x = -1.0f;
    if (input.y > 1.0f) input.y = 1.0f;
    if (input.y < -1.0f) input.y = -1.0f;
    
    return input;
}

JoystickManager::AnalogInput JoystickManager::getRightStick() const {
    AnalogInput input = {0.0f, 0.0f};
    
    if (!isConnected) return input;
    
    int x = SDL_JoystickGetAxis(joystick, AXIS_RIGHT_X);
    int y = SDL_JoystickGetAxis(joystick, AXIS_RIGHT_Y);
    
    // Apply dead zone
    if (std::abs(x) > DEAD_ZONE) {
        input.x = x / 32768.0f;
    }
    if (std::abs(y) > DEAD_ZONE) {
        input.y = y / 32768.0f;
    }
    
    // Clamp to -1.0 to 1.0
    if (input.x > 1.0f) input.x = 1.0f;
    if (input.x < -1.0f) input.x = -1.0f;
    if (input.y > 1.0f) input.y = 1.0f;
    if (input.y < -1.0f) input.y = -1.0f;
    
    return input;
}

float JoystickManager::getLeftTrigger() const {
    if (!isConnected) return 0.0f;
    
    int value = SDL_JoystickGetAxis(joystick, AXIS_TRIGGER_LEFT);
    // Triggers are -32768 to 32767, convert to 0.0 to 1.0
    float normalized = (value + 32768.0f) / 65536.0f;
    if (normalized < 0.0f) normalized = 0.0f;
    if (normalized > 1.0f) normalized = 1.0f;
    return normalized;
}

float JoystickManager::getRightTrigger() const {
    if (!isConnected) return 0.0f;
    
    int value = SDL_JoystickGetAxis(joystick, AXIS_TRIGGER_RIGHT);
    // Triggers are -32768 to 32767, convert to 0.0 to 1.0
    float normalized = (value + 32768.0f) / 65536.0f;
    if (normalized < 0.0f) normalized = 0.0f;
    if (normalized > 1.0f) normalized = 1.0f;
    return normalized;
}

float JoystickManager::getRotationAngle() const {
    AnalogInput stick = getLeftStick();
    
    // If stick is in dead zone, return -1
    if (std::abs(stick.x) < 0.1f && std::abs(stick.y) < 0.1f) {
        return -1.0f;
    }
    
    // Calculate angle: atan2 returns radians from -PI to PI
    // We want 0-360 degrees where 0 is right, 90 is down, etc.
    float angle = std::atan2(stick.y, stick.x) * (180.0f / 3.14159265359f);
    
    // Convert to 0-360 range
    if (angle < 0) {
        angle += 360.0f;
    }
    
    return angle;
}

float JoystickManager::getThrottle() const {
    if (!isConnected) return 0.0f;
    
    // Use right trigger for throttle (0.0 to 1.0)
    return getRightTrigger();
}

bool JoystickManager::isButtonPressed(int button) const {
    if (!isConnected) return false;
    return SDL_JoystickGetButton(joystick, button) == 1;
}

bool JoystickManager::isButtonDown(int button) const {
    // Same as isButtonPressed for joystick
    return isButtonPressed(button);
}

int JoystickManager::getAxisValue(int axis) const {
    if (!isConnected) return 0;
    return SDL_JoystickGetAxis(joystick, axis);
}
