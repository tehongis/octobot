#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <cstdint>

struct Sprite {
    float x, y;
    float width, height;
    uint8_t r, g, b, a;
};

struct Circle {
    float x, y;
    float radius;
    uint8_t r, g, b, a;
};

// Drawing functions
void graphics_draw_rect(SDL_Renderer *renderer, float x, float y, float width, float height, 
                       uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void graphics_draw_filled_rect(SDL_Renderer *renderer, float x, float y, float width, float height,
                              uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void graphics_draw_circle(SDL_Renderer *renderer, float x, float y, float radius,
                         uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void graphics_draw_line(SDL_Renderer *renderer, float x1, float y1, float x2, float y2,
                       uint8_t r, uint8_t g, uint8_t b, uint8_t a);

#endif // GRAPHICS_H
