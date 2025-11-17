# React-SFML Architecture Overview

## What We Built

A complete React-inspired game engine for C++ using SFML, implementing:

✅ **Component-based architecture** (like React components)
✅ **Virtual DOM** (RenderNode tree for declarative rendering)
✅ **Hooks system** (useState, useEffect)
✅ **Reconciliation** (efficient diffing and patching)
✅ **Global state management** (Redux-like Store/Reducer)
✅ **Unidirectional data flow** (State → Components → Render)
✅ **Example game** (Player, enemies, HUD with working controls)

## Architecture Layers

```
┌─────────────────────────────────────────────────────────────────┐
│                        APPLICATION LAYER                         │
│                                                                  │
│   ┌──────────────┐     ┌──────────────┐     ┌──────────────┐  │
│   │   main.cpp   │────▶│ Game Component│────▶│Event Handler │  │
│   └──────────────┘     └──────────────┘     └──────────────┘  │
│                              │                                  │
└──────────────────────────────┼──────────────────────────────────┘
                               │
                               ▼
┌─────────────────────────────────────────────────────────────────┐
│                     COMPONENT LAYER                              │
│                                                                  │
│   ┌────────────────┐  ┌──────────────┐  ┌─────────────────┐   │
│   │ PlayerComponent│  │EnemyComponent│  │ HUDComponent    │   │
│   │  - useState    │  │  - useState  │  │  - Props only   │   │
│   │  - useEffect   │  │  - render    │  │  - Conditional  │   │
│   └────────────────┘  └──────────────┘  └─────────────────┘   │
│                                                                  │
└──────────────────────────────┼──────────────────────────────────┘
                               │ render()
                               ▼
┌─────────────────────────────────────────────────────────────────┐
│                   VIRTUAL DOM LAYER                              │
│                                                                  │
│   ┌──────────────────────────────────────────────────────────┐ │
│   │                    RenderNode Tree                        │ │
│   │  Container                                                │ │
│   │    ├─ Sprite (player)                                    │ │
│   │    ├─ Circle (enemy1)                                    │ │
│   │    ├─ Circle (enemy2)                                    │ │
│   │    ├─ Text (score)                                       │ │
│   │    └─ Rectangle (health bar)                             │ │
│   └──────────────────────────────────────────────────────────┘ │
│                                                                  │
└──────────────────────────────┼──────────────────────────────────┘
                               │ reconcile()
                               ▼
┌─────────────────────────────────────────────────────────────────┐
│                    RECONCILER LAYER                              │
│                                                                  │
│   ┌──────────────────────────────────────────────────────────┐ │
│   │  Reconciler                                               │ │
│   │  • Diff previous tree vs new tree                        │ │
│   │  • Compute changes (create/update/delete)                │ │
│   │  • Cache SFML objects for reuse                          │ │
│   │  • Apply minimal updates                                 │ │
│   └──────────────────────────────────────────────────────────┘ │
│                                                                  │
└──────────────────────────────┼──────────────────────────────────┘
                               │ render()
                               ▼
┌─────────────────────────────────────────────────────────────────┐
│                       SFML LAYER                                 │
│                                                                  │
│   ┌──────────────────────────────────────────────────────────┐ │
│   │  sf::RenderWindow                                         │ │
│   │    ├─ sf::Sprite (cached)                                │ │
│   │    ├─ sf::CircleShape (cached)                           │ │
│   │    ├─ sf::Text (updated content)                         │ │
│   │    └─ sf::RectangleShape (updated size)                  │ │
│   └──────────────────────────────────────────────────────────┘ │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

## State Management Flow

```
┌──────────────┐
│ User Input   │  (Keyboard: WASD, Space, H)
└──────┬───────┘
       │
       ▼
┌──────────────────────────────────────────┐
│ Event Handler                             │
│ • Convert SFML events to Actions         │
│ • createMovePlayerAction(delta)          │
│ • createSpawnEnemyAction(enemy)          │
│ • createDamagePlayerAction(damage)       │
└──────┬───────────────────────────────────┘
       │
       ▼
┌──────────────────────────────────────────┐
│ Store.dispatch(action)                    │
│ • Run middleware (logger)                │
│ • Execute reducer                        │
└──────┬───────────────────────────────────┘
       │
       ▼
┌──────────────────────────────────────────┐
│ Reducer (Pure Function)                   │
│ GameState reducer(state, action) {       │
│   switch(action.type) {                  │
│     case "MOVE_PLAYER":                  │
│       return {...state, pos: newPos};    │
│     case "SPAWN_ENEMY":                  │
│       return {...state, enemies: [...]};  │
│   }                                       │
│ }                                         │
└──────┬───────────────────────────────────┘
       │
       ▼
┌──────────────────────────────────────────┐
│ New State Created (Immutable)             │
│ • playerPosition updated                 │
│ • enemies array modified                 │
│ • score incremented                      │
└──────┬───────────────────────────────────┘
       │
       ▼
┌──────────────────────────────────────────┐
│ Notify Subscribers                        │
│ • GameComponent.forceUpdate()            │
│ • Triggers re-render                     │
└──────┬───────────────────────────────────┘
       │
       ▼
┌──────────────────────────────────────────┐
│ Component Tree Re-renders                 │
│ • GameComponent.render()                 │
│ •   └─ Player.render() (new position)   │
│ •   └─ Enemy.render() (new positions)   │
│ •   └─ HUD.render() (new score)         │
└──────┬───────────────────────────────────┘
       │
       ▼
┌──────────────────────────────────────────┐
│ Reconciler Diffs Trees                    │
│ • Only updates changed props             │
│ • Reuses cached SFML objects             │
└──────┬───────────────────────────────────┘
       │
       ▼
┌──────────────────────────────────────────┐
│ Screen Updates                            │
│ • window.clear()                         │
│ • window.draw(updated objects)           │
│ • window.display()                       │
└──────────────────────────────────────────┘
```

## File Organization

### Core Engine (`src/core/`)

| File | Purpose | React Equivalent |
|------|---------|-----------------|
| `RenderNode.h` | Virtual DOM nodes | React Elements |
| `Component.h` | Base component class | React.Component |
| `StateManager.h` | Local state management | useState hook |
| `EffectManager.h` | Side effects | useEffect hook |
| `Reconciler.h` | Diff & patch | React Reconciler (Fiber) |
| `Store.h` | Global state | Redux Store |
| `ReactSFMLEngine.h` | Game loop orchestrator | ReactDOM.render |

### Game Components (`src/components/`)

| File | Purpose | React Equivalent |
|------|---------|-----------------|
| `GameState.h` | State types, reducer | Redux types/reducers |
| `BackgroundComponent.h` | Background rendering | Stateless component |
| `PlayerComponent.h` | Player with state | Stateful component |
| `EnemyComponent.h` | Enemy entities | Component with props |
| `HUDComponent.h` | UI overlay | Conditional rendering |
| `GameComponent.h` | Root composition | App component |

## React Patterns Implemented

### 1. Declarative Rendering ✅

```cpp
// Instead of:
sprite.setPosition(x, y);
window.draw(sprite);

// We write:
return Circle({
    {"position", sf::Vector2f(x, y)},
    {"radius", 20.0f}
});
```

### 2. Component Composition ✅

```cpp
class GameComponent : public Component {
    std::shared_ptr<RenderNode> render() override {
        return Container({
            background->render(),    // Child component
            player->render(),        // Child component
            ...enemies,              // List rendering
            hud->render()            // Child component
        });
    }
};
```

### 3. Hooks (useState) ✅

```cpp
void onMount() override {
    auto position = useState(sf::Vector2f(100, 100));

    // Later:
    position.set(sf::Vector2f(200, 200));  // Triggers re-render
}
```

### 4. Hooks (useEffect) ✅

```cpp
useEffect([this]() {
    std::cout << "Health changed: " << health << std::endl;
}, {health});  // Runs when health changes
```

### 5. Unidirectional Data Flow ✅

```
State (Store) → Components (Props) → UI (Render)
      ↑                                    │
      └────── Actions (Events) ────────────┘
```

### 6. Immutable State Updates ✅

```cpp
GameState reducer(const GameState& state, const Action& action) {
    GameState newState = state;  // Copy
    newState.score += 100;       // Modify copy
    return newState;             // Return new state
}
```

### 7. Conditional Rendering ✅

```cpp
if (state.gameOver)
    return GameOverScreen({score: state.score});

return GameplayView();
```

### 8. List Rendering ✅

```cpp
for (const auto& enemy : state.enemies) {
    container->children.push_back(
        enemyComponent(enemy)->render()
    );
}
```

### 9. Keys for Reconciliation ✅

```cpp
auto sprite = Sprite(props);
sprite->key = "player_" + std::to_string(id);
// Reconciler uses keys to match nodes
```

### 10. Middleware Pattern ✅

```cpp
engine.getStore().addMiddleware(createLoggerMiddleware());
// Logs every action dispatched
```

## Performance Characteristics

### Virtual DOM Benefits

1. **Batched Updates** - Multiple state changes in one frame trigger only one render
2. **Minimal SFML Calls** - Only changed properties are updated
3. **Object Reuse** - SFML objects cached and reused across frames
4. **Efficient Diffing** - Key-based reconciliation prevents unnecessary work

### Trade-offs

| Aspect | Traditional SFML | React-SFML |
|--------|-----------------|------------|
| **Learning Curve** | Low | Medium (need React knowledge) |
| **Performance** | Direct control | Slight overhead from diffing |
| **Maintainability** | Scattered logic | Centralized, predictable |
| **Scalability** | Manual management | Automatic optimization |
| **Debugging** | Imperative traces | Declarative traces |

## Usage Example

```cpp
// 1. Define your game state
struct GameState {
    sf::Vector2f playerPos;
    int score;
    std::vector<Enemy> enemies;
};

// 2. Define reducer
GameState gameReducer(const GameState& state, const Action& action) {
    // Handle actions and return new state
}

// 3. Create engine
ReactSFMLEngine<GameState> engine(800, 600, "Game", initialState, gameReducer);

// 4. Set root component
auto game = std::make_shared<GameComponent>(&engine.getStore());
engine.setRoot(game);

// 5. Run
engine.run();
```

## Extending the Architecture

### Custom Hooks

```cpp
// usePhysics hook
PhysicsBody usePhysics(sf::Vector2f initialVelocity) {
    auto velocity = useState(initialVelocity);
    auto position = useState(sf::Vector2f(0, 0));

    useEffect([=]() {
        // Apply physics each frame
        position.set(position.get() + velocity.get() * deltaTime);
    }, {});

    return PhysicsBody{position, velocity};
}
```

### Context API

```cpp
class ThemeContext {
    static sf::Color primaryColor;

    static void provide(sf::Color color) {
        primaryColor = color;
    }

    static sf::Color use() {
        return primaryColor;
    }
};
```

### Custom Components

```cpp
class Button : public Component {
    std::shared_ptr<RenderNode> render() override {
        return Rectangle({
            {"size", sf::Vector2f(200, 50)},
            {"color", isHovered ? hoverColor : normalColor}
        });
    }
};
```

## Why This Architecture?

### Traditional Game Engine Problems

❌ **Scattered State** - Game state spread across many objects
❌ **Side Effects Everywhere** - Hard to predict what changes when
❌ **Manual Updates** - Must remember to update UI when data changes
❌ **Complex Synchronization** - Keeping UI in sync with data is hard
❌ **Difficult Testing** - Tightly coupled rendering and logic

### React-SFML Solutions

✅ **Centralized State** - Single source of truth (Store)
✅ **Predictable Updates** - State → UI is a pure function
✅ **Automatic Synchronization** - UI updates automatically
✅ **Easy Testing** - Pure render functions, mockable state
✅ **Declarative** - Describe WHAT, not HOW

## Real-World Applications

This architecture is ideal for:

- **UI-heavy games** (inventory systems, menus, HUDs)
- **Turn-based games** (state changes are discrete)
- **Puzzle games** (state-driven logic)
- **Educational games** (clear data flow)
- **Prototypes** (rapid iteration)

## Conclusion

We've successfully ported React's core principles to C++ game development:

✅ **Component-based architecture**
✅ **Virtual DOM with reconciliation**
✅ **Hooks (useState, useEffect)**
✅ **Redux-like global state management**
✅ **Declarative rendering**
✅ **Unidirectional data flow**

The result is a game engine that's:
- **Predictable** - Same state = Same output
- **Maintainable** - Clear separation of concerns
- **Scalable** - Easy to add features
- **Familiar** - React developers feel at home

This demonstrates that React's paradigm works beautifully outside the web!
