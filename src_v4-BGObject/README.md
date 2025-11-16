# Minimal v4: Scrolling Background + TextureManager

## 🎯 What This Does
Introduces **TextureManager** (resource management singleton) and **BGObject** (scrolling background using textures)!

This is the REAL progression from the original codebase!

---

## 📁 Files Needed
- main.cpp
- BaseRunner.h / BaseRunner.cpp
- GameObjectManager.h / GameObjectManager.cpp
- **AGameObject.h / AGameObject.cpp (UPDATED - full sprite support!)**
- **TextureManager.h / TextureManager.cpp (NEW - Singleton!)**
- **BGObject.h / BGObject.cpp (NEW - from original code!)**
- SimpleFPSCounter.h / SimpleFPSCounter.cpp

---

## 🖼️ **IMPORTANT: You Need a Background Image!**

The code expects a file called **`desert.png`** in the same directory as your executable.

### Quick Options:

**Option 1: Download a desert/sand texture**
- Google "desert texture seamless"
- https://www.pexels.com (search "sand texture")
- https://opengameart.org (search "desert background")
- Save as `desert.png`

**Option 2: Use ANY image you have**
Just rename it to `desert.png` - it can be anything! The original uses a desert theme.

**Option 3: Create a simple gradient**
Use Paint/Photoshop to create:
- Size: 1920x1080 pixels
- Content: Brown/tan gradient or solid color
- Save as `desert.png`

**Option 4: Change the code**
In `BaseRunner.cpp`, change the filename:
```cpp
TextureManager::getInstance()->loadTexture("Desert", "your_image.png");
```

---

## 🆕 What's New in v4?

### 1. **TextureManager (Singleton Pattern)**

A centralized manager for ALL textures in the game!

```cpp
class TextureManager {
    static TextureManager* getInstance();  // Singleton accessor
    void loadTexture(String name, String path);
    sf::Texture* getFromTextureMap(String name, int frameIndex);
};
```

**Why?**
- Load each texture only ONCE
- Share textures between multiple sprites
- Centralized resource management
- Easy to track what's loaded

**Usage:**
```cpp
// Load texture (do once at startup)
TextureManager::getInstance()->loadTexture("Desert", "desert.png");

// Get texture (use anywhere, anytime)
sf::Texture* tex = TextureManager::getInstance()->getFromTextureMap("Desert", 0);
```

---

### 2. **BGObject (Scrolling Background)**

Creates an infinite scrolling background effect!

**How it works:**

#### Step 1: Texture Repeating
```cpp
texture->setRepeated(true);  // Enable texture wrapping
```
Allows the texture to tile/repeat infinitely.

#### Step 2: Large Texture Rectangle
```cpp
sprite->setTextureRect(sf::IntRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT * 8));
```
Creates a rectangle **8 screens tall** using the repeated texture.

#### Step 3: Start Position
```cpp
setPosition(0, -WINDOW_HEIGHT * 7);
```
Starts 7 screens UP (so we have room to scroll down).

#### Step 4: Scroll Down
```cpp
position.y += SPEED_MULTIPLIER * deltaTime.asSeconds();
```
Moves downward at 100 pixels/second.

#### Step 5: Reset When Done
```cpp
if (position.y >= 0) {
    setPosition(0, -WINDOW_HEIGHT * 7);  // Jump back to top
}
```
When we've scrolled all the way down, reset! Creates infinite loop.

**Visual Diagram:**
```
Start:
┌─────────┐ ← position.y = -7560 (7 screens up)
│ Texture │
│ Texture │
│ Texture │
│ Texture │
│ Texture │
│ Texture │
│ Texture │
├─────────┤ ← Screen viewport (what you see)
│ VISIBLE │
└─────────┘

Scrolling... position.y increases...

End:
├─────────┤ ← Screen viewport
│ VISIBLE │
└─────────┘ ← position.y = 0
│ Texture │
│ Texture │
│ Texture │
│ Texture │
│ Texture │
│ Texture │
└─────────┘

Then RESET to start position! → Infinite scroll!
```

---

### 3. **Full AGameObject with Sprite Support**

Now includes:
```cpp
protected:
    sf::Sprite* sprite = nullptr;
    sf::Texture* texture = nullptr;
    float posX, posY;        // Position tracking
    float scaleX, scaleY;    // Scale tracking
```

Plus helper methods:
- `setPosition(x, y)` - Move object
- `setScale(x, y)` - Resize object
- `getPosition()` - Get current position
- `getScale()` - Get current scale
- `getLocalBounds()` - Get sprite dimensions

---

## 🔑 Key Concepts

### Singleton Pattern (TextureManager)

**Problem:** What if 100 objects all try to load "player.png"?
- Wastes memory (100 copies of same image!)
- Wastes time (loading same file 100 times)
- Hard to manage

**Solution:** Singleton!
```cpp
// Everyone shares THE SAME instance
TextureManager* tm1 = TextureManager::getInstance();
TextureManager* tm2 = TextureManager::getInstance();
// tm1 == tm2 (same object!)

// Load once
tm1->loadTexture("Player", "player.png");

// Use everywhere
sf::Texture* tex = tm2->getFromTextureMap("Player", 0);  // Gets same texture!
```

**How it works:**
```cpp
static TextureManager* sharedInstance = NULL;

TextureManager* getInstance() {
    if (sharedInstance == NULL) {
        sharedInstance = new TextureManager();  // Create once
    }
    return sharedInstance;  // Always return same instance
}
```

**Private constructor prevents:**
```cpp
TextureManager* tm = new TextureManager();  // ERROR! Can't do this!
```

---

### Texture Repeating (setRepeated)

**Without repeating:**
```
┌──────┐
│ IMG  │ ← Original texture
└──────┘
┌──────┐
│ IMG  │ ← If you make rect bigger, it stretches (blurry!)
│      │
└──────┘
```

**With repeating:**
```
┌──────┐
│ IMG  │ ← Original texture
└──────┘
┌──────┐
│ IMG  │ ← If you make rect bigger, it tiles (sharp!)
│ IMG  │
└──────┘
```

Perfect for backgrounds that need to scroll infinitely!

---

### Hash Map Storage

```cpp
std::unordered_map<String, TextureList> textureMap;
```

Structure:
```
textureMap:
  "Desert" → [texture1]
  "Player" → [texture1, texture2, texture3]  ← Animation frames!
  "Enemy"  → [texture1]
```

Fast lookup: O(1) to find texture by name!

---

## 💡 Experiments to Try

### 1. **Change Scroll Speed**
```cpp
// In BGObject.h:
const float SPEED_MULTIPLIER = 200.0f;  // Faster!
const float SPEED_MULTIPLIER = 50.0f;   // Slower!
const float SPEED_MULTIPLIER = -100.0f; // Scroll UP!
```

### 2. **Load Multiple Textures**
```cpp
// In BaseRunner constructor:
TextureManager::getInstance()->loadTexture("Desert", "desert.png");
TextureManager::getInstance()->loadTexture("Sky", "sky.png");
TextureManager::getInstance()->loadTexture("Stars", "stars.png");
```

### 3. **Add More Background Layers** (Parallax!)
```cpp
BGObject* bgSky = new BGObject("Sky");
GameObjectManager::getInstance()->addObject(bgSky);

BGObject* bgGround = new BGObject("Ground");
GameObjectManager::getInstance()->addObject(bgGround);
```
Make them scroll at different speeds for depth!

### 4. **Scale the Background**
```cpp
void BGObject::initialize() {
    // ... after sprite setup ...
    this->setScale(2.0f, 2.0f);  // 2x bigger
}
```

---

## 🐛 Troubleshooting

### Black screen?
1. Check `desert.png` exists in executable directory
2. Try absolute path: `"C:/path/to/desert.png"`
3. Check console for error messages
4. Make sure image is valid (open in image viewer)

### Background not moving?
1. Check `SPEED_MULTIPLIER` isn't 0
2. Check FPS is running at ~60
3. Look for errors in console

### Background "jumps" instead of smooth?
This is normal! When `position.y >= 0`, it resets. The texture is repeated, so it should be seamless if your image tiles well.

---

## 📊 Architecture Comparison

| v3 | v4 |
|----|---|---|
| SimpleCircle (shapes) | BGObject (texture) |
| No resource management | TextureManager singleton |
| No texture loading | Texture loading from files |
| Basic AGameObject | Full sprite-capable AGameObject |

---

## 🔍 Code Flow

```
1. main() → BaseRunner()
   ↓
2. TextureManager loads "desert.png"
   ↓
3. BGObject created
   ↓
4. BGObject.initialize()
   → Gets texture from TextureManager
   → Creates sprite with repeating texture
   → Sets up scrolling rectangle
   ↓
5. Game loop (60 FPS):
   → BGObject.update() moves position down
   → When reaches bottom, resets to top
   → BGObject.draw() renders sprite
```

---

## 📚 To Compile

```bash
g++ main.cpp BaseRunner.cpp GameObjectManager.cpp AGameObject.cpp \
    TextureManager.cpp BGObject.cpp SimpleFPSCounter.cpp \
    -o minimal_v4 -lsfml-graphics -lsfml-window -lsfml-system
```

---

## ➡️ Next: v5 - IconObject + Streaming Setup

In v5, we'll add:
- **IconObject** - Display individual texture icons
- **TextureDisplay** - Coordinator for spawning icons
- Grid layout system
- Prepare for streaming in v6!

Then v6 introduces **threading**! 🧵

---

## 🎯 What You Learned

✅ **Singleton pattern** - One shared instance  
✅ **Resource management** - TextureManager  
✅ **Texture repeating** - Infinite tiling  
✅ **Scrolling effects** - Moving backgrounds  
✅ **Hash map storage** - Fast texture lookup  
✅ **Full sprite system** - Position, scale, bounds  

You're now using the REAL classes from the original codebase! 🚀
