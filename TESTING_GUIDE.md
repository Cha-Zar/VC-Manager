# Testing Guide: Adding New Building Types

This guide shows you how to add new building types to test if they appear in the interface and work correctly with the simulation.

---

## Prerequisites

1. **Understand the building hierarchy** (see PROJECT_ARCHITECTURE.md)
2. **Understand the grid system**: 64x64 tilemap, buildings occupy cells
3. **Know how to add buildings**: Use the `ajoutBatiment()` method on the city
4. **Compile command**: `make` (builds the entire project with Makefile)
5. **Run command**: After building, execute the compiled binary

---

## Quick Test: Add Buildings at Startup

The fastest way to test is to add buildings directly in `application.cpp` when the game starts.

### Step 1: Locate the Startup Code

Open [src/application.cpp](src/application.cpp) and find the `run()` method around line 159:

```cpp
int Application::run() {
  // init Simulation
  Simulation sim("Test Town", Difficulty::Medium);
  sim.getVille().setBudget(1000);
  sim.getVille().calculerPolutionTotale();
  sim.getVille().calculerSatisfactionTotale();

  const int ROWS = 64;
  // ... grid setup code ...
```

### Step 2: Add Buildings Right After Simulation Initialization

After `sim.getVille().setSatisfaction(...)`, add your test buildings:

```cpp
  // ========== TEST: Add Buildings Here ==========
  sim.getVille().setBudget(100000);  // Give lots of budget for testing
  sim.getVille().setPopulation(500);  // Give initial population

  // Example 1: Add a simple House
  try {
    auto house = std::make_unique<Resident>(
        1,                                          // ID
        "Test House 1",                            // Name
        &sim.getVille(),                           // Pointer to city
        TypeBatiment::House,                       // Building type
        5,                                         // Satisfaction effect
        100.0,                                     // Cost
        Resources(10.0, 15.0),                     // Consumption (water, electricity)
        1.0f,                                      // Pollution
        Position(10, 10),                          // Position on grid (x, y)
        Surface(1, 1),                             // Size (width, height)
        8,                                         // Max capacity
        5                                          // Current occupants
    );
    sim.getVille().ajoutBatiment(std::move(house));
  } catch (const std::exception& e) {
    std::cerr << "Error adding house: " << e.what() << "\n";
  }

  // Example 2: Add an Apartment
  try {
    auto apt = std::make_unique<Resident>(
        2, "Test Apartment 1", &sim.getVille(),
        TypeBatiment::Apartment, 3, 200.0,
        Resources(15.0, 20.0), 2.0f,
        Position(15, 10), Surface(1, 1),
        16, 10
    );
    sim.getVille().ajoutBatiment(std::move(apt));
  } catch (const std::exception& e) {
    std::cerr << "Error adding apartment: " << e.what() << "\n";
  }

  // Example 3: Add a Park
  try {
    auto park = std::make_unique<Parc>(
        3, "Test Park", &sim.getVille(),
        TypeBatiment::Park, 10, 300.0,
        2, 2,                                      // Employees, EmployeesNeeded
        Resources(5.0, 5.0), 0.5f,
        Position(25, 25), Surface(2, 2)
    );
    sim.getVille().ajoutBatiment(std::move(park));
  } catch (const std::exception& e) {
    std::cerr << "Error adding park: " << e.what() << "\n";
  }

  // Example 4: Add a Cinema
  try {
    auto cinema = std::make_unique<Comercial>(
        4, "Test Cinema", &sim.getVille(),
        TypeBatiment::Cinema, 8, 500.0,
        5, 5,                                      // Employees, EmployeesNeeded
        Resources(20.0, 30.0), 3.0f,
        Position(35, 35), Surface(2, 1),
        100.0                                      // Profit
    );
    sim.getVille().ajoutBatiment(std::move(cinema));
  } catch (const std::exception& e) {
    std::cerr << "Error adding cinema: " << e.what() << "\n";
  }

  // Example 5: Add a Mall (larger building)
  try {
    auto mall = std::make_unique<Comercial>(
        5, "Test Mall", &sim.getVille(),
        TypeBatiment::Mall, 12, 1000.0,
        10, 10,
        Resources(40.0, 50.0), 5.0f,
        Position(10, 30), Surface(3, 3),
        250.0
    );
    sim.getVille().ajoutBatiment(std::move(mall));
  } catch (const std::exception& e) {
    std::cerr << "Error adding mall: " << e.what() << "\n";
  }

  // Example 6: Add Infrastructure (PowerPlant)
  try {
    auto powerplant = std::make_unique<Infrastructure>(
        6, "Test PowerPlant", &sim.getVille(),
        TypeBatiment::PowerPlant, 3, 2000.0,
        15, 15,
        Resources(50.0, 10.0),                     // Consumption
        15.0f,                                     // High pollution
        Position(50, 50), Surface(2, 2),
        Resources(0.0, 100.0)                      // Production (produces electricity)
    );
    sim.getVille().ajoutBatiment(std::move(powerplant));
  } catch (const std::exception& e) {
    std::cerr << "Error adding powerplant: " << e.what() << "\n";
  }
  
  // ========== END TEST BUILDINGS ==========
```

### Step 3: Include Required Headers

Make sure these headers are already included at the top of [src/application.cpp](src/application.cpp):

```cpp
#include "../include/buildings/resident.hpp"
#include "../include/buildings/commercial.hpp"
#include "../include/buildings/infrastructure.hpp"
#include "../include/buildings/parc.hpp"
```

Check if they're already there. If not, add them.

---

## What Should Happen When You Run

1. **Game starts** with test buildings already placed
2. **View the grid** - buildings should appear as colored tiles in their positions
3. **Hover over tiles** - Inspector panel (right side) shows tile info
4. **Check taskbar** - Shows updated population, employment, satisfaction, budget
5. **Click "Skip Month"** - Advances the cycle, updates stats

### Expected Results:

| Building | Appearance | Effects |
|----------|-----------|---------|
| House | Small tile at (10,10) | +Population, +Consumption |
| Apartment | Small tile at (15,10) | +Population, +Consumption |
| Park | 2x2 grid at (25,25) | +Satisfaction, -Pollution |
| Cinema | 2x1 grid at (35,35) | +Satisfaction, +Jobs |
| Mall | 3x3 grid at (10,30) | +Satisfaction, +Jobs, +Profit |
| PowerPlant | 2x2 grid at (50,50) | +Pollution, +Employees, +Electricity |

---

## Building Type Reference

### Resident Buildings (Housing)

Inherits from: `Resident` class
Parameters: `(id, name, city_ptr, type, satisfaction, cost, resources, pollution, position, surface, capacity, current_occupants)`

```cpp
// House
Position(5, 5), Surface(1, 1), capacity=8, occupants=5

// Apartment (can have multiple floors)
Position(15, 10), Surface(1, 1), capacity=16, occupants=10
```

**Types**:
- `TypeBatiment::House`
- `TypeBatiment::Apartment`

---

### Service Buildings (Employment)

Base class: `Service` class (has employees)
Parameters: `(id, name, city_ptr, type, satisfaction, cost, employees, employees_needed, resources, pollution, position, surface)`

**Sub-types**:

#### Commercial (Profit-generating)
Class: `Comercial`
```cpp
Position(35, 35), Surface(2, 1),
employees=5, employees_needed=5,
Resources(20, 30), pollution=3.0f,
profit=100.0
```

**Types**:
- `TypeBatiment::Cinema` (2x1)
- `TypeBatiment::Mall` (3x3)
- `TypeBatiment::Bank` (1x1)

#### Infrastructure (Resource-producing)
Class: `Infrastructure`
Parameters: `(..., production_resources)`

```cpp
Position(50, 50), Surface(2, 2),
Resources(50, 10),      // Consumption
Resources(0, 100.0)     // Production output
```

**Types**:
- `TypeBatiment::PowerPlant` - Produces electricity
- `TypeBatiment::WaterTreatmentPlant` - Produces water
- `TypeBatiment::UtilityPlant` - Mixed utility

---

### Recreation Buildings

#### Park
Class: `Parc`
Parameters: `(id, name, city_ptr, type, satisfaction, cost, employees, employees_needed, resources, pollution, position, surface)`

```cpp
Position(25, 25), Surface(2, 2),
employees=2, employees_needed=2
```

**Type**: `TypeBatiment::Park` (2x2)

---

## Testing Checklist

After running `make` and executing the game:

- [ ] Game window opens (960x640)
- [ ] Grid appears with buildings visible
- [ ] Taskbar shows updated stats (population, jobs, satisfaction, budget)
- [ ] Can zoom with mouse wheel
- [ ] Can pan with WASD keys
- [ ] Can hover over tiles to see info in Inspector
- [ ] Can click "Skip Month" button
- [ ] Stats update when cycle ends
- [ ] No crashes when buildings interact

---

## Debugging Tips

### If buildings don't appear:

1. **Check positions**: Ensure `x < 64` and `y < 64`
2. **Check building includes**: Make sure headers are included
3. **Check console output**: Look for error messages
4. **Check costs**: Ensure budget is sufficient (`setBudget()`)
5. **Check compilation**: Run `make clean && make` to rebuild

### If game crashes:

1. **Check exceptions**: Wrap in try-catch blocks (see examples above)
2. **Check null pointers**: Ensure `&sim.getVille()` is valid
3. **Check memory**: Don't use raw pointers, use `std::make_unique<>`
4. **Check grid bounds**: x and y must be 0-63

### If stats don't update:

1. **Click "Skip Month"**: Cycles don't auto-advance in test mode
2. **Check jobAssignment**: Population must match available jobs
3. **Check satisfaction**: May be capped at 0-100

---

## Advanced Testing: Adding Multiple Variants

To thoroughly test, try adding multiple buildings of the same type:

```cpp
// Add 5 houses in a row
for (int i = 0; i < 5; ++i) {
  auto house = std::make_unique<Resident>(
      100 + i,                                    // Unique ID
      "House " + std::to_string(i + 1),         // Name
      &sim.getVille(),
      TypeBatiment::House,
      5, 100.0,
      Resources(10.0, 15.0), 1.0f,
      Position(5 + i * 2, 5),                   // Position changes
      Surface(1, 1), 8, 5
  );
  sim.getVille().ajoutBatiment(std::move(house));
}

// Add a cluster of parks
for (int x = 20; x < 50; x += 5) {
  auto park = std::make_unique<Parc>(
      200 + x,
      "Park " + std::to_string(x),
      &sim.getVille(),
      TypeBatiment::Park, 10, 300.0,
      2, 2,
      Resources(5.0, 5.0), 0.5f,
      Position(x, 20), Surface(2, 2)
  );
  sim.getVille().ajoutBatiment(std::move(park));
}
```

---

## Understanding Tile Indices

When buildings are added, they map to specific tile indices (0-24) based on type:

```
Tile Index → Building Type
0-6        → Terrain (grass, water)
1          → House
2          → Apartment
3          → Bank
4          → PowerPlant
5          → WaterTreatmentPlant
6          → UtilityPlant
7          → Park
8          → Cinema
9          → Mall
10-24      → Multi-cell building parts
```

The game automatically selects the correct tile graphic for each building type.

---

## Files You'll Modify

Only ONE file needs modification:
- **[src/application.cpp](src/application.cpp)** - Add your test code in the `run()` method

**DO NOT MODIFY**:
- ✗ Makefile (you said so!)
- ✗ Header files (unless adding new building types)
- ✗ Building implementation files (unless fixing bugs)

---

## Creating a Custom Building Type

To add a completely NEW building type (beyond testing existing ones):

### Step 1: Add TypeBatiment enum value
[include/utils.hpp](include/utils.hpp) - add to `enum class TypeBatiment`:
```cpp
enum class TypeBatiment {
  // ... existing ...
  MyCustomBuilding  // New type
};
```

### Step 2: Create header file
`include/buildings/mycustom.hpp`:
```cpp
#ifndef MYCUSTOM
#define MYCUSTOM

#include "service.hpp"  // or other base class

class MyCustom : public Service {
public:
  // Your implementation
};

#endif
```

### Step 3: Create implementation file
`src/buildings/mycustom.cpp`:
```cpp
#include "../../include/buildings/mycustom.hpp"

// Your implementation code
```

### Step 4: Add to application.cpp
```cpp
#include "../include/buildings/mycustom.hpp"

// In run() method:
auto custom = std::make_unique<MyCustom>(
    7, "My Custom", &sim.getVille(),
    TypeBatiment::MyCustomBuilding,
    // ... other parameters
);
sim.getVille().ajoutBatiment(std::move(custom));
```

---

## Next Steps After Testing

Once you've successfully added test buildings and seen them appear:

1. **Verify rendering**: Check that tiles appear correctly
2. **Verify stats**: Check that population/employment/satisfaction update
3. **Verify cycles**: Click "Skip Month" and watch stats change
4. **Modify parameters**: Try different positions, sizes, capacities
5. **Create variants**: Build a city with many different types
6. **Test edge cases**: Overcrowd buildings, run out of money, max out jobs

Then you can implement player interaction to place buildings dynamically!

---

## Summary

**To test buildings**:
1. Edit [src/application.cpp](src/application.cpp)
2. Add buildings using `std::make_unique<BuildingClass>(...)`
3. Add to city with `sim.getVille().ajoutBatiment(std::move(building))`
4. Run `make` to compile
5. Run the executable to see results

**What to verify**:
- Buildings appear on grid ✓
- Correct tile graphics show ✓
- Taskbar stats update ✓
- Cycles work properly ✓
- No crashes ✓
