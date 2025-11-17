# How the React-SFML Architecture Works

A step-by-step guide showing exactly how the architecture processes input and renders to screen.

## Overview: From Keypress to Pixels

```
User presses 'W' key
    ↓
Event Handler converts to Action { type: "MOVE", delta: (0, -10) }
    ↓
Store.dispatch(action) calls Reducer
    ↓
Reducer returns new State { position: (400, 290) }
    ↓
Store notifies subscribers "state changed!"
    ↓
Component.render() creates Virtual DOM tree
    ↓
Reconciler compares old tree vs new tree
    ↓
Reconciler updates SFML circle.setPosition(400, 290)
    ↓
Screen displays circle at new position
```

Let's trace through each step with actual code!

---

## 1. The Game Loop (ReactSFMLEngine)

**Location:** `src/core/ReactSFMLEngine.h`

Every frame (60 times per second), this loop runs:

```cpp
void run() {
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Time deltaTime = clock.restart();

        // STEP 1: Process events (keyboard, mouse, window)
        processEvents();

        // STEP 2: Update components (game logic)
        if (rootComponent) {
            rootComponent->onUpdate(deltaTime);
        }

        // STEP 3: Run effects (side effects from useEffect)
        EffectManager::getInstance().runEffects();

        // STEP 4: Render phase (create virtual DOM)
        if (rootComponent) {
            auto renderTree = rootComponent->render();

            // STEP 5: Reconciliation (diff and patch)
            reconciler.reconcile(renderTree);

            // STEP 6: Commit phase (draw to screen)
            reconciler.render();
        }
    }
}
```

**React Equivalent:**
```jsx
// React's render loop (simplified)
while (true) {
    handleEvents();          // Event listeners
    runEffects();            // useEffect callbacks
    const vdom = render();   // Component.render()
    reconcile(vdom);         // Virtual DOM diffing
    commitToDOM();           // Update actual DOM
}
```

---

## 2. Event Handling → Actions

**Example:** User presses 'W' to move up

### Step 2.1: SFML Event Detection

```cpp
void processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        // Pass to custom event handler
        if (eventHandler) {
            eventHandler(event, store);
        }
    }
}
```

### Step 2.2: Event Handler Converts to Action

**Location:** `src/main_simple.cpp` (or `src/main_game.cpp`)

```cpp
void handleEvents(sf::Event& event, Store<SimpleState>& store) {
    if (event.type == sf::Event::KeyPressed) {
        sf::Vector2f delta(0, 0);
        float speed = 10.0f;

        switch (event.key.code) {
            case sf::Keyboard::W:
                delta.y = -speed;  // Negative Y = up
                break;
            case sf::Keyboard::S:
                delta.y = speed;   // Positive Y = down
                break;
            case sf::Keyboard::A:
                delta.x = -speed;  // Left
                break;
            case sf::Keyboard::D:
                delta.x = speed;   // Right
                break;
        }

        if (delta.x != 0 || delta.y != 0) {
            // Create and dispatch action
            store.dispatch(createMoveAction(delta));
        }
    }
}
```

**Action Created:**
```cpp
Action {
    type: "MOVE",
    payload: sf::Vector2f(0, -10)  // Move up by 10 pixels
}
```

**React Equivalent:**
```jsx
function handleKeyPress(event) {
    if (event.key === 'w') {
        dispatch({ type: 'MOVE', payload: { x: 0, y: -10 } });
    }
}
```

---

## 3. Store → Reducer → New State

**Location:** `src/core/Store.h`

### Step 3.1: Store Receives Action

```cpp
template<typename TState>
class Store {
    void dispatch(const Action& action) {
        // 1. Run middleware (logging, etc.)
        for (auto& middleware : middlewares) {
            middleware(action);
            // Prints: [ACTION] MOVE
        }

        // 2. Run reducer to get new state
        TState newState = reducer(state, action);

        // 3. Update state
        state = newState;

        // 4. Notify all subscribers
        for (auto& subscriber : subscribers) {
            subscriber(state);
            // This triggers component re-render!
        }
    }

private:
    TState state;
    Reducer<TState> reducer;
    vector<function<void(const TState&)>> subscribers;
};
```

### Step 3.2: Reducer Computes New State

**Location:** `src/main_simple.cpp`

```cpp
SimpleState simpleReducer(const SimpleState& state, const Action& action) {
    SimpleState newState = state;  // Copy current state

    if (action.type == "MOVE") {
        auto delta = action.getPayload<sf::Vector2f>();

        // OLD position: (400, 300)
        // Delta:        (0, -10)
        // NEW position: (400, 290)
        newState.position = state.position + delta;

        // Clamp to screen bounds
        newState.position.x = std::max(50.0f, std::min(750.0f, newState.position.x));
        newState.position.y = std::max(50.0f, std::min(550.0f, newState.position.y));
    }

    return newState;  // Return NEW state (immutable pattern!)
}
```

**Before:** `{ position: (400, 300) }`
**After:** `{ position: (400, 290) }`

**React Equivalent:**
```jsx
function gameReducer(state, action) {
    switch (action.type) {
        case 'MOVE':
            return {
                ...state,
                position: {
                    x: state.position.x + action.payload.x,
                    y: state.position.y + action.payload.y
                }
            };
        default:
            return state;
    }
}
```

---

## 4. State Change → Component Re-render

### Step 4.1: Subscriber Notification

When the store updates state, it calls all subscribers:

```cpp
// In Store.dispatch():
for (auto& subscriber : subscribers) {
    subscriber(newState);  // Calls each registered callback
}
```

### Step 4.2: Component Subscribed to Store

**Location:** `src/main_simple.cpp`

```cpp
class SimpleCircle : public Component {
public:
    SimpleCircle(Store<SimpleState>* store) : store(store) {}

    std::shared_ptr<RenderNode> render() override {
        // Read current state from store
        const auto& state = store->getState();

        // Create virtual DOM node with NEW position
        Props circleProps;
        circleProps["radius"] = 30.0f;
        circleProps["color"] = sf::Color::Green;
        circleProps["position"] = state.position;  // (400, 290) now!

        return Circle(circleProps);
    }

private:
    Store<SimpleState>* store;
};
```

### Step 4.3: Virtual DOM Created

After `render()` returns, we have a **RenderNode tree**:

```cpp
RenderNode {
    type: NodeType::Circle,
    props: {
        "radius":   30.0f,
        "color":    sf::Color::Green,
        "position": sf::Vector2f(400, 290)  // NEW position!
    },
    children: []
}
```

**React Equivalent:**
```jsx
function SimpleCircle() {
    const state = useSelector(state => state);

    return (
        <Circle
            radius={30}
            color="green"
            position={state.position}  // { x: 400, y: 290 }
        />
    );
}
```

---

## 5. Reconciliation: Diff & Patch

**Location:** `src/core/Reconciler.h`

The reconciler compares OLD virtual DOM vs NEW virtual DOM:

### Step 5.1: Detect Changes

```cpp
void reconcile(shared_ptr<RenderNode> newTree) {
    if (!previousTree) {
        // First render - create everything
        rootInstance = createInstance(newTree);
    } else {
        // Subsequent render - diff and update
        auto changes = diff(previousTree, newTree, rootInstance);

        // Apply changes if props changed
        if (!changes.empty() || propsChanged(previousTree->props, newTree->props)) {
            // Recreate SFML instance with new props
            rootInstance = createInstance(newTree);
        }
    }

    previousTree = newTree;
}
```

### Step 5.2: Compare Props

```cpp
bool propsChanged(const Props& oldProps, const Props& newProps) {
    for (const auto& [key, value] : newProps) {
        auto it = oldProps.find(key);
        if (it == oldProps.end())
            return true;  // New prop added

        if (!propValueEquals(it->second, value))
            return true;  // Value changed!
    }
    return false;
}

bool propValueEquals(const PropValue& a, const PropValue& b) {
    // ... type checking ...

    if (auto* aVec = get_if<sf::Vector2f>(&a)) {
        auto bVec = get<sf::Vector2f>(b);
        // OLD: (400, 300)
        // NEW: (400, 290)
        return aVec->x == bVec.x && aVec->y == bVec.y;  // FALSE! Different!
    }
    // ...
}
```

**Result:** Props changed! Need to update SFML object.

### Step 5.3: Update SFML Object

```cpp
shared_ptr<SFMLInstance> createInstance(shared_ptr<RenderNode> node) {
    auto instance = make_shared<SFMLInstance>();

    switch (node->type) {
        case NodeType::Circle: {
            // Create new SFML circle
            auto circle = make_shared<sf::CircleShape>();
            circle->setRadius(node->getProp<float>("radius", 50.0f));
            circle->setFillColor(node->getProp<sf::Color>("color", sf::Color::White));

            instance->drawable = circle;
            instance->transformable = circle;
            break;
        }
        // ... other types ...
    }

    // Apply transformations
    if (instance->transformable) {
        auto pos = node->getProp<sf::Vector2f>("position", sf::Vector2f(0, 0));
        instance->transformable->setPosition(pos);  // (400, 290) ✓
    }

    return instance;
}
```

**React Equivalent:**
```jsx
// React's reconciler (simplified)
function reconcile(oldVNode, newVNode) {
    if (oldVNode.props.position !== newVNode.props.position) {
        // Update DOM element
        domElement.style.left = newVNode.props.position.x + 'px';
        domElement.style.top = newVNode.props.position.y + 'px';
    }
}
```

---

## 6. Render to Screen

**Location:** `src/core/Reconciler.h`

```cpp
void render() {
    if (rootInstance) {
        window->clear(sf::Color::Black);  // Clear screen
        renderInstance(rootInstance);      // Draw all instances
        window->display();                 // Show on screen
    }
}

void renderInstance(shared_ptr<SFMLInstance> instance) {
    // Draw this instance
    if (instance->drawable) {
        window->draw(*instance->drawable);  // Circle at (400, 290)!
    }

    // Recursively draw children
    for (auto& child : instance->children) {
        renderInstance(child);
    }
}
```

**Screen Update:** Circle moves from (400, 300) → (400, 290) ✓

---

## Complete Flow Example

Let's trace ONE complete update cycle:

### Frame 0: Initial State

```cpp
State: { position: (400, 300) }

Virtual DOM:
  Circle { position: (400, 300), radius: 30, color: Green }

SFML Object:
  sf::CircleShape at position (400, 300)

Screen:
  [Circle drawn at (400, 300)]
```

### Frame 1: User Presses 'W'

```cpp
1. EVENT: sf::Event::KeyPressed, key = W
   ↓
2. EVENT HANDLER: Creates Action { type: "MOVE", payload: (0, -10) }
   ↓
3. STORE.DISPATCH: Calls reducer
   ↓
4. REDUCER:
   Old state: { position: (400, 300) }
   + Delta: (0, -10)
   = New state: { position: (400, 290) }
   ↓
5. STORE: Notifies subscribers
   ↓
6. COMPONENT.RENDER():
   Virtual DOM:
     Circle { position: (400, 290), radius: 30, color: Green }
   ↓
7. RECONCILER.DIFF():
   Old props: { position: (400, 300) }
   New props: { position: (400, 290) }
   Change detected: position changed!
   ↓
8. RECONCILER.UPDATE():
   circle.setPosition(400, 290)
   ↓
9. RECONCILER.RENDER():
   window.clear()
   window.draw(circle)  // Circle at (400, 290)
   window.display()
   ↓
10. SCREEN:
    [Circle drawn at (400, 290)]  ✓ Moved up!
```

---

## React Hooks in Action

### useState Example

**Location:** `src/components/PlayerComponent.h`

```cpp
class PlayerComponent : public Component {
    void onMount() override {
        // Create state
        position = useState(initialPosition);
        health = useState(initialHealth);
    }

    std::shared_ptr<RenderNode> render() override {
        // Read state
        auto pos = position.get();
        auto hp = health.get();

        // Render based on state
        return Circle({
            {"position", pos},
            {"radius", hp > 50 ? 30.0f : 15.0f}  // Shrink when low health
        });
    }

    void takeDamage(int amount) {
        // Update state
        health.set(health.get() - amount);  // Triggers re-render!
    }

private:
    StateRef<sf::Vector2f> position;
    StateRef<int> health;
};
```

**How it works:**

1. `useState(100)` → StateManager creates storage
2. Returns `StateRef<int>` with get/set methods
3. `health.set(90)` → Marks component for re-render
4. Next frame: `render()` called with new state

**React Equivalent:**
```jsx
function PlayerComponent() {
    const [health, setHealth] = useState(100);

    return (
        <Circle
            radius={health > 50 ? 30 : 15}
        />
    );
}
```

### useEffect Example

```cpp
void onMount() override {
    auto health = useState(100);

    // Run when health changes
    useEffect([this, health]() {
        std::cout << "Health: " << health.get() << std::endl;

        if (health.get() <= 0) {
            gameOver();
        }
    }, {});  // Empty deps = run every render
}
```

---

## Component Composition

**Location:** `src/components/GameComponent.h`

```cpp
class GameComponent : public Component {
    std::shared_ptr<RenderNode> render() override {
        auto container = Container();

        // Compose multiple components
        container->children.push_back(background->render());
        container->children.push_back(player->render());

        for (auto& enemy : enemies) {
            container->children.push_back(enemy->render());
        }

        container->children.push_back(hud->render());

        return container;
    }
};
```

**Resulting Virtual DOM:**
```
Container
  ├─ Rectangle (background)
  ├─ Circle (player)
  ├─ Circle (enemy1)
  ├─ Circle (enemy2)
  └─ Text (HUD score)
```

**React Equivalent:**
```jsx
function GameComponent() {
    return (
        <Container>
            <Background />
            <Player />
            {enemies.map(e => <Enemy key={e.id} data={e} />)}
            <HUD />
        </Container>
    );
}
```

---

## Summary: The React Pattern in C++

| React Concept | C++ Implementation | Location |
|--------------|-------------------|----------|
| Component | `class Component` | `core/Component.h` |
| `render()` | `virtual render()` | Returns `RenderNode` |
| Virtual DOM | `RenderNode` tree | `core/RenderNode.h` |
| Reconciliation | `Reconciler::diff()` | `core/Reconciler.h` |
| `useState` | `StateRef<T>` | `core/StateManager.h` |
| `useEffect` | `useEffect()` | `core/EffectManager.h` |
| Redux Store | `Store<TState>` | `core/Store.h` |
| Actions | `Action` struct | Dispatched to reducer |
| Reducer | Pure function | Returns new state |

## Why This Works

**Predictable:** Same state always produces same render
```cpp
render(state) → virtual DOM
// Always the same output for same input!
```

**Efficient:** Only update what changed
```cpp
diff(oldTree, newTree) → minimal changes
// Don't recreate everything!
```

**Declarative:** Describe WHAT, not HOW
```cpp
// Not: circle.setPosition(x, y)
// But: Circle({ position: (x, y) })
```

**Composable:** Build complex from simple
```cpp
GameScene = Background + Player + Enemies + HUD
```

---

## Try It Yourself

Build `main_simple.cpp` and watch the flow:

1. Press 'W'
2. Console shows: `[ACTION] MOVE`
3. Console shows: `New position: (400, 290)`
4. Circle moves on screen

You just saw the complete React data flow in C++! 🎉
