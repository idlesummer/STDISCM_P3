# Minimal v4.5: Complete TextureManager (Non-Async)

## 🎯 What This Does
Completes the TextureManager with ALL non-threading methods from the original codebase!

This adds important features like:
- Loading from asset lists (batch loading)
- Stream texture tracking
- String utility helpers
- Proper error handling

---

## 📁 New Files

### From v4:
- main.cpp
- BaseRunner.h / BaseRunner.cpp
- GameObjectManager.h / GameObjectManager.cpp
- AGameObject.h / AGameObject.cpp
- BGObject.h / BGObject.cpp
- SimpleFPSCounter.h / SimpleFPSCounter.cpp

### New in v4.5:
- **StringUtils.h / StringUtils.cpp** (NEW!)
- **TextureManager.h / TextureManager.cpp** (UPDATED with more methods!)

---

## 🆕 What's New in v4.5?

### **1. loadFromAssetList() - Batch Loading**

```cpp
void loadFromAssetList();
```

**What it does:**
- Reads a file called `Media/assets.txt`
- Each line contains a path to an image
- Loads all images automatically

**Example `Media/assets.txt`:**
```
Media/desert.png
Media/sky.png
Media/clouds.png
```

**Usage:**
```cpp
// Load all startup assets at once
TextureManager::getInstance()->loadFromAssetList();
```

**Implementation breakdown:**
```cpp
void TextureManager::loadFromAssetList()
{
    // 1. Open the asset list file
    std::ifstream stream("Media/assets.txt");
    
    // 2. Read each line (each line is a file path)
    String path;
    while (std::getline(stream, path))  // "Media/desert.png"
    {
        // 3. Parse the filename from the path
        //    "Media/desert.png" -> ["Media", "desert.png"]
        std::vector<String> tokens = StringUtils::split(path, '/');
        
        //    "desert.png" -> ["desert", "png"]
        String assetName = StringUtils::split(tokens[tokens.size() - 1], '.')[0];
        //    assetName = "desert"
        
        // 4. Load the texture
        this->instantiateAsTexture(path, assetName, false);
        //                                           ↑
        //                                  false = base texture (not streaming)
    }
}
```

**Why useful?**
- Load dozens of textures with one function call
- Easy to manage what gets loaded at startup
- Just edit a text file to add/remove assets

---

### **2. loadSingleStreamAsset(int index) - Stream Asset Loading**

```cpp
void loadSingleStreamAsset(int index);
```

**What it does:**
- Loads a single texture from the `Media/Streaming/` directory
- Uses directory index (not filename)
- Automatically parses filename to create asset name
- Marks texture as "streaming" (goes into streamTextureList)

**How it works:**
```cpp
void loadSingleStreamAsset(int index)
{
    int fileNum = 0;
    
    // Iterate through streaming directory
    for (const auto& entry : std::filesystem::directory_iterator(STREAMING_PATH))
    {
        if (index == fileNum)  // Found the file at this index
        {
            // Get full path: "Media/Streaming/icon01.png"
            String path = entry.path().string();
            
            // Parse filename: "icon01.png" -> "icon01"
            std::vector<String> pathTokens = StringUtils::split(path, '/');
            String filename = pathTokens[pathTokens.size() - 1];
            String assetName = StringUtils::split(filename, '.')[0];
            
            // Load as streaming texture
            this->instantiateAsTexture(path, assetName, true);
            //                                           ↑
            //                                    true = streaming texture
            break;
        }
        fileNum++;
    }
}
```

**Why index-based?**
- Don't need to know filenames in advance
- Can load incrementally: 0, 1, 2, 3...
- Supports progressive loading (one at a time)

**Usage:**
```cpp
// Load streaming assets one by one
TextureManager* tm = TextureManager::getInstance();

tm->loadSingleStreamAsset(0);  // Load first file in Media/Streaming/
tm->loadSingleStreamAsset(1);  // Load second file
tm->loadSingleStreamAsset(2);  // Load third file
// ...

// Or in a loop
for (int i = 0; i < 10; i++) {
    tm->loadSingleStreamAsset(i);
}
```

**Difference from loadTexture():**
```cpp
// loadTexture: You specify path and name
tm->loadTexture("Hero", "Media/hero.png");

// loadSingleStreamAsset: Automatic from directory
tm->loadSingleStreamAsset(0);  // Loads Media/Streaming/[first file]
//                                Auto-names it based on filename
```

---

### **3. getStreamTextureFromList() - Direct Index Access**

```cpp
sf::Texture* getStreamTextureFromList(const int index);
```

**What it does:**
- Gets a texture by its index in the stream list
- Used by IconObject to display streamed textures

**Difference from getFromTextureMap():**
```cpp
// By NAME (hash map lookup):
sf::Texture* tex = tm->getFromTextureMap("Desert", 0);  // Need to know name

// By INDEX (direct array access):
sf::Texture* tex = tm->getStreamTextureFromList(5);  // Get 6th loaded stream texture
```

**Usage:**
```cpp
// IconObject displays the Nth loaded texture
class IconObject {
    void initialize() {
        sf::Texture* tex = TextureManager::getInstance()
                              ->getStreamTextureFromList(this->textureIndex);
        sprite->setTexture(*tex);
    }
};
```

**Why two access methods?**
- **By name**: For known, named textures (backgrounds, characters)
- **By index**: For dynamically loaded textures (streaming system)

---

### **3. getNumLoadedStreamTextures() - Progress Tracking**

```cpp
int getNumLoadedStreamTextures() const;
```

**What it does:**
- Returns how many stream textures have been loaded so far
- Useful for progress bars or checking if loading is complete

**Usage:**
```cpp
int loaded = TextureManager::getInstance()->getNumLoadedStreamTextures();
int total = 480;  // Total textures to load

std::cout << "Progress: " << loaded << "/" << total << std::endl;

// Progress bar
float progress = (float)loaded / total * 100.0f;
std::cout << "Loading: " << progress << "%" << std::endl;
```

---

### **4. StringUtils - Path Parsing Helper**

```cpp
class StringUtils {
    static std::vector<std::string> split(const std::string& s, char delimiter);
};
```

**What it does:**
- Splits a string by a delimiter character
- Returns a vector of tokens

**Examples:**
```cpp
// Split by slash
std::vector<std::string> parts = StringUtils::split("Media/Streaming/icon01.png", '/');
// Result: ["Media", "Streaming", "icon01.png"]

// Split by dot
std::vector<std::string> parts = StringUtils::split("icon01.png", '.');
// Result: ["icon01", "png"]

// Get filename without extension
std::string filename = parts[0];  // "icon01"
```

**Why needed?**
```
Full path: "Media/Streaming/icon01.png"
           ↓ split by '/'
           ["Media", "Streaming", "icon01.png"]
           ↓ take last element
           "icon01.png"
           ↓ split by '.'
           ["icon01", "png"]
           ↓ take first element
           "icon01"  ← This becomes the asset name!
```

---

### **5. instantiateAsTexture() - Private Helper**

```cpp
void instantiateAsTexture(String path, String assetName, bool isStreaming);
```

**What it does:**
- Private helper method
- Actually loads the texture from disk
- Adds it to the appropriate lists

**Parameters:**
- `path`: File path ("Media/desert.png")
- `assetName`: Name to store it under ("Desert")
- `isStreaming`: Whether this is a base or stream texture

**Implementation:**
```cpp
void TextureManager::instantiateAsTexture(String path, String assetName, bool isStreaming)
{
    // 1. Create texture and load from file
    sf::Texture* texture = new sf::Texture();
    if (!texture->loadFromFile(path)) {
        std::cerr << "ERROR: Failed to load " << path << std::endl;
        delete texture;
        return;
    }
    
    // 2. Add to hash map (by name)
    this->textureMap[assetName].push_back(texture);
    
    // 3. Add to appropriate list (by type)
    if (isStreaming) {
        this->streamTextureList.push_back(texture);  // For IconObjects
    } else {
        this->baseTextureList.push_back(texture);    // For startup objects
    }
}
```

**Why separate lists?**
```
textureMap:           (Access by name)
  "Desert" → [texture*]
  "Player" → [texture*]
  
baseTextureList:      (Loaded at startup)
  [desert*, player*]
  
streamTextureList:    (Loaded progressively)
  [icon01*, icon02*, icon03*, ...]
```

---

### **6. countStreamingAssets() - Preparation**

```cpp
void countStreamingAssets();
```

**What it does:**
- Counts how many files are in `Media/Streaming/` directory
- Called in constructor
- Prepares for future streaming

**Implementation:**
```cpp
void TextureManager::countStreamingAssets()
{
    this->streamingAssetCount = 0;
    
    // Check if directory exists
    if (!std::filesystem::exists(STREAMING_PATH)) {
        std::cout << "Streaming path does not exist" << std::endl;
        return;
    }
    
    // Count files in directory
    for (const auto& entry : std::filesystem::directory_iterator(STREAMING_PATH)) {
        this->streamingAssetCount++;
    }
    
    std::cout << "Number of streaming assets: " << this->streamingAssetCount << std::endl;
}
```

**Why needed?**
- Know total count before loading starts
- Calculate progress percentage
- Validate streaming setup

---

## 📊 Three Data Structures Explained

### **1. textureMap (Hash Map)**
```cpp
std::unordered_map<String, TextureList> textureMap;
```
**Purpose:** Access textures by name  
**Structure:**
```
"Desert" → [texture*]
"Hero"   → [texture1*, texture2*, texture3*]  ← Animation frames
"Enemy"  → [texture*]
```

### **2. baseTextureList (Vector)**
```cpp
std::vector<sf::Texture*> baseTextureList;
```
**Purpose:** Track all startup/base textures  
**Structure:**
```
[desert*, hero*, enemy*, ...]
```
**Why?** Easy cleanup, memory tracking, debugging

### **3. streamTextureList (Vector)**
```cpp
std::vector<sf::Texture*> streamTextureList;
```
**Purpose:** Track dynamically loaded textures  
**Structure:**
```
[icon01*, icon02*, icon03*, ...]
```
**Why?** Access by index (IconObject uses index, not name)

---

## 🔍 Complete Method Reference

| Method | Purpose | Returns |
|--------|---------|---------|
| **getInstance()** | Get singleton instance | `TextureManager*` |
| **loadFromAssetList()** | Load all from Media/assets.txt | `void` |
| **loadTexture(name, path)** | Load single texture manually | `void` |
| **loadSingleStreamAsset(index)** | Load streaming asset by directory index | `void` |
| **getFromTextureMap(name, frame)** | Get texture by name | `sf::Texture*` |
| **getNumFrames(name)** | Count animation frames | `int` |
| **getStreamTextureFromList(index)** | Get stream texture by index | `sf::Texture*` |
| **getNumLoadedStreamTextures()** | Count loaded stream textures | `int` |
| **countStreamingAssets()** (private) | Count files in Media/Streaming/ | `void` |
| **instantiateAsTexture(...)** (private) | Actually load and store texture | `void` |

---

## 💡 Usage Examples

### Example 1: Batch Loading at Startup
```cpp
// Create Media/assets.txt:
// Media/background.png
// Media/player.png
// Media/enemy.png

void BaseRunner::BaseRunner() {
    // Load all at once
    TextureManager::getInstance()->loadFromAssetList();
    
    // Now all are available by name
    sf::Texture* bg = TextureManager::getInstance()->getFromTextureMap("background", 0);
    sf::Texture* player = TextureManager::getInstance()->getFromTextureMap("player", 0);
}
```

### Example 2: Manual Loading
```cpp
void BaseRunner::BaseRunner() {
    TextureManager* tm = TextureManager::getInstance();
    
    tm->loadTexture("Desert", "desert.png");
    tm->loadTexture("Sky", "sky.png");
    tm->loadTexture("Hero", "hero_frame1.png");
    tm->loadTexture("Hero", "hero_frame2.png");  // Animation frame 2
    
    // Get textures
    sf::Texture* desert = tm->getFromTextureMap("Desert", 0);
    sf::Texture* heroFrame1 = tm->getFromTextureMap("Hero", 0);
    sf::Texture* heroFrame2 = tm->getFromTextureMap("Hero", 1);
}
```

### Example 3: Progressive Streaming (Single Asset at a Time)
```cpp
// Load streaming assets one by one
TextureManager* tm = TextureManager::getInstance();

// Load first 10 streaming assets
for (int i = 0; i < 10; i++) {
    tm->loadSingleStreamAsset(i);
}

// Check how many loaded
int numLoaded = tm->getNumLoadedStreamTextures();
std::cout << "Loaded " << numLoaded << " streaming textures" << std::endl;

// Access them by index
for (int i = 0; i < numLoaded; i++) {
    sf::Texture* tex = tm->getStreamTextureFromList(i);
    // Create IconObject with this texture...
}
```

### Example 4: Stream Texture Access (for future IconObject)
```cpp
// After streaming loads some textures...
int numLoaded = TextureManager::getInstance()->getNumLoadedStreamTextures();

for (int i = 0; i < numLoaded; i++) {
    sf::Texture* tex = TextureManager::getInstance()->getStreamTextureFromList(i);
    // Create IconObject with this texture...
}
```

---

## 📚 To Compile

```bash
g++ main.cpp BaseRunner.cpp GameObjectManager.cpp AGameObject.cpp \
    TextureManager.cpp BGObject.cpp SimpleFPSCounter.cpp StringUtils.cpp \
    -o minimal_v4_5 -lsfml-graphics -lsfml-window -lsfml-system
```

---

## ➡️ What's Next?

You now have the COMPLETE non-async TextureManager!

**Next steps:**
- **v5**: Add IconObject + TextureDisplay (grid layout)
- **v6**: Add threading basics (IETThread)
- **v7**: Add async loading methods (the methods we skipped!)

---

## 🎯 What You Learned

✅ **Batch loading** - Load from asset lists  
✅ **Path parsing** - StringUtils for file handling  
✅ **Multiple access methods** - By name vs by index  
✅ **Progress tracking** - Count loaded textures  
✅ **Three storage patterns** - Hash map + 2 lists  
✅ **Private helpers** - Internal implementation details  

The TextureManager is now complete and ready for the streaming system! 🚀
