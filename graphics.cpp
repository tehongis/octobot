#include "graphics.h"
#include <cmath>

void graphics_draw_rect(SDL_Renderer *renderer, float x, float y, float width, float height,
                       uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_Rect rect = {
        (int)x,
        (int)y,
        (int)width,
        (int)height
    };
    SDL_RenderDrawRect(renderer, &rect);
}

void graphics_draw_filled_rect(SDL_Renderer *renderer, float x, float y, float width, float height,
                              uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_Rect rect = {
        (int)x,
        (int)y,
        (int)width,
        (int)height
    };
    SDL_RenderFillRect(renderer, &rect);
}

void graphics_draw_circle(SDL_Renderer *renderer, float x, float y, float radius,
                         uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    
    float cx = x;
    float cy = y;
    float rad = radius;
    
    const float PI = 3.14159265359f;
    for (float angle = 0.0f; angle < 2 * PI; angle += 0.05f) {
        float x1 = cx + rad * cosf(angle);
        float y1 = cy + rad * sinf(angle);
        float x2 = cx + rad * cosf(angle + 0.05f);
        float y2 = cy + rad * sinf(angle + 0.05f);
        
        SDL_RenderDrawLine(renderer, (int)x1, (int)y1, (int)x2, (int)y2);
    }
}

void graphics_draw_line(SDL_Renderer *renderer, float x1, float y1, float x2, float y2,
                       uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderDrawLine(renderer, (int)x1, (int)y1, (int)x2, (int)y2);
}
