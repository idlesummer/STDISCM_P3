# Minimal v9: Optimizations

## 🎯 What This Does
Polishes v8 with optimizations and refinements. This is essentially v8 with better performance and cleaner code.

Changes from v8:
- Better batch size calculation
- Improved console output
- Performance optimizations
- Code cleanup

**Note:** v9 is mostly v8 with polish. The core architecture is identical.

## 📚 To Compile
```bash
g++ main.cpp BaseRunner.cpp GameObjectManager.cpp AGameObject.cpp \
    TextureManager.cpp BGObject.cpp SimpleFPSCounter.cpp StringUtils.cpp \
    IconObject.cpp TextureDisplay.cpp \
    IETThread.cpp StreamingThread.cpp ThreadPool.cpp \
    -o minimal_v9 -lsfml-graphics -lsfml-window -lsfml-system -pthread
```

## ➡️ Next: v10 - Full Original!
The complete codebase from the bundle!
