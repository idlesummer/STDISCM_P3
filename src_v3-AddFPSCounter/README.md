# Minimal v3: FPS Counter

## What This Does
Shows 3 green circles moving across the screen + an FPS counter in the top-left corner.

## Files Needed
- main.cpp
- BaseRunner.h / BaseRunner.cpp
- GameObjectManager.h / GameObjectManager.cpp
- AGameObject.h / AGameObject.cpp
- SimpleCircle.h / SimpleCircle.cpp
- SimpleFPSCounter.h / SimpleFPSCounter.cpp

## What You'll See
- 3 green circles moving from left to right
- FPS counter showing ~60 FPS in the top-left corner

## Key Concepts
- Text rendering with SFML
- FPS calculation (frames per second over 1 second intervals)
- Multiple game objects working together
- System font loading (platform-specific)

## Font Note
The FPS counter tries to load a system font:
- **Windows**: C:/Windows/Fonts/arial.ttf
- **Mac**: /System/Library/Fonts/Helvetica.ttc
- **Linux**: /usr/share/fonts/truetype/dejavu/DejaVuSans.ttf

If it fails, you'll see a warning but the program will still run (just no FPS text).

## To Compile (example)
```bash
g++ main.cpp BaseRunner.cpp GameObjectManager.cpp AGameObject.cpp \
    SimpleCircle.cpp SimpleFPSCounter.cpp \
    -o minimal_v3 -lsfml-graphics -lsfml-window -lsfml-system
```

## Things to Try
1. Add more circles and see if FPS drops
2. Make circles bigger/smaller
3. Change text position and color
4. Add object count to the display

## Next Step
Minimal v4 will add textures and sprites - getting closer to the original!
