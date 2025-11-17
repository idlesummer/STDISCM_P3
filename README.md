# Reactive SFML - Minimal Game Engine

A minimal React-inspired game engine for C++ using SFML. Complete architecture in ~130 lines of code.

## Quick Start

```bash
# Build
cmake -B build -S .
cmake --build build

# Run
./build/bin/STDISCM_P3
```

**Controls:** WASD or Arrow Keys to move the green circle

## What This Demonstrates

This minimal example shows the complete React architecture working:

### 1. **State Management (Redux Pattern)**
```cpp
struct GameState : public State {
    sf::Vector2f position;  // Single source of truth
};
```

### 2. **Actions**
```cpp
store.dispatch(createMoveAction(delta));
// [ACTION] MOVE (logged to console)
```

### 3. **Reducer (Pure Function)**
```cpp
GameState gameReducer(const GameState& state, const Action& action) {
    auto newState = GameState(state);
    if (action.type == "MOVE")
        newState.position = state.position + delta;
    return newState;  // Immutable!
}
```

### 4. **Component with Render**
```cpp
class CircleComponent : public Component {
    std::shared_ptr<RenderNode> render() override {
        auto state = this->store->getState();

        auto props = Props();
        props["position"] = state.position;
        props["radius"] = 30.0f;

        return Circle(props);  // Virtual DOM!
    }
};
```

### 5. **Reconciliation (Auto-updates)**
```cpp
// When state changes:
// 1. Component re-renders → new Virtual DOM
// 2. Reconciler diffs old vs new
// 3. Updates only changed props
// 4. Circle moves on screen!
```

## Complete Data Flow

```
Press 'W'
    ↓
Event Handler → Action { type: "MOVE", delta: (0, -10) }
    ↓
Store.dispatch(action)
    ↓
Reducer → New State { position: (400, 290) }
    ↓
Component.render() → Virtual DOM { Circle at (400, 290) }
    ↓
Reconciler.diff() → "Position changed!"
    ↓
SFML circle.setPosition(400, 290)
    ↓
Screen updates ✓
```

## File Structure

```
src/
├── main.cpp                    ← Everything in one file! (~130 lines)
│   ├── GameState               ← State definition
│   ├── gameReducer             ← Pure state update function
│   ├── CircleComponent         ← React-like component
│   └── main()                  ← Setup and run
│
└── core/                       ← React architecture (reusable)
    ├── Engine.h                ← Main game loop
    ├── Component.h             ← Base component class
    ├── RenderNode.h            ← Virtual DOM nodes
    ├── Reconciler.h            ← Diffing & patching
    ├── Store.h                 ← Redux store
    ├── StateManager.h          ← useState hook
    └── EffectManager.h         ← useEffect hook
```

## Why This Architecture?

### Traditional SFML
```cpp
// Imperative - manual updates everywhere
circle.setPosition(x, y);
if (x > 100) {
    circle.setColor(red);
}
// State scattered, hard to test
```

### Reactive SFML
```cpp
// Declarative - describe what to show
render() {
    return Circle({
        {"position", state.position},
        {"color", state.position.x > 100 ? red : green}
    });
}
// Predictable: same state = same output
```

## Core Concepts

### 1. **Single Source of Truth**
All game state in one place:
```cpp
auto engine = Engine<GameState>(...);
engine.getStore().getState();  // One place to look!
```

### 2. **Immutable Updates**
State never mutates, always returns new state:
```cpp
GameState reducer(const GameState& state, const Action& action) {
    auto newState = GameState(state);  // Copy
    newState.position += delta;        // Modify copy
    return newState;                   // Return new state
}
```

### 3. **Declarative Rendering**
Describe WHAT to show, not HOW:
```cpp
Circle({ {"position", pos}, {"radius", 30} })
// Engine figures out the "how"
```

### 4. **Automatic Reconciliation**
Only updates what changed:
```cpp
// Old: Circle at (400, 300)
// New: Circle at (400, 290)
// Reconciler: "Only position changed, update just that!"
circle->setPosition(400, 290);  // Minimal SFML call
```

## Style Guide

This code follows modern C++ style:

```cpp
// ✓ Reactive namespace (was ReactSFML)
namespace Reactive { ... }

// ✓ Clean class name (was ReactSFMLEngine)
auto engine = Engine<GameState>(...);

// ✓ Explicit this->
this->position.set(newPos);

// ✓ auto with explicit type
auto circle = sf::CircleShape(30.0f);

// ✓ Guard clauses
if (!this->rootComponent)
    return;
```

See **STYLE_GUIDE.md** for complete conventions.

## Extending the Example

### Add Another Component
```cpp
class TextComponent : public Component {
    std::shared_ptr<RenderNode> render() override {
        auto props = Props();
        props["content"] = "Score: " + std::to_string(score);
        props["position"] = sf::Vector2f(10, 10);
        return Text(props);
    }
};
```

### Add More State
```cpp
struct GameState : public State {
    sf::Vector2f position;
    int score;           // Add this
    bool gameOver;       // And this
};
```

### Add More Actions
```cpp
inline Action createIncrementScoreAction() {
    return Action{"INCREMENT_SCORE", 0};
}

// In reducer:
if (action.type == "INCREMENT_SCORE")
    newState.score++;
```

### Use Hooks (useState, useEffect)
```cpp
class Player : public Component {
    void onMount() override {
        auto health = useState(100);

        useEffect([this, health]() {
            if (health.get() <= 0)
                gameOver();
        }, {});
    }
};
```

## Documentation

- **HOW_IT_WORKS.md** - Complete architecture walkthrough
- **STYLE_GUIDE.md** - Coding conventions
- **ARCHITECTURE_OVERVIEW.md** - Visual diagrams
- **TESTING.md** - Testing guide

## Benefits Summary

| Feature | Traditional | Reactive SFML |
|---------|------------|---------------|
| **State** | Scattered | Centralized |
| **Updates** | Manual sync | Automatic |
| **Testing** | Hard | Easy (pure functions) |
| **Debugging** | Trace code | Track actions |
| **Predictable** | No | Yes (state → UI) |

## Example Output

```
=== Reactive SFML - Minimal Demo ===
Controls: WASD or Arrow Keys to move

[ACTION] MOVE
[ACTION] MOVE
[ACTION] MOVE
Done!
```

## Next Steps

1. ✅ **Run the minimal example** - Verify it works
2. 🎮 **Add a second circle** - Practice component composition
3. 📊 **Add score tracking** - Practice state management
4. 🎯 **Add collision detection** - Practice reducers
5. 🚀 **Build your game!** - Use the architecture

---

**Current:** Minimal working example (1 component, ~130 lines)
**Status:** Clean, modern C++ with React principles ✓

## Requirements

- **C++20** or later
- **SFML 2.6.x** (auto-downloaded by CMake)
- **CMake 3.10+**
- **MSVC 2022** (Windows) or **GCC/Clang** (Linux/Mac)

## License

Educational project - use and learn!
