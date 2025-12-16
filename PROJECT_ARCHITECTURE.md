# VC-Manage Project Architecture

## Project Overview
This is a city management simulation game built with C++, SDL2, and ImGui. The game allows you to manage a virtual city by placing buildings, managing resources, employment, and satisfaction metrics.

---

## Core Architecture & Data Flow

### 1. **Main Entry Point** → `main.cpp`
- Creates a WindowSettings object
- Initializes the Application
- Calls `app.run()` which enters the main game loop

---

## 2. **Application Class** (`application.hpp/cpp`)
The central controller that manages the entire game loop.

### Key Components:
- **Window Management**: Creates SDL2 window and renderer
- **ImGui Integration**: Sets up the GUI framework
- **Game Loop** (`run()` method):
  - Initializes a `Simulation` object (the city)
  - Sets up the 64x64 tile grid system
  - Loads buildings from the simulation into the tilemap
  - Handles keyboard input (WASD for camera movement)
  - Handles mouse input (zooming with wheel)
  - Renders the grid with SDL2
  - Manages ImGui UI panels (Toolkit, Inspector, Taskbar)
  - Runs at ~60 FPS (16ms per frame)

### Key Variables:
```cpp
float scale;           // Zoom level (1.0 to 20.0)
float cameraX/Y;       // Camera position in world space
float speed;           // Movement speed
int tilemap[64][64];   // The 2D grid (each cell is a tile type 0-24)
```

### UI Panels:
1. **Toolkit** (left side): Shows building buttons (House, Apartment, Cinema, Bank)
2. **Inspector** (right side): Shows info about hovered tile
3. **Taskbar** (bottom): Shows city stats (population, employment, budget, satisfaction, pollution, cycle)

---

## 3. **Simulation Class** (`cycle/simulation.hpp/cpp`)
Manages the game's state machine and time progression.

### Key Components:
- **Ville** (City): The actual city object being simulated
- **Difficulty Levels**:
  - Easy: 2000 budget, 120s per cycle
  - Medium: 1000 budget, 60s per cycle
  - Hard: 500 budget, 30s per cycle

### Simulation States:
```cpp
enum SimState { Running, Evaluating, GameOver };
```

### Key Methods:
- `tick(float delta)`: Called every frame, advances the simulation timer
- `demarerCycle()`: Start a new cycle
- `terminerCycle()`: End current cycle and evaluate stats
- `terminerCycleEarly()`: Player can skip to next cycle early

### What Happens Each Cycle:
1. Profit collection from commercial buildings
2. Pollution calculation
3. Job assignment to population
4. Satisfaction calculation
5. Population updates
6. Check for game over

---

## 4. **Ville Class** (`ville/ville.hpp/cpp`)
Represents the city itself with all its buildings, resources, and statistics.

### Data:
```cpp
string nom;                    // City name
double budget;                 // Available money
unsigned int population;       // Total population
int satisfaction;              // 0-100 satisfaction %
float polution;                // 0-100 pollution %
Resources resources;           // {eau, electricite}
BatimentList batiments;        // Vector of all buildings
```

### Key Methods:
- `ajoutBatiment(BatPtr batiment)`: Add building to city
- `supprimerBatiment(int id)`: Remove building
- `calculerPolutionTotale()`: Calculate total pollution
- `calculerSatisfactionTotale()`: Calculate total satisfaction
- `calculerCapaciteEmploi()`: Total jobs available
- `calculerEmploiActuel()`: Current employed population
- `calculerTauxChomage()`: Unemployment percentage
- `assignerEmplois()`: Distribute population to jobs

### Satisfaction Factors:
- **Base**: 50% (only if population > 0)
- **Positive**: Parks (+8), Cinema/Mall/Bank (+5 each), low housing ratio (+5)
- **Negative**: Overcrowding (-15), unemployment (up to -20), pollution (quadratic)

---

## 5. **Building Hierarchy**

### Base Class: `Batiment` (`buildings/batiment.hpp/cpp`)
```
TypeBatiment type;          // What kind of building
Surface surface;            // Width & length
Position position;          // X, Y coordinates on grid
Resources consommation;     // Water & electricity consumed
float polution;             // Pollution generated
int id;                     // Unique identifier
string nom;                 // Building name
```

### Building Types (TypeBatiment enum):
```cpp
Blank(0), House(1), Apartment(2), Bank(3), PowerPlant(4),
WaterTreatmentPlant(5), UtilityPlant(6), Park(7),
Cinema(8), Mall(9), Custom(10)
```

### Class Hierarchy:
```
Batiment (base class)
├── Resident (residential buildings)
│   └── Appartement (apartment with floors)
├── Service (service buildings with employees)
│   ├── Commercial (profit-generating)
│   │   ├── Cinema
│   │   ├── Mall
│   │   └── Bank
│   └── Infrastructure (resource producing)
│       ├── PowerPlant
│       ├── WaterTreatmentPlant
│       └── UtilityPlant
└── Park (special recreation building)
```

### Building-Specific Details:

#### **Resident Buildings** (House, Apartment)
- Have `capaciteHabitants`: Max residents
- Have `habitantsActuels`: Current residents
- Resource consumption based on occupancy
- Pollution from residents

#### **Commercial Buildings** (Cinema, Mall, Bank)
- Generate profit
- Need employees
- Have satisfaction bonuses
- Consume resources

#### **Infrastructure** (PowerPlant, etc.)
- Produce resources
- Employ workers
- Generate pollution

---

## 6. **Grid System (TileMap)**

### Grid Layout:
- **64x64 cells** (rows x columns)
- **32x32 pixels per tile** (in texture)
- **Coordinate system**: `[y][x]` in the tilemap array

### Tile Mapping:
```cpp
const int ROWS = 64;
const int COLS = 64;
const int TILE_SIZE = 32;
const int TILES_X = 5;  // Tileset is 5x5
const int TILES_Y = 5;
const int TILE_COUNT = 25; // Total 25 tiles (0-24)
```

### How Buildings Map to Tiles:

When buildings are loaded from the simulation, their size determines how many cells they occupy:

```cpp
// Building dimensions
Cinema:   2x1 (width x height)
Mall:     3x3
Park:     2x2
Others:   1x1 (default)

// Tile index mapping
tilemap[x][y] = tile_number  // 0-24 represents different tile graphics

// For multi-cell buildings, each cell gets assigned a different tile index
// Example (Mall 3x3):
tilemap[x][y] = 12        // Top-left
tilemap[x][y+1] = 13      // Top-middle
tilemap[x][y+2] = 14      // Top-right
tilemap[x+1][y] = 17      // etc...
```

### Camera & Zoom:
- **Scale**: Zoom level (1.0 = normal, higher = zoomed in)
- **CameraX/Y**: Position of top-left corner in world space
- **Mouse hover**: Detects which tile is under cursor
- **Conversion**: `worldX = cameraX + mouseX / scale`

### Rendering Pipeline:
```
For each tile in tilemap:
  1. Calculate destination rectangle on screen
  2. Copy texture source rect to destination
  3. Handle zoom and camera offset

Result: Visible grid rendered to screen
```

---

## 7. **Building Creation Flow**

### Current System (Empty City):
The game starts with an empty city. Buildings are added via code, not player interaction.

### Static Factory Methods:
Each building type has a static `create*` method:

```cpp
// Resident
Resident::createHouse(id, name, ville, x, y);

// Commercial
Comercial::createCinema(id, name, ville, x, y);
Comercial::createMall(id, name, ville, x, y);
Comercial::createBank(id, name, ville, x, y);

// Infrastructure
Infrastructure::createPowerPlant(id, name, ville, x, y);
Infrastructure::createWaterTreatmentPlant(id, name, ville, x, y);
Infrastructure::createUtilityPlant(id, name, ville, x, y);
```

### How to Add a Building at Startup:

In `application.cpp` after `Simulation sim(...)` is created:

```cpp
Simulation sim("Test Town", Difficulty::Medium);
sim.getVille().setBudget(10000);  // Give budget

// Add buildings to city
auto house = std::make_unique<Resident>(
    id, "House Name", &sim.getVille(), 
    TypeBatiment::House, ...parameters...
);
sim.getVille().ajoutBatiment(std::move(house));
```

Or use factory methods:
```cpp
Resident house = Resident::createHouse(1, "My House", &sim.getVille(), 10, 5);
sim.getVille().ajoutBatiment(std::make_unique<Resident>(house));
```

---

## 8. **Data Structures**

### Position Struct
```cpp
struct Position {
    int x, y;  // Grid coordinates
    // Supports + - += -= operators for arithmetic
};
```

### Surface Struct
```cpp
struct Surface {
    float longeur;  // Length
    float largeur;  // Width
};
```

### Resources Struct
```cpp
struct Resources {
    double eau;           // Water
    double electricite;   // Electricity
    // Supports arithmetic operators
};
```

### WindowSettings Struct
```cpp
struct WindowSettings {
    string title;
    int width = 960;
    int height = 640;
};
```

---

## 9. **Main Game Loop Pseudocode**

```
Initialize:
  - Create Window (960x640, SDL2)
  - Initialize ImGui
  - Create Simulation with city
  - Load buildings into tilemap
  - Set initial camera/zoom

Every Frame (16ms = 60 FPS):
  1. Check Events (mouse wheel zoom, keyboard, quit)
  2. Update Camera (WASD movement)
  3. ImGui New Frame
  4. Get mouse hover tile info
  5. Render UI panels (Toolkit, Inspector, Taskbar)
  6. Render Grid to screen
  7. Draw white outline on hovered tile
  8. ImGui Render
  9. Simulation tick (check cycle timer)
  
When Cycle Ends:
  1. Update all building stats
  2. Calculate city-wide metrics
  3. Update population/budget/satisfaction
  4. Start next cycle
```

---

## 10. **Key Files & Their Purpose**

### Headers (`include/`)
- `application.hpp`: Main game controller
- `window.hpp`: SDL2 window wrapper
- `simulation.hpp`: Game state & cycle management
- `ville/ville.hpp`: City and all buildings
- `buildings/batiment.hpp`: Base building class
- `buildings/resident.hpp`: Residential building base
- `buildings/service.hpp`: Service building base
- `buildings/commercial.hpp`: Commercial buildings
- `buildings/infrastructure.hpp`: Utility buildings
- `buildings/appartement.hpp`: Apartment (multi-floor)
- `buildings/parc.hpp`: Parks
- `utils.hpp`: Enums, structs, data types

### Source (`src/`)
- `main.cpp`: Entry point
- `application.cpp`: Game loop & rendering (438 lines)
- `window.cpp`: SDL window initialization
- `simulation.cpp`: Cycle & state management
- `ville.cpp`: City logic & statistics
- `buildings/`: Implementation of all building types

### Utilities
- `tools/imgui/`: ImGui library for UI
- `tools/`: Other third-party tools

---

## 11. **Understanding the Tile Rendering**

### Tileset Image
- Single image file with 5x5 grid of tiles (25 tiles total)
- Each tile is 32x32 pixels
- Tiles 0-24 represent different graphics:
  - Grass variants
  - Building graphics
  - Water
  - Roads
  - etc.

### Rendering Process:
```cpp
// For each cell in 64x64 tilemap
for (int y = 0; y < ROWS; ++y) {
    for (int x = 0; x < COLS; ++x) {
        int tileIndex = tilemap[y][x];  // 0-24
        
        // Source rect in tileset (which tile to copy)
        SDL_Rect src = src[tileIndex];  // Pre-calculated: {x*32, y*32, 32, 32}
        
        // Destination rect on screen (with camera offset & zoom)
        SDL_Rect dest = {
            int((x * 32 - cameraX) * scale),
            int((y * 32 - cameraY) * scale),
            int(32 * scale),
            int(32 * scale)
        };
        
        // Copy from tileset to screen
        SDL_RenderCopy(renderer, tileTexture, &src, &dest);
    }
}
```

---

## 12. **Resource System**

### Resource Types:
- **Eau** (Water)
- **Electricite** (Electricity)

### Consumption:
Each building consumes resources based on type and occupancy:
- Residential: Per capita consumption
- Commercial: Fixed + employee dependent
- Infrastructure: Generate resources

### Impact:
- Budget decreases when resources are consumed
- City-wide calculations track total consumption vs production

---

## 13. **Testing the System**

### Current Test Scenario:
The game starts with an empty city. To verify everything works:

### Test Steps:
1. **Start game**: City appears with empty grid (grass)
2. **Add buildings via code** (see section below)
3. **View in interface**: Buildings should appear as tiles
4. **Check stats**: Taskbar shows updated metrics
5. **Progress cycles**: Click "Skip Month" to advance
6. **Observe changes**: Population, budget, satisfaction update

---

## How to Add Test Buildings

### Quick Test: Add Multiple Building Types

Edit `application.cpp` in the `run()` method, after `sim.getVille().setBudget(10000);`:

```cpp
// Give the city starting budget
sim.getVille().setBudget(50000);
sim.getVille().setPopulation(100);

// Add different building types to test
// Note: You need to include the building headers

auto house = std::make_unique<Resident>(
    1, "Test House", &sim.getVille(),
    TypeBatiment::House, 5, 100.0, 
    Resources(10.0, 10.0), 1.0f,
    Position(5, 5), Surface(1, 1), 8, 5
);
sim.getVille().ajoutBatiment(std::move(house));

auto cinema = Comercial::createCinema(2, "Test Cinema", &sim.getVille(), 15, 15);
sim.getVille().ajoutBatiment(std::make_unique<Comercial>(cinema));

auto park = Park::createPark(3, "Test Park", &sim.getVille(), 25, 25);
sim.getVille().ajoutBatiment(std::move(park));
```

### Expected Results:
- Buildings appear on the grid
- Tiles show correct graphics
- Taskbar updates with new statistics
- Cycles run properly

---

## Summary

**Grid System**: 64x64 tilemap with 32x32 pixel tiles, supports zoom/pan camera
**Buildings**: Hierarchical class structure, placed on grid, generate stats
**Simulation**: Time-based cycles, automatic stat calculation each cycle
**UI**: ImGui for menus, SDL2 for grid rendering
**Data Flow**: Application → Simulation → Ville → Buildings → Tilemap → Render
