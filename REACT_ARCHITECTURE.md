# React-Inspired SFML Game Engine Architecture

## Overview

This is a complete implementation of a React-inspired game engine built on top of SFML. It brings React's declarative, component-based architecture to C++ game development.

## Core Principles

### 1. **Declarative Rendering**
Instead of imperatively manipulating SFML objects, you describe WHAT the UI should look like based on state.

```cpp
// Traditional SFML (Imperative)
sprite.setPosition(100, 100);
sprite.setColor(sf::Color::Red);
window.draw(sprite);

// React-SFML (Declarative)
auto player = Circle({
    {"position", sf::Vector2f(100, 100)},
    {"color", sf::Color::Red},
    {"radius", 20.0f}
});
```

### 2. **Component-Based Architecture**
Everything is a component that can be composed together.

```cpp
class GameScene : public Component {
    std::shared_ptr<RenderNode> render() override {
        return Container({
            background->render(),
            player->render(),
            ...enemies,
            hud->render()
        });
    }
};
```

### 3. **Unidirectional Data Flow**
State flows down through props, events flow up through actions.

```
State (Redux Store)
    ↓
Components (Read state)
    ↓
Render Tree (Virtual DOM)
    ↓
Reconciler (Diff & Patch)
    ↓
SFML Objects (Actual rendering)
```

## Architecture Components

### Core Systems (`src/core/`)

#### 1. **RenderNode.h** - Virtual DOM
- Represents the virtual DOM tree (like React elements)
- Node types: Container, Sprite, Text, Rectangle, Circle
- Props system for declarative configuration
- Key-based reconciliation support

```cpp
auto node = Sprite({
    {"texture", texturePtr},
    {"position", sf::Vector2f(100, 100)},
    {"scale", sf::Vector2f(2.0f, 2.0f)}
});
node->key = "player_sprite";  // For efficient reconciliation
```

#### 2. **Component.h** - Base Component Class
- Abstract base class for all game components
- Lifecycle methods: `onMount()`, `onUnmount()`, `onUpdate()`
- Main method: `render()` - returns virtual DOM tree
- Provides hooks: `useState()`, `useEffect()`

```cpp
class MyComponent : public Component {
    void onMount() override {
        auto health = useState<int>(100);
        useEffect([this]() {
            std::cout << "Mounted!" << std::endl;
        }, {});
    }

    std::shared_ptr<RenderNode> render() override {
        return Text({{"content", "Hello World"}});
    }
};
```

#### 3. **StateManager.h** - React Hooks (useState)
- Manages component-local state
- `useState<T>(initialValue)` returns `StateRef<T>`
- Automatically triggers re-render on state changes
- State is tied to component lifecycle

```cpp
auto [count, setCount] = useState<int>(0);

// Later...
setCount(count + 1);  // Triggers re-render
```

#### 4. **EffectManager.h** - React Hooks (useEffect)
- Side effect management
- Runs effects when dependencies change
- Supports cleanup functions
- Runs after render (like React)

```cpp
// Run once on mount
useEffect([]() {
    std::cout << "Component mounted" << std::endl;
}, {});

// Run when health changes
useEffect([=]() {
    if (health <= 0)
        gameOver();
}, {health});

// With cleanup
useEffect([]() {
    auto timer = startTimer();
    return [=]() { stopTimer(timer); };  // Cleanup
}, {});
```

#### 5. **Reconciler.h** - Virtual DOM Diffing
- Diffs previous and current render trees
- Computes minimal set of changes
- Creates/updates/deletes SFML objects as needed
- Caches SFML objects for reuse
- Handles actual rendering to window

```cpp
// Internally:
reconciler.reconcile(newTree);  // Diff & patch
reconciler.render();            // Draw to screen
```

#### 6. **Store.h** - Redux-like State Management
- Global state container
- Actions trigger reducers
- Reducers return new state (immutable)
- Subscribers notified on state change
- Middleware support (logging, async, etc.)

```cpp
// Define state
struct GameState {
    int score;
    sf::Vector2f playerPos;
};

// Define reducer
GameState reducer(const GameState& state, const Action& action) {
    if (action.type == "MOVE_PLAYER") {
        auto newState = state;
        newState.playerPos += action.getPayload<sf::Vector2f>();
        return newState;
    }
    return state;
}

// Create store
Store<GameState> store(initialState, reducer);

// Dispatch actions
store.dispatch({"MOVE_PLAYER", sf::Vector2f(5, 0)});
```

#### 7. **ReactSFMLEngine.h** - Main Game Loop
- Orchestrates all systems
- Runs the game loop (event → update → render → commit)
- Manages window and timing
- Integrates store with components

```cpp
ReactSFMLEngine<GameState> engine(800, 600, "Game", initialState, reducer);
engine.setRoot(gameComponent);
engine.setEventHandler(handleEvents);
engine.run();  // Start game loop
```

### Game Components (`src/components/`)

#### GameState.h
- Defines game state structure
- Action creators
- Main reducer function

#### BackgroundComponent.h
- Simple background rendering

#### PlayerComponent.h
- Player entity with local state
- Demonstrates useState hook

#### EnemyComponent.h
- Enemy entity with movement

#### HUDComponent.h
- UI overlay for score/health
- Conditional rendering (game over screen)

#### GameComponent.h
- Root component that composes all others
- Subscribes to store changes
- Manages child component lifecycle

## Data Flow

### Game Loop (60 FPS)

```
┌─────────────────────────────────────┐
│  1. Process Events (SFML)           │
│     - Keyboard, mouse, window       │
│     - Convert to Actions            │
│     - Dispatch to Store             │
└────────────┬────────────────────────┘
             ↓
┌─────────────────────────────────────┐
│  2. Run Reducer                     │
│     - Current state + Action        │
│     - Returns new state             │
│     - Notify subscribers            │
└────────────┬────────────────────────┘
             ↓
┌─────────────────────────────────────┐
│  3. Update Components               │
│     - component.onUpdate(deltaTime) │
│     - Update from new state         │
└────────────┬────────────────────────┘
             ↓
┌─────────────────────────────────────┐
│  4. Run Effects                     │
│     - Check dependency changes      │
│     - Run effect callbacks          │
│     - Run cleanup functions         │
└────────────┬────────────────────────┘
             ↓
┌─────────────────────────────────────┐
│  5. Render Phase                    │
│     - component.render()            │
│     - Generate virtual DOM tree     │
└────────────┬────────────────────────┘
             ↓
┌─────────────────────────────────────┐
│  6. Reconciliation Phase            │
│     - Diff previous vs new tree     │
│     - Compute minimal changes       │
│     - Update SFML objects           │
└────────────┬────────────────────────┘
             ↓
┌─────────────────────────────────────┐
│  7. Commit Phase                    │
│     - window.clear()                │
│     - Draw SFML objects             │
│     - window.display()              │
└─────────────────────────────────────┘
```

## Usage Example

### Complete Game Setup

```cpp
#include "core/ReactSFMLEngine.h"
#include "components/GameState.h"
#include "components/GameComponent.h"

int main() {
    // 1. Create engine with initial state
    ReactSFMLEngine<GameState> engine(
        800, 600,
        "My Game",
        GameState(),        // Initial state
        gameReducer         // Reducer function
    );

    // 2. Optional: Add middleware
    engine.getStore().addMiddleware(createLoggerMiddleware());

    // 3. Set event handler
    engine.setEventHandler([](sf::Event& event, Store<GameState>& store) {
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Left)
                store.dispatch({"MOVE_LEFT", 5.0f});
        }
    });

    // 4. Create root component
    auto game = std::make_shared<GameComponent>(&engine.getStore());
    engine.setRoot(game);

    // 5. Run game loop
    engine.run();

    return 0;
}
```

### Creating Custom Components

```cpp
class HealthBar : public Component {
public:
    HealthBar(int maxHealth) : maxHealth(maxHealth) {}

    void onMount() override {
        // Local state
        currentHealth = useState<int>(maxHealth);

        // Effect: log when health changes
        useEffect([=]() {
            std::cout << "Health: " << currentHealth.get() << std::endl;
        }, {currentHealth.get()});
    }

    std::shared_ptr<RenderNode> render() override {
        float width = (currentHealth.get() / (float)maxHealth) * 200.0f;

        return Rectangle({
            {"position", sf::Vector2f(10, 10)},
            {"size", sf::Vector2f(width, 20)},
            {"color", sf::Color::Green}
        });
    }

    void setHealth(int health) {
        currentHealth.set(health);
    }

private:
    int maxHealth;
    StateRef<int> currentHealth;
};
```

## Key Benefits

### ✅ **Declarative & Predictable**
- Same state always produces same output
- Easy to reason about game logic

### ✅ **Composable**
- Build complex UIs from simple components
- Reusable component library

### ✅ **Performance**
- Only updates what changed (reconciliation)
- SFML objects are cached and reused

### ✅ **Maintainable**
- Clear separation of concerns
- Unidirectional data flow
- Type-safe throughout

### ✅ **Testable**
- Pure render functions
- Reducers are pure functions
- Easy to mock state

### ✅ **Familiar**
- React developers feel at home
- Well-understood patterns
- Large ecosystem of knowledge

## Comparison

### Traditional SFML Game

```cpp
class Game {
    sf::Sprite playerSprite;
    std::vector<sf::Sprite> enemySprites;
    int score;

    void update() {
        // Imperative logic scattered everywhere
        if (health <= 0) {
            gameOver = true;
            playerSprite.setColor(sf::Color::Red);
        }
        if (collides(player, enemy)) {
            health -= 10;
            updateHealthBar();  // Side effect!
        }
        playerSprite.setPosition(playerX, playerY);
    }

    void draw() {
        // Manual drawing
        window.draw(background);
        window.draw(playerSprite);
        for (auto& enemy : enemySprites)
            window.draw(enemy);
        window.draw(scoreText);
    }
};
```

### React-SFML Game

```cpp
class Game : public Component {
    std::shared_ptr<RenderNode> render() override {
        const auto& state = store->getState();

        // Declarative - just describe what to show
        return Container({
            background->render(),
            player->render(),
            ...renderEnemies(state.enemies),
            hud->render()
        });
    }
};

// State updates trigger automatic re-render
store.dispatch({"DAMAGE_PLAYER", 10});
// Everything updates automatically!
```

## File Structure

```
src/
├── core/                      # Core engine systems
│   ├── RenderNode.h           # Virtual DOM nodes
│   ├── Component.h            # Base component class
│   ├── StateManager.h         # useState hook
│   ├── EffectManager.h        # useEffect hook
│   ├── Reconciler.h           # Virtual DOM diffing
│   ├── Store.h                # Redux-like store
│   └── ReactSFMLEngine.h      # Main game loop
│
├── components/                # Game components
│   ├── GameState.h            # State & reducer
│   ├── BackgroundComponent.h  # Background
│   ├── PlayerComponent.h      # Player entity
│   ├── EnemyComponent.h       # Enemy entity
│   ├── HUDComponent.h         # UI overlay
│   └── GameComponent.h        # Root component
│
└── main.cpp                   # Entry point
```

## Building

```bash
# Configure
cmake -B build -S .

# Build
cmake --build build

# Run
./build/bin/STDISCM_P3
```

## Controls (Demo Game)

- **WASD / Arrow Keys** - Move player
- **Space** - Spawn enemy
- **H** - Take damage (test)

## Future Enhancements

- [ ] Context API for avoiding prop drilling
- [ ] Custom hooks (usePhysics, useAnimation, etc.)
- [ ] Better key-based reconciliation
- [ ] Profiler/DevTools
- [ ] Hot reload support
- [ ] Time-travel debugging
- [ ] Concurrent rendering
- [ ] Suspense for async loading
- [ ] Portals for rendering to different layers
- [ ] Memoization (useMemo, React.memo)

## Acknowledgments

Inspired by:
- React.js (Facebook/Meta)
- Redux (Dan Abramov)
- SFML (Laurent Gomila)

## License

This is a demonstration/educational project showcasing how React principles can be applied to game development with C++ and SFML.
