#include "cave_generator.h"
#include <cmath>
#include <iostream>
#include <algorithm>

CaveGenerator::CaveGenerator(int w, int h, unsigned int seed)
    : width(w), height(h), rng(seed) {
    map.resize(height, std::vector<int>(width, TILE_WALL));
}

CaveGenerator::~CaveGenerator() {
}

float CaveGenerator::perlinNoise(float x, float y, int seed) {
    // Simple hash-based pseudo-random function
    auto hash = [](int n) {
        n = ((n << 13) ^ n);
        return (float)((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 0x7fffffff;
    };
    
    int xi = (int)x;
    int yi = (int)y;
    float xf = x - xi;
    float yf = y - yi;
    
    // Get hash values for corners
    float n00 = hash(xi + yi * 73856093 + seed);
    float n10 = hash(xi + 1 + yi * 73856093 + seed);
    float n01 = hash(xi + (yi + 1) * 73856093 + seed);
    float n11 = hash(xi + 1 + (yi + 1) * 73856093 + seed);
    
    // Interpolate
    float u = fade(xf);
    float v = fade(yf);
    
    float nx0 = interpolate(n00, n10, u);
    float nx1 = interpolate(n01, n11, u);
    float result = interpolate(nx0, nx1, v);
    
    return result;
}

float CaveGenerator::interpolate(float a, float b, float t) {
    return a + (b - a) * t;
}

float CaveGenerator::fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

void CaveGenerator::generateCellularAutomata(float fillProbability, int iterations) {
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    // Initial random fill
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            map[y][x] = dist(rng) < fillProbability ? TILE_WALL : TILE_FLOOR;
        }
    }
    
    // Apply cellular automata rules
    for (int iter = 0; iter < iterations; iter++) {
        std::vector<std::vector<int>> newMap = map;
        
        for (int y = 1; y < height - 1; y++) {
            for (int x = 1; x < width - 1; x++) {
                // Count neighboring walls (including diagonals)
                int wallCount = 0;
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        if (dx == 0 && dy == 0) continue;
                        if (map[y + dy][x + dx] == TILE_WALL) {
                            wallCount++;
                        }
                    }
                }
                
                // Apply rules: if >= 5 neighbors are walls, become wall; otherwise floor
                newMap[y][x] = (wallCount >= 5) ? TILE_WALL : TILE_FLOOR;
            }
        }
        
        map = newMap;
    }
}

void CaveGenerator::generatePerlinNoise(float scale, float threshold) {
    std::cout << "Generating Perlin noise-based cave (" << width << "x" << height << ")..." << std::endl;
    
    for (int y = 0; y < height; y++) {
        if (y % 128 == 0) {
            std::cout << "  Progress: " << (y * 100 / height) << "%" << std::endl;
        }
        
        for (int x = 0; x < width; x++) {
            float noise = perlinNoise(x * scale, y * scale, 12345);
            map[y][x] = (noise > threshold) ? TILE_WALL : TILE_FLOOR;
        }
    }
    
    std::cout << "Perlin noise generation complete" << std::endl;
}

void CaveGenerator::generateRandomWalk(int walks, int walkLength) {
    std::cout << "Generating random walk cave..." << std::endl;
    
    std::uniform_int_distribution<int> distX(1, width - 2);
    std::uniform_int_distribution<int> distY(1, height - 2);
    std::uniform_int_distribution<int> distDir(0, 3);
    std::uniform_int_distribution<int> distWidth(8, 16);  // Wider tunnel variation (8-16 tiles)
    
    // Start with all walls
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            map[y][x] = TILE_WALL;
        }
    }
    
    // Create tunnels through random walks
    for (int w = 0; w < walks; w++) {
        if (w % 10 == 0) {
            std::cout << "  Walk " << w << "/" << walks << std::endl;
        }
        
        int x = distX(rng);
        int y = distY(rng);
        int tunnelWidth = distWidth(rng);  // Randomize tunnel width for variety
        
        for (int step = 0; step < walkLength; step++) {
            // Carve out tunnel with variable width for wider passages
            for (int wy = -tunnelWidth/2; wy <= tunnelWidth/2; wy++) {
                for (int wx = -tunnelWidth/2; wx <= tunnelWidth/2; wx++) {
                    int cx = x + wx;
                    int cy = y + wy;
                    
                    if (cx >= 1 && cx < width - 1 && cy >= 1 && cy < height - 1) {
                        map[cy][cx] = TILE_FLOOR;
                    }
                }
            }
            
            // Random direction: 0=up, 1=down, 2=left, 3=right
            int dir = distDir(rng);
            switch (dir) {
                case 0: y = std::max(1, y - 1); break;
                case 1: y = std::min(height - 2, y + 1); break;
                case 2: x = std::max(1, x - 1); break;
                case 3: x = std::min(width - 2, x + 1); break;
            }
        }
    }
    
    std::cout << "Random walk generation complete" << std::endl;
}

void CaveGenerator::smoothMap(int iterations) {
    std::cout << "Smoothing map..." << std::endl;
    
    for (int iter = 0; iter < iterations; iter++) {
        std::vector<std::vector<int>> newMap = map;
        
        for (int y = 1; y < height - 1; y++) {
            for (int x = 1; x < width - 1; x++) {
                int floorCount = 0;
                int wallCount = 0;
                
                // Count neighbors
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        if (map[y + dy][x + dx] == TILE_FLOOR) {
                            floorCount++;
                        } else {
                            wallCount++;
                        }
                    }
                }
                
                // If more floors than walls in neighborhood, become floor
                newMap[y][x] = (floorCount > wallCount) ? TILE_FLOOR : TILE_WALL;
            }
        }
        
        map = newMap;
    }
    
    std::cout << "Smoothing complete" << std::endl;
}

void CaveGenerator::fillSmallCaverns(int minSize) {
    std::cout << "Filling small caverns..." << std::endl;
    
    std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (!visited[y][x] && map[y][x] == TILE_FLOOR) {
                // Flood fill to find cavern size
                std::vector<std::pair<int, int>> cavern;
                std::vector<std::pair<int, int>> queue;
                queue.push_back({x, y});
                visited[y][x] = true;
                
                while (!queue.empty()) {
                    auto pos = queue.back();
                    queue.pop_back();
                    int cx = pos.first;
                    int cy = pos.second;
                    cavern.push_back({cx, cy});
                    
                    // Check 4 neighbors
                    int dx[] = {0, 0, 1, -1};
                    int dy[] = {1, -1, 0, 0};
                    
                    for (int i = 0; i < 4; i++) {
                        int nx = cx + dx[i];
                        int ny = cy + dy[i];
                        
                        if (nx >= 0 && nx < width && ny >= 0 && ny < height &&
                            !visited[ny][nx] && map[ny][nx] == TILE_FLOOR) {
                            visited[ny][nx] = true;
                            queue.push_back({nx, ny});
                        }
                    }
                }
                
                // Fill if too small
                if ((int)cavern.size() < minSize) {
                    for (auto pos : cavern) {
                        int cx = pos.first;
                        int cy = pos.second;
                        map[cy][cx] = TILE_WALL;
                    }
                }
            }
        }
    }
    
    std::cout << "Cavern filling complete" << std::endl;
}

void CaveGenerator::connectAllCaverns() {
    std::cout << "Connecting isolated caverns..." << std::endl;
    
    std::vector<std::vector<int>> caveID(height, std::vector<int>(width, -1));
    std::vector<std::pair<int, int>> caveCenters;
    int nextID = 0;
    
    // Flood fill to identify all cavern regions
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (caveID[y][x] == -1 && map[y][x] == TILE_FLOOR) {
                // Flood fill this cavern
                std::vector<std::pair<int, int>> queue;
                queue.push_back({x, y});
                caveID[y][x] = nextID;
                
                int minX = x, maxX = x, minY = y, maxY = y;
                
                while (!queue.empty()) {
                    auto pos = queue.back();
                    queue.pop_back();
                    int cx = pos.first;
                    int cy = pos.second;
                    
                    minX = std::min(minX, cx);
                    maxX = std::max(maxX, cx);
                    minY = std::min(minY, cy);
                    maxY = std::max(maxY, cy);
                    
                    // Check 4 neighbors
                    int dx[] = {0, 0, 1, -1};
                    int dy[] = {1, -1, 0, 0};
                    
                    for (int i = 0; i < 4; i++) {
                        int nx = cx + dx[i];
                        int ny = cy + dy[i];
                        
                        if (nx >= 0 && nx < width && ny >= 0 && ny < height &&
                            caveID[ny][nx] == -1 && map[ny][nx] == TILE_FLOOR) {
                            caveID[ny][nx] = nextID;
                            queue.push_back({nx, ny});
                        }
                    }
                }
                
                // Store center of this cavern
                caveCenters.push_back({(minX + maxX) / 2, (minY + maxY) / 2});
                nextID++;
            }
        }
    }
    
    std::cout << "  Found " << nextID << " caverns, connecting them..." << std::endl;
    
    // Connect each cavern to the next one using simple tunnels
    for (int i = 0; i < nextID - 1; i++) {
        int x1 = caveCenters[i].first;
        int y1 = caveCenters[i].second;
        int x2 = caveCenters[i + 1].first;
        int y2 = caveCenters[i + 1].second;
        
        // Carve wider tunnels: 20 tiles wide for major connections
        int connectionWidth = 20;
        
        // Carve horizontal tunnel first, then vertical
        // Horizontal
        int startX = std::min(x1, x2);
        int endX = std::max(x1, x2);
        for (int x = startX; x <= endX; x++) {
            for (int wy = -connectionWidth/2; wy <= connectionWidth/2; wy++) {
                int ny = y1 + wy;
                if (ny >= 1 && ny < height - 1 && x >= 1 && x < width - 1) {
                    map[ny][x] = TILE_FLOOR;
                }
            }
        }
        
        // Vertical
        int startY = std::min(y1, y2);
        int endY = std::max(y1, y2);
        for (int y = startY; y <= endY; y++) {
            for (int wx = -connectionWidth/2; wx <= connectionWidth/2; wx++) {
                int nx = x2 + wx;
                if (y >= 1 && y < height - 1 && nx >= 1 && nx < width - 1) {
                    map[y][nx] = TILE_FLOOR;
                }
            }
        }
    }
    
    std::cout << "Cavern connection complete" << std::endl;
}

void CaveGenerator::ensureTopCenterEntrance() {
    std::cout << "Ensuring top center entrance with passage to main cavern..." << std::endl;
    
    int centerX = width / 2;
    int topY = 5;  // A few tiles from top to avoid edge
    
    // Carve out entrance area at top center (5x5 chamber)
    for (int y = topY - 2; y <= topY + 2; y++) {
        for (int x = centerX - 2; x <= centerX + 2; x++) {
            if (x >= 1 && x < width - 1 && y >= 1 && y < height - 1) {
                map[y][x] = TILE_FLOOR;
            }
        }
    }
    
    // Carve a passage downward from entrance until we hit an existing floor area
    for (int y = topY + 3; y < height - 1; y++) {
        for (int x = centerX - 1; x <= centerX + 1; x++) {
            map[y][x] = TILE_FLOOR;
            
            // Stop carving if we hit a large floor area
            if (y > topY + 200) {
                int floorNeighbors = 0;
                for (int dy = -2; dy <= 2; dy++) {
                    for (int dx = -2; dx <= 2; dx++) {
                        int ny = y + dy;
                        int nx = x + dx;
                        if (nx >= 0 && nx < width && ny >= 0 && ny < height && map[ny][nx] == TILE_FLOOR) {
                            floorNeighbors++;
                        }
                    }
                }
                
                if (floorNeighbors > 8) {
                    std::cout << "  Entrance connected to main cavern at depth " << (y - topY) << std::endl;
                    return;
                }
            }
        }
    }
}

std::vector<int> CaveGenerator::getMapFlat() const {
    std::vector<int> flat;
    flat.reserve(width * height);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            flat.push_back(map[y][x]);
        }
    }
    
    return flat;
}
