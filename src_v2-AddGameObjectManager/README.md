# Minimal v2: GameObjectManager + Circle

## What This Does
Shows a green circle moving across the screen (wraps around when it reaches the edge).

## Files Needed
- main.cpp
- BaseRunner.h / BaseRunner.cpp
- GameObjectManager.h / GameObjectManager.cpp
- AGameObject.h / AGameObject.cpp
- SimpleCircle.h / SimpleCircle.cpp

## What You'll See
A green circle moving from left to right continuously.

## Key Concepts
- GameObjectManager singleton pattern
- Abstract base class (AGameObject)
- Concrete implementation (SimpleCircle)
- Update loop modifying position over time
- Manager calling update/draw on all objects

## To Compile (example)
```bash
g++ main.cpp BaseRunner.cpp GameObjectManager.cpp AGameObject.cpp SimpleCircle.cpp \
    -o minimal_v2 -lsfml-graphics -lsfml-window -lsfml-system
```

## Things to Try
1. Create multiple circles with different speeds
2. Add keyboard controls to move the circle (Arrow keys)
3. Make the circle bounce instead of wrap
4. Change the color based on position

## Next Step
Minimal v3 will add text rendering (FPS counter) without needing external font files!
