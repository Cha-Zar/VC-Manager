# Quick Reference: Grid & Building System

## Visual Grid Layout

```
       0        32       64       96  ... COLS(64)
       |        |        |        |
    0  +--------+--------+--------+ ---
       |        |        |        |
   32  +--------+--------+--------+  |
       |        |        |        |  | TILE_SIZE = 32 pixels
   64  +--------+--------+--------+  |
       |        |        |        |
   96  +--------+--------+--------+ ---
       |
      ...
    
    ROWS(64)

Each cell: 32x32 pixels
Total grid: 64x64 cells = 2048x2048 world pixels
```

## Coordinate System

```cpp
Position(x, y)  // Grid coordinates (0-63 each)
            ^
            | y increases downward
            |
    0 ------+------ 63 (x)
    |       |
    |       |
    63      +

// Screen coordinates (with camera offset)
screen_x = (world_x - cameraX) * scale
screen_y = (world_y - cameraY) * scale
```

---

## Building Class Hierarchy

```
Batiment (abstract base)
    ├── Resident
    │   ├── Appartement
    │   └── (created via Resident::createHouse())
    │
    ├── Service
    │   ├── Comercial
    │   │   ├── Cinema (2x1)
    │   │   ├── Mall (3x3)
    │   │   └── Bank (1x1)
    │   │
    │   ├── Infrastructure
    │   │   ├── PowerPlant (2x2)
    │   │   ├── WaterTreatmentPlant
    │   │   └── UtilityPlant
    │   │
    │   └── Parc (inherits from Service)
    │       └── Park (2x2)
    │
    └── [Custom buildings]
```

---

## Building Type → Tile Size Mapping

| Building | Type ID | Size | Tile Indices | Position Example |
|----------|---------|------|--------------|-----------------|
| House | 1 | 1x1 | 0 | (10, 10) |
| Apartment | 2 | 1x1 | 1 | (15, 10) |
| Bank | 3 | 1x1 | 2 | (20, 10) |
| PowerPlant | 4 | 2x2 | 3-8 | (5, 5) |
| WaterPlant | 5 | 2x2 | 3-8 | (15, 5) |
| UtilityPlant | 6 | 2x2 | 3-8 | (25, 5) |
| Park | 7 | 2x2 | 15-18 | (25, 25) |
| Cinema | 8 | 2x1 | 10-11 | (35, 35) |
| Mall | 9 | 3x3 | 12-24 | (10, 30) |

---

## Data Structure Quick Reference

### Batiment (Base)
```cpp
struct Batiment {
    TypeBatiment type;          // What kind
    Surface surface;            // Size: {longeur, largeur}
    Position position;          // Location: {x, y}
    Ville* ville;              // City pointer
    int id;                     // Unique ID
    string nom;                // Name
    Resources consommation;    // {eau, electricite} consumed
    float polution;            // Pollution output
    double cost;               // Purchase price
    int effectSatisfication;   // Satisfaction impact
};
```

### Position
```cpp
struct Position {
    int x, y;
    // Can use: + - += -= operators
    Position(10, 20)
    Position(5, 5) + Position(3, 2) == Position(8, 7)
};
```

### Surface
```cpp
struct Surface {
    float longeur;   // Length
    float largeur;   // Width
    Surface(1, 1)    // 1x1 building
    Surface(2, 2)    // 2x2 building
    Surface(3, 3)    // 3x3 building
};
```

### Resources
```cpp
struct Resources {
    double eau;             // Water
    double electricite;     // Electricity
    Resources(10.0, 15.0)  // 10 water, 15 electricity
    // Can use: + - += -= operators
};
```

---

## Game Loop Timing

```
Frame Time: 16ms = 60 FPS

Each Frame:
  1. Check input (2ms)
  2. Update simulation (2ms)
  3. Render (12ms)

Cycle Timing (by difficulty):
  Easy:   120 seconds
  Medium:  60 seconds
  Hard:    30 seconds

Each cycle: ~7500 frames (at 60 FPS for medium)
```

---

## Building Parameter Quick Guide

### Creating a Building

```cpp
auto building = std::make_unique<BuildingClass>(
    1,                          // int id (unique identifier)
    "Building Name",            // string nom
    &sim.getVille(),           // Ville* ville
    TypeBatiment::House,       // TypeBatiment type
    5,                         // int effectSatisfication (0-100)
    100.0,                     // double cost (budget required)
    Resources(10.0, 15.0),    // Resources consommation
    1.0f,                      // float polution (0-100)
    Position(10, 10),          // Position position
    Surface(1, 1),             // Surface surface
    
    // Additional params depend on class:
    // For Resident:
    8,                         // int capaciteHabitants
    5,                         // int habitantsActuels
    
    // For Service:
    5,                         // unsigned int employees
    5,                         // unsigned int employeesNeeded
    
    // For Comercial:
    100.0                      // double profit
    
    // For Infrastructure:
    // Resources productionRessources
);
```

---

## Important Methods

### Adding Buildings
```cpp
Ville& ville = sim.getVille();
auto building = std::make_unique<Resident>(...);
ville.ajoutBatiment(std::move(building));  // Building added to city
```

### City Statistics
```cpp
int pop = ville.getPopulation();
double budget = ville.getBudget();
int satisfaction = ville.getSatisfaction();
float pollution = ville.getPolution();
Resources res = ville.getResources();

// Calculations
ville.calculerPolutionTotale();         // Updates pollution
ville.calculerSatisfactionTotale();     // Updates satisfaction
int employed = ville.calculerEmploiActuel();
int capacity = ville.calculerCapaciteEmploi();
float unemployment = ville.calculerTauxChomage();

// Population management
ville.assignerEmplois();                // Distribute jobs
ville.updatePopulation();               // Update growth
```

### Simulation Control
```cpp
int cycle = sim.getCycle();
float timeRemaining = sim.getTimePerCycle() - sim.getCurrentTime();
SimState state = sim.getState();

sim.tick(0.016f);              // Called every frame
sim.terminerCycleEarly();      // Skip to next cycle
```

---

## Satisfaction Calculation Formula

```
Base Satisfaction: 50% (if population > 0)

Positive factors:
  + Parks:                    +8 per park
  + Commerce (Cinema/Mall):   +5 per building
  + Low housing ratio (<50%): +5
  
Negative factors:
  - Unemployment:  (unemployment% × 0.2)
  - Pollution:     (pollution² × 0.5)
  - Overcrowding:  -15 if >90% capacity

Final: Clamped to 0-100%
```

---

## Pollution Calculation Formula

```
Total = 0

For each building:
  PowerPlant:        +15 (major polluter)
  Commercial:        +5 (mall, cinema, bank)
  Residential:       +2 base + (occupancy × 3)
  Population:        + (population / 100 × 0.5)

Environment:
  Natural decrease:  × 0.95

Final: Clamped to 0-100%
```

---

## Employment System

```
Employed = Current job assignments
Capacity = Total available jobs across all services
Unemployment = 100% × (population - employed) / population

Job Distribution:
  1. Calculate total job slots needed
  2. Distribute population to jobs
  3. Excess population remains unemployed
  4. Affects satisfaction
```

---

## Testing Checklist

When adding buildings, verify:

```
[ ] Building ID unique (no duplicates)
[ ] Position valid (0-63 for x and y)
[ ] Budget sufficient (cost ≤ current budget)
[ ] Building appears on grid
[ ] Correct tile graphic shows
[ ] Taskbar stats update properly
[ ] Can hover to inspect (Inspector panel)
[ ] Cycles advance with "Skip Month"
[ ] Stats change appropriately
[ ] No crashes or errors
```

---

## Tile Index Reference

### Tileset (5x5 grid of 32x32 tiles)

```
Tile 0  | Tile 1  | Tile 2  | Tile 3  | Tile 4
--------|---------|---------|---------|--------
Tile 5  | Tile 6  | Tile 7  | Tile 8  | Tile 9
--------|---------|---------|---------|--------
Tile 10 | Tile 11 | Tile 12 | Tile 13 | Tile 14
--------|---------|---------|---------|--------
Tile 15 | Tile 16 | Tile 17 | Tile 18 | Tile 19
--------|---------|---------|---------|--------
Tile 20 | Tile 21 | Tile 22 | Tile 23 | Tile 24
```

**Mapping:**
- 0-6: Terrain variations (grass, water)
- 7-11: Single buildings (house, apt, bank, power, etc)
- 12-24: Multi-cell building parts (mall, park combinations)

---

## Common Building Configurations

### Small Village (test basic functionality)
```
- 5 Houses at y=10, x=5,10,15,20,25
- 1 Park at (25,25)
- 1 Cinema at (35,35)
Budget: 50,000
Population: 100
```

### Medium City (test complex interactions)
```
- 10 Apartments across x=0-50, y=0-20
- 2 Parks at (15,40) and (45,40)
- 1 Mall at (25,25)
- 1 PowerPlant at (50,50)
- 1 WaterPlant at (40,60)
Budget: 100,000
Population: 500
```

### Large City (stress test)
```
- 20+ Apartments/Houses scattered
- 5+ Parks
- 3+ Commercial buildings
- 3+ Infrastructure buildings
Budget: 200,000+
Population: 1000+
```

---

## includes Required (at top of application.cpp)

```cpp
#include "../include/buildings/resident.hpp"
#include "../include/buildings/comercial.hpp"
#include "../include/buildings/infrastructure.hpp"
#include "../include/buildings/parc.hpp"
#include "../include/buildings/appartement.hpp"
#include "../include/buildings/service.hpp"
#include "../include/buildings/batiment.hpp"
```

---

## Common Errors & Fixes

| Error | Cause | Fix |
|-------|-------|-----|
| Building doesn't appear | Position out of bounds | Check x,y < 64 |
| Crash on startup | Null ville pointer | Use `&sim.getVille()` |
| No stats update | Wrong cycle state | Click "Skip Month" |
| Memory error | Forgot std::move() | Use `std::move(building)` |
| Budget goes negative | Cost too high | Increase setBudget() |
| Compilation error | Missing include | Add building class headers |
| Grid all black | Camera out of bounds | Reset cameraX/Y to 0 |

---

## Performance Notes

```
Grid rendering: O(64×64) per frame = acceptable
Building updates: O(num_buildings) per cycle = efficient
Satisfaction calc: O(num_buildings) per cycle = fast
Job assignment: O(population) per cycle = can be slow if high

Optimization possible if:
- Need >1000 buildings
- Population >10,000
- Running on low-end hardware
```

---

## Default Static Values

```cpp
// Resident factors
Resident::WATER_PER_PERSON = 1.0
Resident::ELECTRICITY_PER_PERSON = 1.0
Resident::SATISFACTION_PER_PERSON = 0.5
Resident::POLLUTION_PER_PERSON = 0.1
Resident::BASE_CAPACITY_HOUSE = 8
Resident::BASE_CAPACITY_APARTMENT = 16

// Commercial factors
Comercial::PROFIT_PER_EMPLOYEE = 50.0
Comercial::SATISFACTION_BONUS = 5.0
Comercial::POLLUTION_PENALTY = 3.0
Comercial::EMPLOYEE_EFFICIENCY = 1.0
```

These can be modified with setter methods if needed!

---

## Quick Start Command Reference

```bash
# Compile
make

# Run
./[binary_name]  # Check Makefile for actual binary name

# Clean & rebuild
make clean && make

# Check for errors
make 2>&1 | grep error
```

---

## Next Development Steps

After understanding and testing:

1. **Player interaction**: Detect mouse clicks on grid
2. **Building placement**: Allow placing buildings in empty tiles
3. **Building deletion**: Right-click to remove
4. **Budget validation**: Prevent placing buildings if too expensive
5. **Persistence**: Save/load city state
6. **Pathfinding**: For residents/workers to move
7. **Events**: Random events affecting the city
8. **Achievements**: Milestones and goals
