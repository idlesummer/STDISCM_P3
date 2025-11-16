# Minimal v5: IconObject + Grid Layout

## 🎯 What This Does
Introduces **IconObject** and **TextureDisplay** to create a grid of icons that appear progressively as textures load!

This demonstrates:
- Grid layout system (28 columns × 22 rows)
- Progressive texture loading (one per second)
- Coordinator pattern (TextureDisplay manages IconObjects)
- Stream texture usage

---

## 📁 Files

### From v4.5:
- main.cpp
- BaseRunner.h / BaseRunner.cpp (UPDATED)
- GameObjectManager.h / GameObjectManager.cpp
- AGameObject.h / AGameObject.cpp
- BGObject.h / BGObject.cpp
- TextureManager.h / TextureManager.cpp
- StringUtils.h / StringUtils.cpp
- SimpleFPSCounter.h / SimpleFPSCounter.cpp

### New in v5:
- **IconObject.h / IconObject.cpp** (NEW!)
- **TextureDisplay.h / TextureDisplay.cpp** (NEW!)

---

## 🆕 What's New in v5?

### **1. IconObject - Single Icon Display**

```cpp
class IconObject : public AGameObject
{
    int textureIndex;  // Index into streamTextureList
};
```

**What it does:**
- Displays ONE icon from the streaming texture list
- Gets texture by index (not name!)
- Static (doesn't move or update)

**Implementation:**
```cpp
void IconObject::initialize()
{
    // Get texture from stream list by index
    sf::Texture* texture = TextureManager::getInstance()
                              ->getStreamTextureFromList(this->textureIndex);
    
    // Create sprite
    this->sprite = new sf::Sprite();
    this->sprite->setTexture(*texture);
}
```

**Key point:** Uses `getStreamTextureFromList(index)` not `getFromTextureMap(name)`!

---

### **2. TextureDisplay - Grid Coordinator**

```cpp
class TextureDisplay : public AGameObject
{
    std::vector<IconObject*> iconList;  // Track spawned icons
    int columnGrid, rowGrid;             // Current grid position
    float ticks;                         // Timer
};
```

**What it does:**
- Loads streaming textures progressively (one per second)
- Spawns IconObjects to display them
- Arranges icons in a grid layout
- Manages the entire streaming process

**Flow:**
```
Every second:
  1. Load next streaming texture
  2. Create IconObject with that texture index
  3. Position it in the grid
  4. Add to GameObjectManager
  5. Icon appears on screen!
```

---

## 🎨 Grid Layout System

### **Grid Dimensions:**
```cpp
const int MAX_COLUMN = 28;   // 28 icons per row
const int MAX_ROW = 22;      // 22 rows max
const int IMG_WIDTH = 68;    // Each icon is 68 pixels wide
const int IMG_HEIGHT = 68;   // Each icon is 68 pixels tall
```

### **How Grid Position Works:**
```cpp
void TextureDisplay::spawnObject()
{
    // Calculate position
    float x = columnGrid * IMG_WIDTH;   // 0, 68, 136, 204...
    float y = rowGrid * IMG_HEIGHT;     // 0, 0, 0, 0...
    
    iconObj->setPosition(x, y);
    
    // Move to next cell
    columnGrid++;
    if (columnGrid >= MAX_COLUMN)  // Reached end of row?
    {
        columnGrid = 0;  // Reset to column 0
        rowGrid++;       // Move to next row
    }
}
```

### **Visual Grid:**
```
Screen (1920×1080):

┌─────────────────────────────────────────┐
│ Icon0  Icon1  Icon2  Icon3  ... Icon27 │ ← Row 0
│ (0,0)  (68,0) (136,0)          (1836,0)│
│                                         │
│ Icon28 Icon29 Icon30 ...      Icon55  │ ← Row 1
│ (0,68) (68,68)                (1836,68)│
│                                         │
│ Icon56 Icon57 ...                      │ ← Row 2
│ (0,136)                                │
│ ...                                    │
└─────────────────────────────────────────┘

Grid capacity: 28 × 22 = 616 icons total
```

---

## 🔄 Progressive Loading Flow

### **The Timeline:**
```
Time 0s (Startup):
  ├─ BaseRunner constructor runs
  ├─ Load base texture (desert.png)
  ├─ Create BGObject (background)
  ├─ Create TextureDisplay
  ├─ Create FPSCounter
  └─ Game loop starts

Time 1s (Gameplay):
  ├─ TextureDisplay.update() triggers
  ├─ Load streaming texture at index 0
  ├─ Spawn IconObject(0)
  ├─ Position at (0, 0)
  └─ Icon appears on screen!

Time 2s:
  ├─ Load streaming texture at index 1
  ├─ Spawn IconObject(1)
  ├─ Position at (68, 0)
  └─ Second icon appears!

Time 3s:
  ├─ Load streaming texture at index 2
  ├─ Spawn IconObject(2)
  ├─ Position at (136, 0)
  └─ Third icon appears!

...continues until 50 icons loaded
```

---

## 🔍 Code Flow Explanation

### **update() Method:**
```cpp
void TextureDisplay::update(sf::Time deltaTime)
{
    // Accumulate time
    this->ticks += BaseRunner::TIME_PER_FRAME.asMilliseconds();  // Add ~16.6ms

    // Every 1000ms (1 second)
    if (this->ticks >= this->STREAMING_LOAD_DELAY)
    {
        this->ticks = 0.0f;  // Reset timer

        int currentIndex = this->iconList.size();  // How many icons so far?
        
        if (currentIndex < MAX_ICONS)  // Still have more to load?
        {
            // Load one texture
            TextureManager::getInstance()->loadSingleStreamAsset(currentIndex);
            
            // Spawn icon to display it
            this->spawnObject();
        }
    }
}
```

### **spawnObject() Method:**
```cpp
void TextureDisplay::spawnObject()
{
    // Create icon with texture index
    int index = this->iconList.size();  // 0, 1, 2, 3...
    IconObject* iconObj = new IconObject("Icon_" + std::to_string(index), index);
    this->iconList.push_back(iconObj);  // Track it

    // Calculate grid position
    float x = this->columnGrid * IMG_WIDTH;   // Column position
    float y = this->rowGrid * IMG_HEIGHT;     // Row position
    iconObj->setPosition(x, y);

    // Move to next grid cell
    this->columnGrid++;
    if (this->columnGrid >= this->MAX_COLUMN) {
        this->columnGrid = 0;
        this->rowGrid++;
    }
    
    // Add to game (calls initialize, starts updating/drawing)
    GameObjectManager::getInstance()->addObject(iconObj);
}
```

---

## 🖼️ Setting Up Icon Images

### **Directory Structure:**
```
YourProject/
├── Media/
│   ├── desert.png          ← Background texture
│   └── Streaming/          ← Icon folder
│       ├── icon01.png      ← 68×68 pixels
│       ├── icon02.png      ← 68×68 pixels
│       ├── icon03.png      ← 68×68 pixels
│       └── ...
├── main.cpp
├── BaseRunner.cpp
└── ...
```

### **Icon Requirements:**
- **Size:** 68×68 pixels (recommended)
- **Format:** PNG, JPG, BMP (SFML supported formats)
- **Naming:** Any name (loaded by directory order)
- **Quantity:** This demo loads up to 50 icons

### **Where to Get Icons:**
1. **Flaticon.com** - Free icon sets
2. **Icons8.com** - Thousands of free icons
3. **OpenGameArt.org** - Game assets
4. **Create your own** - Use any image editor

### **Quick Test Setup:**
```bash
# Create directories
mkdir -p Media/Streaming

# Copy any 68×68 images to Media/Streaming/
# Or create simple colored squares for testing
```

---

## 💡 Key Concepts

### **1. Coordinator Pattern**

**TextureDisplay** is a coordinator:
```
TextureDisplay (Coordinator)
    ├─ Manages loading timing
    ├─ Creates IconObjects
    ├─ Positions them in grid
    └─ Tracks all spawned icons

IconObject (Managed Entity)
    └─ Just displays one texture
```

**Benefits:**
- IconObject is simple (single responsibility)
- TextureDisplay handles complexity
- Easy to modify grid layout
- Can track all icons in one place

---

### **2. Stream Texture Index**

**Why index instead of name?**
```cpp
// Icons don't care about names:
IconObject* icon = new IconObject("Icon_5", 5);
//                                          ↑
//                                   Just needs index

// Gets texture by index:
sf::Texture* tex = tm->getStreamTextureFromList(5);
```

**Advantages:**
- Simpler - no name management
- Sequential - 0, 1, 2, 3...
- Works with any filenames
- Perfect for progressive loading

---

### **3. Progressive Loading**

**Instead of:**
```cpp
// Load all at startup (freezes game)
for (int i = 0; i < 480; i++) {
    loadTexture(i);
}
// User waits...
```

**We do:**
```cpp
// Load one at a time during gameplay
if (timer >= 1.0f) {
    loadTexture(nextIndex);
    nextIndex++;
    // Game continues running!
}
```

---

## 📊 What You'll See

```
┌────────────────────────────────────────┐
│ FPS: 60                                │
│                                        │
│ 🖼️ 🖼️ 🖼️ 🖼️ 🖼️ 🖼️ ⬜ ⬜ ⬜ ... │ ← Icons appearing
│                                        │
│ (Background scrolling)                 │
│                                        │
│ Console:                               │
│ [TextureDisplay] Loading texture 0     │
│ Spawned Icon_0 at (0, 0)              │
│ Icons spawned: 1/50                    │
│                                        │
│ [TextureDisplay] Loading texture 1     │
│ Spawned Icon_1 at (68, 0)             │
│ Icons spawned: 2/50                    │
└────────────────────────────────────────┘
```

---

## 🐛 Troubleshooting

### **No icons appearing?**

**Check 1:** Media/Streaming/ directory exists?
```bash
ls Media/Streaming/
# Should show your icon images
```

**Check 2:** Images are valid?
```bash
# Try opening them in an image viewer
```

**Check 3:** Console output?
```
[TextureManager] Number of streaming assets: 0  ← Problem! No files found
```

**Check 4:** Image permissions?
- Make sure files are readable
- Try absolute paths if relative doesn't work

---

### **Icons in wrong positions?**

**Check:** IMG_WIDTH and IMG_HEIGHT match your images
```cpp
// If your icons are 64×64 instead of 68×68:
const int IMG_WIDTH = 64;
const int IMG_HEIGHT = 64;
```

---

### **Loading too slow/fast?**

**Adjust:** STREAMING_LOAD_DELAY
```cpp
const float STREAMING_LOAD_DELAY = 500.0f;   // Load every 0.5 seconds (faster)
const float STREAMING_LOAD_DELAY = 2000.0f;  // Load every 2 seconds (slower)
```

---

## 💡 Experiments to Try

### **1. Change Grid Size**
```cpp
const int MAX_COLUMN = 20;  // Wider icons
const int IMG_WIDTH = 96;   // Larger spacing
```

### **2. Load More Icons**
```cpp
const int MAX_ICONS = 100;  // Load 100 instead of 50
```

### **3. Add Animation**
```cpp
void IconObject::update(sf::Time deltaTime) {
    // Pulse effect
    static float time = 0;
    time += deltaTime.asSeconds();
    float scale = 1.0f + 0.1f * sin(time * 5.0f);
    this->setScale(scale, scale);
}
```

### **4. Random Positions**
```cpp
void TextureDisplay::spawnObject() {
    float x = rand() % 1920;
    float y = rand() % 1080;
    iconObj->setPosition(x, y);
}
```

### **5. Load in Batches**
```cpp
void TextureDisplay::update(sf::Time deltaTime) {
    if (ticks >= STREAMING_LOAD_DELAY) {
        // Load 5 at once instead of 1
        for (int i = 0; i < 5; i++) {
            int index = iconList.size();
            if (index < MAX_ICONS) {
                tm->loadSingleStreamAsset(index);
                spawnObject();
            }
        }
    }
}
```

---

## 📚 To Compile

```bash
g++ main.cpp BaseRunner.cpp GameObjectManager.cpp AGameObject.cpp \
    TextureManager.cpp BGObject.cpp SimpleFPSCounter.cpp StringUtils.cpp \
    IconObject.cpp TextureDisplay.cpp \
    -o minimal_v5 -lsfml-graphics -lsfml-window -lsfml-system
```

---

## 🎯 What You Learned

✅ **IconObject** - Simple display entity  
✅ **TextureDisplay** - Coordinator pattern  
✅ **Grid layout** - Positioning system  
✅ **Progressive loading** - Load during gameplay  
✅ **Stream texture usage** - Access by index  
✅ **Timer-based spawning** - Delayed actions  

---

## ➡️ Next: v6 - Threading!

In v6, we'll add:
- **IETThread** - Abstract thread class
- **StreamingThread** - Load textures on separate thread
- **Asynchronous loading** - No more freezing!

The icons will load in the **background** without blocking the game loop!

Ready to tackle threading? 🧵🚀
