# LeadRose - 2D Game Engine

A lightweight 2D game engine written in C using libSDL2 for graphics and libbox2d for physics simulation.

## Features

- **Graphics Rendering**: SDL2-based rendering with support for rectangles, circles, and lines
- **Physics Engine**: Box2D integration for realistic 2D physics simulation
- **Input Handling**: Keyboard input support for game controls
- **Frame Rate Control**: Smooth 60 FPS gameplay with delta-time updates
- **Modular Architecture**: Separate modules for engine, graphics, and physics

## Project Structure

```
LeadRose/
├── main.c           # Game entry point and main loop
├── engine.h/c       # Core engine initialization and SDL management
├── graphics.h/c     # Graphics rendering functions
├── physics.h/c      # Physics world and body management
├── makefile         # Build configuration
└── README.md        # This file
```

## Dependencies

### System Requirements
- GCC or Clang compiler
- Linux OS (Ubuntu/Debian recommended)
- pkg-config

### Libraries
- **libSDL2**: Graphics and windowing
- **libbox2d**: Physics simulation

## Installation

### Install Dependencies (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install -y libsdl2-dev libbox2d-dev pkg-config build-essential
```

Or use the Makefile:

```bash
make install-deps
```

## Building

### Basic Build
```bash
make
```

### Debug Build (with symbols and optimization disabled)
```bash
make debug
```

### Clean Build Artifacts
```bash
clean
make
```

## Running

### Run the Game
```bash
make run
```

### Run with Debugger (GDB)
```bash
make debug-run
```

## Game Controls

- **Left Arrow / A**: Move left
- **Right Arrow / D**: Move right
- **Space**: Jump
- **ESC/Close Button**: Exit game

## Code Overview

### Engine Module (engine.h/c)

Core engine initialization and management:

```c
bool engine_init(const char *title, int width, int height);
void engine_cleanup(void);
SDL_Renderer* engine_get_renderer(void);
```

### Graphics Module (graphics.h/c)

Rendering functions for various shapes:

```c
void graphics_draw_rect(SDL_Renderer *renderer, float x, float y, float width, float height, 
                       uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void graphics_draw_circle(SDL_Renderer *renderer, float x, float y, float radius,
                         uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void graphics_draw_line(SDL_Renderer *renderer, float x1, float y1, float x2, float y2,
                       uint8_t r, uint8_t g, uint8_t b, uint8_t a);
```

### Physics Module (physics.h/c)

Physics world and body management:

```c
PhysicsWorld* physics_create_world(float gravity_x, float gravity_y);
PhysicsBody* physics_create_box_body(PhysicsWorld *world, float x, float y, float width, float height,
                                     float density, b2BodyType type, const char *name);
PhysicsBody* physics_create_circle_body(PhysicsWorld *world, float x, float y, float radius,
                                       float density, b2BodyType type, const char *name);
void physics_apply_impulse(PhysicsBody *body, float ix, float iy);
```

## Example: Creating a Game Entity

```c
// Create physics world
PhysicsWorld *world = physics_create_world(0.0f, 9.81f);

// Create a dynamic box body (player)
PhysicsBody *player = physics_create_box_body(
    world, 
    400.0f, 100.0f,  // Position
    50.0f, 50.0f,    // Size
    1.0f,             // Density
    b2_dynamicBody,   // Type
    "player"          // Name
);

// Apply impulse (jump)
physics_apply_impulse(player, 0.0f, -15.0f);

// Get position for rendering
b2Vec2 pos = b2Body_GetPosition(player->id);
```

## Architecture

### Game Loop

The main game loop follows this pattern:

1. **Input**: Poll keyboard events and process player input
2. **Update**: Apply forces/impulses and step physics simulation
3. **Render**: Clear screen, draw entities, present frame
4. **Frame Timing**: Limit to ~60 FPS with delta-time

### Physics Integration

- Uses fixed timestep (0.016s @ 60 FPS) for physics simulation
- Accumulator pattern for smooth, stable physics
- Dynamic body for player with impulse-based movement
- Static body for ground/platforms

## Building a Custom Game

To extend this engine:

1. Add new physics bodies in `main.c`
2. Implement game-specific rendering logic
3. Handle additional input in the event loop
4. Add collision callbacks (extend physics module)
5. Create sprite/animation system (extend graphics module)

## Future Enhancements

- [ ] Sprite and texture loading
- [ ] Animation system
- [ ] Audio support (OpenAL)
- [ ] Particle system
- [ ] Collision callbacks and events
- [ ] Scene management
- [ ] Camera system
- [ ] 2D lighting
- [ ] Editor tools

## Troubleshooting

### Missing Dependencies Error

```
error: SDL2/SDL.h: No such file or directory
```

Solution: Install SDL2 development files:
```bash
sudo apt-get install libsdl2-dev
```

### Box2D Link Error

```
undefined reference to `b2CreateWorld'
```

Solution: Install Box2D development files:
```bash
sudo apt-get install libbox2d-dev
```

### Compilation Warning about Implicit Declaration

Make sure all header files are properly included in source files.

## Performance Tips

- Use fixed timestep for physics to ensure consistent behavior
- Batch SDL rendering operations
- Use hardware-accelerated rendering (SDL_RENDERER_ACCELERATED)
- Limit body count to prevent performance degradation
- Profile with `perf` or Valgrind for optimization

## License

MIT License - Feel free to use and modify this engine

## Contributing

Improvements and bug fixes are welcome! 

## References

- [SDL2 Documentation](https://wiki.libsdl.org/SDL2/Introduction)
- [Box2D Manual](https://box2d.org/documentation/)
- [Box2D C API](https://box2d.org/documentation/file/box2d_8h.html)
