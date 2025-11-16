# Minimal v1: Just a Window

## What This Does
Opens a black window. That's it!

## Files Needed
- main.cpp
- BaseRunner.h
- BaseRunner.cpp

## What You'll See
A 1920x1080 black window that you can close.

## Key Concepts
- SFML window creation
- Basic game loop (update/render cycle at 60 FPS)
- Event handling (closing the window)

## To Compile (example)
```bash
g++ main.cpp BaseRunner.cpp -o minimal_v1 -lsfml-graphics -lsfml-window -lsfml-system
```

## Next Step
Minimal v2 will add the GameObjectManager and a simple shape!
