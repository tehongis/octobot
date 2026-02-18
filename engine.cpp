#include "engine.h"
#include <iostream>

Engine* Engine::instance = nullptr;

Engine::Engine() : window(nullptr), renderer(nullptr), width(0), height(0) {}

Engine::~Engine() {
    cleanup();
}

Engine* Engine::getInstance() {
    if (!instance) {
        instance = new Engine();
    }
    return instance;
}

bool Engine::init(const char *title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    this->width = width;
    this->height = height;
    
    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(
        window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    std::cout << "Engine initialized: " << width << "x" << height << std::endl;
    return true;
}

void Engine::cleanup() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Quit();
    std::cout << "Engine cleaned up" << std::endl;
}

SDL_Renderer* Engine::getRenderer() const {
    return renderer;
}

void Engine::setDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void Engine::clearScreen() {
    SDL_RenderClear(renderer);
}

void Engine::present() {
    SDL_RenderPresent(renderer);
}

// C API wrappers for compatibility
bool engine_init(const char *title, int width, int height) {
    return Engine::getInstance()->init(title, width, height);
}

void engine_cleanup(void) {
    Engine::getInstance()->cleanup();
}

SDL_Renderer* engine_get_renderer(void) {
    return Engine::getInstance()->getRenderer();
}

void engine_set_draw_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    Engine::getInstance()->setDrawColor(r, g, b, a);
}

void engine_clear_screen(void) {
    Engine::getInstance()->clearScreen();
}

void engine_present(void) {
    Engine::getInstance()->present();
}

