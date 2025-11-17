# Testing the React-SFML Architecture

## Bug Fix Applied

**CRITICAL BUG FIXED:** The reconciler wasn't detecting prop value changes, only type changes. This meant when position changed from `(400, 300)` to `(410, 300)`, it wouldn't update the screen because both are `sf::Vector2f`.

**Fix:** Added proper value comparison in `Reconciler.h` - the `propValueEquals()` function now deeply compares all prop types.

## Test Programs

Three test programs are provided to verify the architecture works:

### 1. main_minimal.cpp - Pure SFML Test
**Purpose:** Verify SFML itself works (no React architecture)

```bash
# Build with CMake - set as startup project
# Or compile directly:
# g++ src/main_minimal.cpp -o minimal -lsfml-graphics -lsfml-window -lsfml-system
```

**What it does:**
- Green circle at center of screen
- WASD or Arrow keys move the circle
- Direct SFML rendering, no reconciliation
- Prints position to console on each move

**If this works:** SFML is working correctly
**If this fails:** SFML/graphics setup issue

### 2. main_simple.cpp - Minimal React-SFML Test
**Purpose:** Test core React-SFML architecture with minimal complexity

```bash
# Set as startup project in CMakeLists.txt
```

**What it does:**
- Uses full React-SFML engine
- Single state: just position
- Single component: SimpleCircle
- WASD moves circle via Redux actions
- Logger middleware shows dispatched actions

**Components used:**
- ✅ Store + Reducer (global state)
- ✅ Component with render()
- ✅ Reconciler (virtual DOM)
- ✅ Event handler (WASD → Actions)

**If this works:** Core architecture is functional
**If this fails:** Issue in React-SFML core systems

### 3. main.cpp - Full Game Demo
**Purpose:** Complete example with multiple components

**What it does:**
- Player component (green circle with health)
- Enemy spawning (red circles)
- HUD component (score, game over)
- Complex state management

**Components:**
- GameComponent (root)
- PlayerComponent (useState, useEffect)
- EnemyComponent (list rendering)
- HUDComponent (conditional rendering)
- BackgroundComponent

**Controls:**
- WASD / Arrows - Move player
- Space - Spawn enemy
- H - Take damage (test)

## Debugging Checklist

### Issue: Nothing appears on screen
1. Check console output - are actions being dispatched?
2. Try `main_minimal.cpp` - does pure SFML work?
3. Check window is created (800x600 title appears?)

### Issue: Actions dispatch but nothing moves
**This was the bug!** Should be fixed now with the reconciler update.

Verify the fix:
1. Build latest code with reconciler changes
2. Run `main_simple.cpp`
3. Press WASD - circle should move
4. Console should show: `New position: (X, Y)`

### Issue: Build errors
See previous fixes for:
- C2512: StateRef default constructor
- C2668: useEffect overload ambiguity
- C4244: int to float conversion

## CMakeLists.txt Configuration

To test different programs, modify the executable:

```cmake
# Test 1: Minimal SFML
add_executable(STDISCM_P3 "src/main_minimal.cpp")
target_link_libraries(STDISCM_P3 PRIVATE sfml-graphics)

# Test 2: Simple React-SFML
add_executable(STDISCM_P3 "${PROJ_SRC}")
# (Exclude main_minimal.cpp from PROJ_SRC)

# Test 3: Full demo (default)
add_executable(STDISCM_P3 "${PROJ_SRC}")
# main.cpp is the entry point
```

Or create multiple executables:

```cmake
add_executable(minimal "src/main_minimal.cpp")
target_link_libraries(minimal PRIVATE sfml-graphics)

add_executable(simple "src/main_simple.cpp" <core files>)
target_link_libraries(simple PRIVATE sfml-graphics)

add_executable(demo "${PROJ_SRC}")
target_link_libraries(demo PRIVATE sfml-graphics)
```

## Expected Behavior

### main_minimal.cpp
```
=== Minimal SFML Test ===
Use WASD to move the circle
Left - new X: 390
Left - new X: 380
Up - new Y: 290
```

### main_simple.cpp
```
=== Simple React-SFML Demo ===
Use WASD or Arrow Keys to move the circle

Starting engine...
[ACTION] MOVE
New position: (410, 300)
[ACTION] MOVE
New position: (420, 300)
```

### main.cpp
```
=== React-Inspired SFML Game Engine ===
Controls:
  WASD / Arrow Keys - Move player
  Space - Spawn enemy
  H - Take damage (test)

[ACTION] MOVE_PLAYER
Player health: 100
[ACTION] SPAWN_ENEMY
[ACTION] DAMAGE_PLAYER
Player health: 90
```

## Architecture Validation

| Test | What It Validates |
|------|------------------|
| main_minimal.cpp | SFML rendering pipeline works |
| main_simple.cpp | React-SFML core (Store, Reconciler, Component) |
| main.cpp | Full feature set (hooks, composition, effects) |

## Next Steps

Once `main_simple.cpp` works:
1. ✅ Core architecture is proven
2. Add more components incrementally
3. Test hooks (useState, useEffect)
4. Test component composition
5. Build your actual game!

## Common Issues

**"Circle appears but doesn't move"**
- ✅ FIXED: Reconciler now compares prop values correctly
- The propValueEquals() function deep-compares Vector2f

**"Actions logged but no visual update"**
- ✅ FIXED: Was caused by reconciler not detecting changes
- Verify you have latest Reconciler.h with propValueEquals()

**"Multiple definition error"**
- ✅ FIXED: Use `inline static sf::Font` in SFMLInstance

**"StateRef has no default constructor"**
- ✅ FIXED: StateRef now has default constructor

## Performance Notes

The current reconciler recreates SFML objects on every prop change (simplified approach). For better performance:

1. **Batch updates** - Group state changes
2. **Memoization** - Only render when state changes
3. **Object pooling** - Reuse SFML objects
4. **Smart diffing** - Update only changed properties

For a simple demo game, current performance is fine!
