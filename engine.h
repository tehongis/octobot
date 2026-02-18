#ifndef ENGINE_H
#define ENGINE_H

#include <SDL2/SDL.h>

class Engine {
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    int width;
    int height;
    static Engine *instance;
    
    Engine();
    
public:
    ~Engine();
    static Engine* getInstance();
    bool init(const char *title, int width, int height);
    void cleanup();
    SDL_Renderer* getRenderer() const;
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    void setDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void clearScreen();
    void present();
};

// C API wrappers
bool engine_init(const char *title, int width, int height);
void engine_cleanup(void);
SDL_Renderer* engine_get_renderer(void);
void engine_set_draw_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void engine_clear_screen(void);
void engine_present(void);

#endif // ENGINE_H
