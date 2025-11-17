# React-Inspired SFML Game Engine

A complete implementation of React's component-based architecture for C++ game development using SFML.

## Quick Start

### Current Setup (Simplified)

**main.cpp** - Minimal SFML test (pure SFML, no React architecture)
- Simple green circle that responds to WASD
- Perfect for verifying SFML builds and runs correctly
- No complex architecture - just basic SFML rendering

### Building

**Windows (Visual Studio):**
```bash
# Open in Visual Studio 2022
# Configure CMake (x64-Debug)
# Press F7 to build
# Press F5 to run with debugging
```

**Command Line:**
```bash
cmake -B build -S .
cmake --build build
./build/bin/STDISCM_P3
```

### Controls

- **WASD / Arrow Keys** - Move the green circle
- **ESC / Close Window** - Exit

## File Structure

```
src/
├── main.cpp                    ← Current entry point (minimal SFML test)
├── main_simple.cpp             ← React-SFML with single component (reference)
├── main_game.cpp               ← Full game demo (reference)
│
├── core/                       ← React-SFML engine core
│   ├── Component.h             ← Base component class
│   ├── RenderNode.h            ← Virtual DOM nodes
│   ├── StateManager.h          ← useState hook
│   ├── EffectManager.h         ← useEffect hook
│   ├── Reconciler.h            ← Diffing & patching
│   ├── Store.h                 ← Redux-like state
│   └── ReactSFMLEngine.h       ← Main game loop
│
└── components/                 ← Example game components
    ├── GameState.h             ← State types & reducers
    ├── PlayerComponent.h       ← Player with hooks
    ├── EnemyComponent.h        ← Enemy entities
    ├── HUDComponent.h          ← UI overlay
    ├── BackgroundComponent.h   ← Background layer
    └── GameComponent.h         ← Root composition
```

## Progression Path

### Step 1: Verify SFML Works (CURRENT)
**File:** `main.cpp` (minimal SFML test)

✅ Simple circle renders
✅ WASD input works
✅ Window displays correctly

**If this works:** SFML is set up correctly, proceed to Step 2

### Step 2: Test React Architecture
**Switch to:** `main_simple.cpp`

Change in CMakeLists.txt or rename files:
```bash
mv src/main.cpp src/main_sfml.cpp
mv src/main_simple.cpp src/main.cpp
```

**What you'll test:**
- Store + Reducer (Redux pattern)
- Component rendering
- Reconciler (virtual DOM)
- Event → Action → State → Render flow

### Step 3: Full Game Demo
**Switch to:** `main_game.cpp`

**Features:**
- Multiple components (Player, Enemy, HUD)
- Complex state management
- Component composition
- Hooks (useState, useEffect)
- Conditional rendering

## React Principles Implemented

| React Feature | Implementation | File |
|--------------|----------------|------|
| Component | `class Component` | `core/Component.h` |
| Virtual DOM | `RenderNode` tree | `core/RenderNode.h` |
| Reconciliation | Diffing & patching | `core/Reconciler.h` |
| useState | `StateRef<T>` | `core/StateManager.h` |
| useEffect | Effect registration | `core/EffectManager.h` |
| Redux Store | `Store<TState>` | `core/Store.h` |
| Actions | `Action` struct | `core/Store.h` |
| Reducer | Pure functions | `components/GameState.h` |

## Architecture Overview

```
Events (WASD) → Actions → Reducer → New State
                                       ↓
                                  Components
                                       ↓
                                 render()
                                       ↓
                                  Virtual DOM
                                       ↓
                                  Reconciler
                                       ↓
                                 SFML Objects
                                       ↓
                                    Screen
```

## Documentation

- **ARCHITECTURE_OVERVIEW.md** - Complete architecture explanation
- **TESTING.md** - Testing guide and debugging
- **ARCHITECTURE_REFACTOR.md** - Design decisions

## Recent Bug Fixes

✅ **StateRef default constructor** - Members can be declared without immediate initialization
✅ **useEffect overload ambiguity** - Clear distinction between overloads
✅ **Reconciler value comparison** - Props now detect actual value changes (critical fix!)
✅ **Type conversions** - All implicit conversions made explicit
✅ **Multiple definition errors** - Static members use inline static

## Build Requirements

- **C++20** or later
- **SFML 2.6.x** (auto-downloaded by CMake)
- **CMake 3.10+**
- **MSVC 2022** (Windows) or **GCC/Clang** (Linux/Mac)

## Example Usage (React-SFML Architecture)

```cpp
// Define state
struct GameState : public State {
    sf::Vector2f playerPos;
    int score;
};

// Define reducer
GameState gameReducer(const GameState& state, const Action& action) {
    GameState newState = state;
    if (action.type == "MOVE_PLAYER") {
        newState.playerPos += action.getPayload<sf::Vector2f>();
    }
    return newState;
}

// Create component
class Player : public Component {
    std::shared_ptr<RenderNode> render() override {
        return Circle({
            {"position", store->getState().playerPos},
            {"radius", 30.0f},
            {"color", sf::Color::Green}
        });
    }
};

// Run engine
ReactSFMLEngine<GameState> engine(800, 600, "Game", GameState(), gameReducer);
engine.setRoot(std::make_shared<Player>());
engine.run();
```

## Why React + SFML?

### Traditional Game Loop Issues
❌ State scattered everywhere
❌ Manual UI synchronization
❌ Side effects hard to track
❌ Difficult to test

### React-SFML Solutions
✅ Single source of truth (Store)
✅ Automatic UI updates (Reconciler)
✅ Predictable data flow
✅ Pure render functions (easy to test)

## Contributing

This is an educational/demonstration project showing how web development patterns (React, Redux) can be applied to game development.

## License

Educational project - use and modify as needed.

## Next Steps

1. ✅ **Build and run main.cpp** - Verify SFML works
2. 🔄 **Switch to main_simple.cpp** - Test React architecture
3. 🎮 **Try main_game.cpp** - Full featured demo
4. 🚀 **Build your own game!** - Use the architecture for your project

---

**Current Status:** Basic SFML test is active. Once verified, switch to React architecture for full features.
