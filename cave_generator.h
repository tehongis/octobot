#ifndef CAVE_GENERATOR_H
#define CAVE_GENERATOR_H

#include <vector>
#include <random>

class CaveGenerator {
private:
    int width;
    int height;
    std::vector<std::vector<int>> map;
    std::mt19937 rng;
    
    // Perlin noise-like simplex approximation
    float perlinNoise(float x, float y, int seed);
    float interpolate(float a, float b, float t);
    float fade(float t);
    
public:
    CaveGenerator(int w, int h, unsigned int seed = 12345);
    ~CaveGenerator();
    
    // Generate cave using cellular automata
    void generateCellularAutomata(float fillProbability = 0.45f, int iterations = 5);
    
    // Generate cave using Perlin noise
    void generatePerlinNoise(float scale = 0.05f, float threshold = 0.4f);
    
    // Generate cave using random walk
    void generateRandomWalk(int walks = 50, int walkLength = 500);
    
    // Smooth the map using cellular automata rules
    void smoothMap(int iterations = 1);
    
    // Fill small isolated caverns
    void fillSmallCaverns(int minSize = 5);
    
    // Connect all floor regions to ensure playable tunnels
    void connectAllCaverns();
    
    // Ensure entrance at top center with passage to main cavern
    void ensureTopCenterEntrance();
    
    // Get the generated map
    const std::vector<std::vector<int>>& getMap() const { return map; }
    
    // Get map as flat array (for tilemap)
    std::vector<int> getMapFlat() const;
    
    // Tile types (these should correspond to spritesheet indices)
    static constexpr int TILE_WALL = 30*11+24;
    static constexpr int TILE_FLOOR = 30*2+19;
    // static constexpr int TILE_STONE = 30*12+24;
};

#endif // CAVE_GENERATOR_H
