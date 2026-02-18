#include <iostream>
#include <cstdlib>
#include "engine.h"
#include "physics.h"
#include "graphics.h"
#include "tilemap.h"
#include "cave_generator.h"
#include "joystick_manager.h"
#include <SDL2/SDL.h>
#include <cmath>

#define WIDTH 256
#define HEIGHT 1024

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    // Initialize the game engine
    if (!engine_init("LeadRose - Procedural Cave Generator", 800, 600)) {
        std::cerr << "Failed to initialize engine" << std::endl;
        return 1;
    }

    // Create and generate a WIDTH*HEIGHT cave map
    std::cout << "Generating WIDTH*HEIGHT cave map..." << std::endl;
    CaveGenerator caveGen(WIDTH, HEIGHT, 42);
    
    // Choose generation method with parameters scaled for 8x larger map
    std::cout << "Using random walk generation (scaled for WIDTH*HEIGHT)..." << std::endl;
    caveGen.generateRandomWalk(300, 3000);  // 300 walks, 3000 steps each (8x larger)
    caveGen.smoothMap(3);                     // Smooth 3 times
    caveGen.fillSmallCaverns(50);             // Fill caverns smaller than 50 tiles
    caveGen.connectAllCaverns();              // Connect isolated caverns for playable tunnels
    caveGen.ensureTopCenterEntrance();        // Ensure entrance at top center with passage down

    // Create large tilemap and load generated map
    std::cout << "Creating WIDTH*HEIGHT tilemap..." << std::endl;
    Tilemap tilemap(engine_get_renderer(), "Spritesheet/roguelikeDungeon_transparent.png", 16, 16, WIDTH, HEIGHT);
    
    auto flatMap = caveGen.getMapFlat();
    tilemap.loadMapFromArray(flatMap.data());
    std::cout << "Tilemap loaded successfully" << std::endl;

    // Create the physics world with gravity pointing downward
    CPhysicsWorld *world = physics_create_world(0.0f, 9.8f);
    if (!world) {
        std::cerr << "Failed to create physics world" << std::endl;
        engine_cleanup();
        return 1;
    }

    // Create a dynamic box body (player) at top center
    // Map is WIDTH*HEIGHT tiles, each 16px, so center is at (4096*16, top*16) = (65536, ~80)
    PhysicsBody *player = physics_create_box_body(
        world, (WIDTH * 16.0f)/2, 80.0f, 12.0f, 16.0f,
        1.0f, BODY_DYNAMIC, "player"
    );

    // Create static ground body at bottom of map (far enough to not interfere with cave collisions)
    physics_create_box_body(
        world, 65536.0f, 131072.0f, 131072.0f, 10.0f,
        0.0f, BODY_STATIC, "ground"
    );

    // Initialize joystick manager for spaceship-style controls
    JoystickManager joystick;
    joystick.init();
    std::cout << "Joystick initialized: " << (joystick.isJoystickConnected() ? "Connected" : "No controller detected (keyboard fallback enabled)") << std::endl;

    // Game loop
    bool running = true;
    SDL_Event event;
    const Uint8 *keystate;
    float dt = 0.016f; // ~60 FPS
    float accumulator = 0.0f;
    const float FIXED_TIMESTEP = 0.016f;
    
    // Player rotation and physics state
    float playerRotation = 0.0f;  // 0-360 degrees
    const float MAX_THRUST = 500.0f;  // Maximum acceleration force
    const float ROTATION_SPEED = 360.0f;  // Degrees per second (for keyboard)
    
    // Camera position (follows player, starting at top center)
    float cameraX = (WIDTH * 16.0f)/2 - 400.0f;  // Center player horizontally on screen
    float cameraY = (HEIGHT * 16.0f)/2 - 300.0f;     // Player at top of screen

    std::cout << "Starting game loop..." << std::endl;
    std::cout << "Joystick Controls: Left stick for 360-degree rotation, Right trigger for rocket throttle" << std::endl;
    std::cout << "Keyboard Controls: A/D or Arrow Keys for rotation, W to throttle, S for reverse, ESC to quit" << std::endl;
    std::cout << "Physics: Gravity pulls player downward, thrust in facing direction propels spaceship" << std::endl;

    while (running) {
        Uint32 frameStart = SDL_GetTicks();

        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
            }
        }

        keystate = SDL_GetKeyboardState(NULL);

        // Handle joystick input for spaceship-style controls
        float throttle = 0.0f;
        float joystickRotationAngle = -1.0f;
        
        if (joystick.isJoystickConnected()) {
            // Get throttle from right trigger
            throttle = joystick.getThrottle();
            
            // Get rotation from left stick
            joystickRotationAngle = joystick.getRotationAngle();
            if (joystickRotationAngle >= 0.0f) {
                playerRotation = joystickRotationAngle;
            }
        }
        
        // Keyboard input fallback / override
        if (keystate[SDL_SCANCODE_LEFT] || keystate[SDL_SCANCODE_A]) {
            playerRotation -= ROTATION_SPEED * dt;
            if (playerRotation < 0.0f) playerRotation += 360.0f;
            throttle = std::max(throttle, 0.5f);  // Min throttle with keyboard input
        }
        if (keystate[SDL_SCANCODE_RIGHT] || keystate[SDL_SCANCODE_D]) {
            playerRotation += ROTATION_SPEED * dt;
            if (playerRotation >= 360.0f) playerRotation -= 360.0f;
            throttle = std::max(throttle, 0.5f);
        }
        if (keystate[SDL_SCANCODE_UP] || keystate[SDL_SCANCODE_W]) {
            throttle = std::max(throttle, 0.5f);
        }
        if (keystate[SDL_SCANCODE_DOWN] || keystate[SDL_SCANCODE_S]) {
            throttle = std::max(throttle, 0.2f);  // Light reverse
        }
        
        // Apply thrust force in the direction player is facing
        // Convert rotation angle to radians
        float radians = playerRotation * (3.14159265359f / 180.0f);
        float thrustX = std::cos(radians) * throttle * MAX_THRUST * dt;
        float thrustY = std::sin(radians) * throttle * MAX_THRUST * dt;
        
        if (throttle > 0.01f) {
            physics_apply_force(player, thrustX, thrustY);
        }

        // Update physics with fixed timestep
        accumulator += dt;
        while (accumulator >= FIXED_TIMESTEP) {
            physics_step_world(world, FIXED_TIMESTEP);
            accumulator -= FIXED_TIMESTEP;
        }

        // Get player position
        b2Vec2 playerPos = player->GetPosition();
        
        // Collision detection with cave walls
        // Check tiles around player bounding box
        float playerRadius = 8.0f;  // Half of player width/height
        
        // Check if player overlaps with solid tiles
        bool collidingLeft = tilemap.isSolidTile((int)((playerPos.x - playerRadius) / 16), (int)(playerPos.y / 16));
        bool collidingRight = tilemap.isSolidTile((int)((playerPos.x + playerRadius) / 16), (int)(playerPos.y / 16));
        bool collidingTop = tilemap.isSolidTile((int)(playerPos.x / 16), (int)((playerPos.y - playerRadius) / 16));
        bool collidingBottom = tilemap.isSolidTile((int)(playerPos.x / 16), (int)((playerPos.y + playerRadius) / 16));
        
        // Get current velocity
        b2Vec2 velocity = player->GetLinearVelocity();
        
        // Resolve collisions by dampening velocity in collision direction
        if (collidingLeft && velocity.x < 0) {
            velocity.x = 0;
        }
        if (collidingRight && velocity.x > 0) {
            velocity.x = 0;
        }
        if (collidingTop && velocity.y < 0) {
            velocity.y = 0;
        }
        if (collidingBottom && velocity.y > 0) {
            velocity.y = 0;
        }
        
        // Apply corrected velocity
        player->SetLinearVelocity(velocity);

        // Keep player centered on screen
        float targetCameraX = playerPos.x - 400.0f;  // 400 = 800/2, center horizontally
        float targetCameraY = playerPos.y - 300.0f;  // 300 = 600/2, center vertically
        
        // Clamp camera to map boundaries (WIDTH*HEIGHT tiles, 16px per tile)
        float mapPixelWidth = WIDTH * 16.0f; 
        float mapPixelHeight = HEIGHT * 16.0f;
        targetCameraX = std::max(0.0f, std::min(targetCameraX, mapPixelWidth - 800.0f));
        targetCameraY = std::max(0.0f, std::min(targetCameraY, mapPixelHeight - 600.0f));
        
        // Immediately center camera on player (no smoothing for instant centering)
        cameraX = targetCameraX;
        cameraY = targetCameraY;

        // Clear and render
        SDL_SetRenderDrawColor(engine_get_renderer(), 20, 20, 30, 255);
        SDL_RenderClear(engine_get_renderer());

        // Render tilemap with camera viewport
        tilemap.renderViewport(cameraX, cameraY, 800, 600);

        // Render player as a rotated triangle (spaceship-like)
        float halfWidth = 6.0f;
        float halfHeight = 8.0f;
        float screenX = playerPos.x - cameraX;
        float screenY = playerPos.y - cameraY;
        
        // Draw a triangle pointing in the direction of rotation
        float rad = playerRotation * (3.14159265359f / 180.0f);
        float cos_a = std::cos(rad);
        float sin_a = std::sin(rad);
        
        // Triangle vertices (nose, left, right)
        float nose_x = screenX + cos_a * halfHeight;
        float nose_y = screenY + sin_a * halfHeight;
        
        float left_x = screenX - sin_a * halfWidth - cos_a * halfHeight * 0.5f;
        float left_y = screenY + cos_a * halfWidth - sin_a * halfHeight * 0.5f;
        
        float right_x = screenX + sin_a * halfWidth - cos_a * halfHeight * 0.5f;
        float right_y = screenY - cos_a * halfWidth - sin_a * halfHeight * 0.5f;
        
        // Draw triangle
        graphics_draw_line(engine_get_renderer(), nose_x, nose_y, left_x, left_y, 100, 200, 255, 255);
        graphics_draw_line(engine_get_renderer(), left_x, left_y, right_x, right_y, 100, 200, 255, 255);
        graphics_draw_line(engine_get_renderer(), right_x, right_y, nose_x, nose_y, 100, 200, 255, 255);
        
        // Draw throttle indicator (flame behind ship when accelerating)
        if (throttle > 0.1f) {
            float flame_length = throttle * 10.0f;
            float flame_x = screenX - cos_a * halfHeight - cos_a * flame_length;
            float flame_y = screenY - sin_a * halfHeight - sin_a * flame_length;
            
            uint8_t flame_color = (uint8_t)(255 * throttle);
            graphics_draw_line(engine_get_renderer(), 
                screenX - cos_a * halfHeight, screenY - sin_a * halfHeight,
                flame_x, flame_y,
                255, flame_color, 0, 255);
        }

        SDL_RenderPresent(engine_get_renderer());

        // Frame rate limiting
        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < 16) {
            SDL_Delay(16 - frameTime);
        }
    }

    // Cleanup
    physics_destroy_world(world);
    engine_cleanup();
    
    std::cout << "Game ended successfully" << std::endl;

    return 0;
}
