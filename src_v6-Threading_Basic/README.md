# Minimal v6: Threading Basics

## 🎯 What This Does
Introduces **threading** for asynchronous texture loading! Textures now load on **separate threads** while the game continues running smoothly.

This demonstrates:
- IETThread (abstract thread class)
- StreamingThread (worker thread for loading)
- Asynchronous loading (no freezing!)
- Thread lifecycle management

---

## 📁 New Files in v6

- **IETThread.h / IETThread.cpp** - Abstract thread base class
- **StreamingThread.h / StreamingThread.cpp** - Worker thread for loading
- **TextureDisplay.cpp** (UPDATED) - Now uses threading!

---

## 🆕 What's New?

### **1. IETThread - Abstract Thread Class**

```cpp
class IETThread {
public:
    void start();              // Start thread execution
    static void sleep(int ms); // Sleep helper
    
protected:
    virtual void run() = 0;    // Override with thread behavior
};
```

**Implementation:**
```cpp
void IETThread::start() {
    std::thread(&IETThread::run, this).detach();
    // Creates thread, runs run() method, detaches
}
```

**Usage:**
```cpp
class MyThread : public IETThread {
protected:
    void run() override {
        // This code runs on separate thread!
        doWork();
    }
};

MyThread* thread = new MyThread();
thread->start();  // Runs on separate thread!
```

---

### **2. StreamingThread - Worker Thread**

```cpp
class StreamingThread : public IETThread {
    int textureIndex;
    std::atomic<bool> finished{false};
    
protected:
    void run() override {
        TextureManager::getInstance()->loadSingleStreamAsset(textureIndex);
        finished = true;
    }
};
```

**What it does:**
- Loads ONE texture on separate thread
- Uses `std::atomic<bool>` for thread-safe flag
- Main thread can poll `isFinished()`

---

### **3. Updated TextureDisplay**

**Before (v5):**
```cpp
void TextureDisplay::update() {
    if (ticks >= 1000) {
        // Blocks main thread!
        TextureManager::loadSingleStreamAsset(index);
        spawnObject();
    }
}
```

**After (v6):**
```cpp
void TextureDisplay::update() {
    // Start new load if no active thread
    if (ticks >= 1000 && currentStreamThread == nullptr) {
        currentStreamThread = new StreamingThread(index);
        currentStreamThread->start();  // Returns immediately!
    }

    // Check if worker finished
    if (currentStreamThread && currentStreamThread->isFinished()) {
        spawnObject();
        delete currentStreamThread;
        currentStreamThread = nullptr;
    }
}
```

---

## 🧵 Threading Explained

### **What is a Thread?**

```
Main Thread (Game Loop):      Worker Thread:
  Update game                   (waiting)
    ↓                              ↓
  Draw graphics                 Loading texture...
    ↓                              ↓
  Update game                   Still loading...
    ↓                              ↓
  Draw graphics                 Done!
    ↓
  Spawn icon (use loaded texture)
```

**Key point:** Both threads run **simultaneously**!

---

### **Timeline Example:**

```
Tick 60:   Main: Start worker thread
           Worker: Begin loading texture 0
           
Tick 61:   Main: Update, draw (smooth!)
           Worker: Loading... (50% done)
           
Tick 62:   Main: Update, draw (smooth!)
           Worker: Loading... (100% done)
           
Tick 63:   Main: Check isFinished() → YES!
                  Spawn icon
           Worker: (finished, cleaned up)
```

---

## 🎯 Key Concepts

### **1. Detached Thread**

```cpp
std::thread(...).detach();
```

**What detach does:**
- Thread runs independently
- Main thread doesn't wait
- Thread cleans up automatically when done
- Cannot join() later

**Why we use it:**
- Main thread continues immediately
- No blocking
- Game stays smooth

---

### **2. std::atomic**

```cpp
std::atomic<bool> finished{false};
```

**Why atomic?**

**Bad (race condition):**
```cpp
bool finished = false;

// Worker writes:
finished = true;

// Main reads:
if (finished) ...

// Both access simultaneously → UNDEFINED BEHAVIOR!
```

**Good (thread-safe):**
```cpp
std::atomic<bool> finished{false};

// Worker writes:
finished = true;  // Atomic write

// Main reads:
if (finished.load()) ...  // Atomic read

// Safe! No race condition
```

---

### **3. Thread Lifecycle**

```
1. Create
   StreamingThread* t = new StreamingThread(0);

2. Start
   t->start();  // Spawns new thread
   
3. Run (on separate thread)
   run() executes
   
4. Check completion
   if (t->isFinished()) ...
   
5. Cleanup
   delete t;
```

---

## 📊 v5 vs v6 Comparison

| Aspect | v5 (Synchronous) | v6 (Asynchronous) |
|--------|------------------|-------------------|
| **Loading** | Main thread | Worker thread |
| **Blocking** | Yes (freezes) | No (smooth) |
| **FPS during load** | Drops | Stays 60 |
| **Complexity** | Simple | More complex |
| **Real-world** | Bad for games | Industry standard |

---

## 🎮 What You'll See

### **Console Output:**
```
[TextureDisplay] Starting thread to load texture 0
[StreamingThread] Loading texture 0 on thread 140234567890
[StreamingThread] Finished loading texture 0
[TextureDisplay] Thread finished! Spawning icon...
Spawned Icon_0 at position (0, 0)

[TextureDisplay] Starting thread to load texture 1
[StreamingThread] Loading texture 1 on thread 140234567891
```

**Notice the different thread IDs!** Each worker runs on its own thread.

---

## 💡 Real-World Applications

### **Games:**
- Loading assets without freezing
- Background music streaming
- AI pathfinding calculations
- Network requests

### **Other Software:**
- File downloads (browser)
- Image processing
- Video encoding
- Database queries

**Common pattern:** Do slow work on separate thread while UI stays responsive!

---

## 🐛 Common Issues

### **Issue: Deleting Active Thread**
```cpp
// BAD:
delete currentStreamThread;  // Still running!
```

**Solution:**
```cpp
// GOOD:
if (currentStreamThread && currentStreamThread->isFinished())
    delete currentStreamThread;
```

---

### **Issue: Memory Leak**
```cpp
// BAD:
currentStreamThread = new StreamingThread(1);  // Overwrites pointer!
// Old thread never deleted!
```

**Solution:**
```cpp
// GOOD:
if (currentStreamThread == nullptr) {
    currentStreamThread = new StreamingThread(1);
}
```

---

## ✅ What You Learned

✅ **Threading basics** - Separate execution paths  
✅ **IETThread** - Abstract thread class pattern  
✅ **std::thread** - C++ threading primitives  
✅ **detach()** - Fire-and-forget threads  
✅ **std::atomic** - Thread-safe variables  
✅ **Asynchronous loading** - Non-blocking I/O  

---

## ➡️ Next: v7 - Thread Synchronization

v6 handles **one thread at a time**. But what if we want **multiple threads** loading simultaneously?

v7 will introduce:
- **Mutex** - Lock shared data
- **std::lock_guard** - RAII locking
- **Queue protection** - Safe multi-access
- **Race condition prevention**

Ready for advanced threading? 🔒🚀

---

## 📚 To Compile

```bash
g++ main.cpp BaseRunner.cpp GameObjectManager.cpp AGameObject.cpp \
    TextureManager.cpp BGObject.cpp SimpleFPSCounter.cpp StringUtils.cpp \
    IconObject.cpp TextureDisplay.cpp \
    IETThread.cpp StreamingThread.cpp \
    -o minimal_v6 -lsfml-graphics -lsfml-window -lsfml-system -pthread
```

**Important:** Add `-pthread` flag for threading support!
