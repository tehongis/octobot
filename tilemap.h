#ifndef TILEMAP_H
#define TILEMAP_H

#include <SDL2/SDL.h>
#include <vector>
#include <string>

class Tilemap {
private:
    SDL_Texture *spritesheet;
    SDL_Renderer *renderer;
    std::vector<std::vector<int>> tiles;
    int tileWidth;
    int tileHeight;
    int mapWidth;   // in tiles
    int mapHeight;  // in tiles
    int spritesheetCols;
    int spritesheetRows;
    static const int TILE_MARGIN = 1;
    
public:
    Tilemap(SDL_Renderer *renderer, const std::string &imagePath, 
            int tileW, int tileH, int mapW, int mapH);
    ~Tilemap();
    
    bool loadSpritesheet(const std::string &imagePath);
    bool loadMapFromArray(const int *mapData);
    void render(float offsetX = 0, float offsetY = 0);
    void setTile(int x, int y, int tileIndex);
    int getTile(int x, int y) const;
    int getMapWidth() const { return mapWidth; }
    int getMapHeight() const { return mapHeight; }
    // Helper to get position
    void clear();
    
    // Camera/viewport support for large maps
    void renderViewport(float cameraX, float cameraY, int screenWidth, int screenHeight);
    
    // Check if tile at position is solid (wall)
    bool isSolidTile(int x, int y) const;
    
    // Get tile type at world position (in pixels)
    int getTileAtWorldPos(float worldX, float worldY) const;
};

#endif // TILEMAP_H
