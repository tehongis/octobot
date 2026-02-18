#include "tilemap.h"
#include <SDL2/SDL_image.h>
#include <iostream>
#include <cmath>
#include <algorithm>

Tilemap::Tilemap(SDL_Renderer *renderer, const std::string &imagePath,
                 int tileW, int tileH, int mapW, int mapH)
    : spritesheet(nullptr), renderer(renderer), tileWidth(tileW), tileHeight(tileH),
      mapWidth(mapW), mapHeight(mapH), spritesheetCols(0), spritesheetRows(0) {
    
    // Initialize tilemap with zeros
    tiles.resize(mapHeight, std::vector<int>(mapWidth, 0));
    
    // Load spritesheet
    if (!loadSpritesheet(imagePath)) {
        std::cerr << "Failed to load spritesheet: " << imagePath << std::endl;
    }
}

Tilemap::~Tilemap() {
    if (spritesheet) {
        SDL_DestroyTexture(spritesheet);
    }
}

bool Tilemap::loadSpritesheet(const std::string &imagePath) {
    // Initialize SDL_image if not already done
    int flags = IMG_INIT_PNG;
    if (!(IMG_Init(flags) & flags)) {
        std::cerr << "IMG_Init failed: " << IMG_GetError() << std::endl;
        return false;
    }
    
    // Load image surface
    SDL_Surface *surface = IMG_Load(imagePath.c_str());
    if (!surface) {
        std::cerr << "IMG_Load failed: " << IMG_GetError() << std::endl;
        return false;
    }
    
    // Create texture from surface
    spritesheet = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    if (!spritesheet) {
        std::cerr << "SDL_CreateTextureFromSurface failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Calculate number of tiles in spritesheet
    // For 492x305 image with 16x16 tiles and 1px margin:
    // cols = (492 - margin) / (16 + margin) = 491 / 17 = 28
    // rows = (305 - margin) / (16 + margin) = 304 / 17 = 17
    spritesheetCols = (492 - TILE_MARGIN) / (tileWidth + TILE_MARGIN);
    spritesheetRows = (305 - TILE_MARGIN) / (tileHeight + TILE_MARGIN);
    
    std::cout << "Spritesheet loaded: " << spritesheetCols << "x" << spritesheetRows 
              << " tiles (" << spritesheetCols * spritesheetRows << " total)" << std::endl;
    
    return true;
}

bool Tilemap::loadMapFromArray(const int *mapData) {
    if (!mapData) {
        std::cerr << "Map data is null" << std::endl;
        return false;
    }
    
    int idx = 0;
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            tiles[y][x] = mapData[idx++];
        }
    }
    
    return true;
}

void Tilemap::render(float offsetX, float offsetY) {
    if (!spritesheet) return;
    
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            int tileIndex = tiles[y][x];
            if (tileIndex < 0) continue; // Skip invalid tiles
            
            // Calculate which tile in spritesheet
            int spriteCol = tileIndex % spritesheetCols;
            int spriteRow = tileIndex / spritesheetCols;
            
            // Source rectangle in spritesheet
            SDL_Rect srcRect = {
                spriteCol * (tileWidth + TILE_MARGIN) + TILE_MARGIN,
                spriteRow * (tileHeight + TILE_MARGIN) + TILE_MARGIN,
                tileWidth,
                tileHeight
            };
            
            // Destination rectangle on screen
            SDL_Rect dstRect = {
                (int)(offsetX + x * tileWidth),
                (int)(offsetY + y * tileHeight),
                tileWidth,
                tileHeight
            };
            
            SDL_RenderCopy(renderer, spritesheet, &srcRect, &dstRect);
        }
    }
}

void Tilemap::setTile(int x, int y, int tileIndex) {
    if (x >= 0 && x < mapWidth && y >= 0 && y < mapHeight) {
        tiles[y][x] = tileIndex;
    }
}

int Tilemap::getTile(int x, int y) const {
    if (x >= 0 && x < mapWidth && y >= 0 && y < mapHeight) {
        return tiles[y][x];
    }
    return -1;
}

void Tilemap::clear() {
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            tiles[y][x] = 0;
        }
    }
}

void Tilemap::renderViewport(float cameraX, float cameraY, int screenWidth, int screenHeight) {
    if (!spritesheet) return;
    
    // Calculate visible tile range
    int startX = (int)(cameraX / tileWidth);
    int startY = (int)(cameraY / tileHeight);
    int endX = startX + (screenWidth / tileWidth) + 2;
    int endY = startY + (screenHeight / tileHeight) + 2;
    
    // Clamp to map boundaries
    startX = std::max(0, startX);
    startY = std::max(0, startY);
    endX = std::min(mapWidth, endX);
    endY = std::min(mapHeight, endY);
    
    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            int tileIndex = tiles[y][x];
            if (tileIndex < 0) continue;
            
            // Calculate which tile in spritesheet
            int spriteCol = tileIndex % spritesheetCols;
            int spriteRow = tileIndex / spritesheetCols;
            
            // Source rectangle in spritesheet
            SDL_Rect srcRect = {
                spriteCol * (tileWidth + TILE_MARGIN) + TILE_MARGIN,
                spriteRow * (tileHeight + TILE_MARGIN) + TILE_MARGIN,
                tileWidth,
                tileHeight
            };
            
            // Destination rectangle on screen (accounting for camera)
            SDL_Rect dstRect = {
                (int)(x * tileWidth - cameraX),
                (int)(y * tileHeight - cameraY),
                tileWidth,
                tileHeight
            };
            
            SDL_RenderCopy(renderer, spritesheet, &srcRect, &dstRect);
        }
    }
}

bool Tilemap::isSolidTile(int x, int y) const {
    // Check bounds
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight) {
        return true;  // Treat out of bounds as solid
    }
    
    int tileIndex = tiles[y][x];
    
    // Tile index 0 and 4 are TILE_WALL and TILE_STONE (solid)
    // Tile index 5 is TILE_FLOOR (walkable)
    return (tileIndex == 0 || tileIndex == 4);
}

int Tilemap::getTileAtWorldPos(float worldX, float worldY) const {
    // Convert world coordinates (pixels) to tile coordinates
    int tileX = (int)(worldX / tileWidth);
    int tileY = (int)(worldY / tileHeight);
    
    // Check bounds
    if (tileX < 0 || tileX >= mapWidth || tileY < 0 || tileY >= mapHeight) {
        return -1;  // Out of bounds
    }
    
    return tiles[tileY][tileX];
}
