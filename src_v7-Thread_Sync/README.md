# Minimal v7: Thread Synchronization

## 🎯 What This Does
Adds **thread synchronization** using **mutexes** and **lock_guard** to safely share data between threads!

This demonstrates:
- std::mutex (mutual exclusion)
- std::lock_guard (RAII locking)
- Thread-safe queue (producer-consumer pattern)
- Race condition prevention

---

## 📁 New in v7

- **TextureManager.h** (UPDATED) - Added mutex and ready queue
- **TextureManager.cpp** (UPDATED) - Thread-safe queue operations
- **TextureDisplay.cpp** (UPDATED) - Consumes from ready queue

---

## 🆕 What's New?

### **1. Thread-Safe Ready Queue**

```cpp
class TextureManager {
private:
    std::queue<LoadedTexture> readyQueue;  // Shared data
    std::mutex queueMutex;                 // Protects queue
};
```

**The problem v7 solves:**
```cpp
// v6 (UNSAFE if multiple threads):
Worker Thread:     Main Thread:
readyQueue.push()  if (!readyQueue.empty())
                       readyQueue.pop()
    ↑ RACE CONDITION! Both accessing at same time!

// v7 (SAFE with mutex):
Worker Thread:           Main Thread:
lock mutex              lock mutex (waits...)
readyQueue.push()       
unlock mutex            acquired lock!
                        readyQueue.pop()
                        unlock mutex
    ✅ Only one thread accesses at a time!
```

---

### **2. std::mutex - Mutual Exclusion**

```cpp
std::mutex queueMutex;  // "Lock" for protecting data
```

**What it does:**
- Only ONE thread can hold the lock at a time
- Other threads must wait
- Prevents race conditions

**Visual:**
```
Thread 1: Acquire lock → Access data → Release lock
Thread 2: Wait...       Wait...        Acquire lock → Access data
Thread 3: Wait...       Wait...        Wait...        Wait...
```

---

### **3. std::lock_guard - RAII Locking**

```cpp
void addToReadyQueue(LoadedTexture loaded) {
    std::lock_guard<std::mutex> lock(queueMutex);  // Lock acquired
    readyQueue.push(loaded);
    // Lock released automatically when function ends
}
```

**RAII = Resource Acquisition Is Initialization**
- Lock acquired in constructor
- Lock released in destructor
- Automatic, exception-safe!

**vs Manual locking:**
```cpp
// Manual (BAD - can forget to unlock):
queueMutex.lock();
readyQueue.push(loaded);
queueMutex.unlock();  // What if exception before this?

// RAII (GOOD - automatic):
{
    std::lock_guard<std::mutex> lock(queueMutex);
    readyQueue.push(loaded);
}  // ← Automatically unlocks here!
```

---

### **4. Producer-Consumer Pattern**

```cpp
// PRODUCER (Worker Thread):
void StreamingThread::run() {
    loadTexture();
    TextureManager::getInstance()->addToReadyQueue(loaded);  // Produce
}

// CONSUMER (Main Thread):
void TextureDisplay::update() {
    while (TextureManager::getInstance()->hasReadyTexture()) {
        LoadedTexture loaded = TextureManager::getInstance()->popReadyTexture();  // Consume
        spawnIcon();
    }
}
```

---

## 🔒 Thread Synchronization Explained

### **The Problem: Race Condition**

**Without mutex:**
```cpp
std::queue<int> q;

// Thread 1:
q.push(5);  // Writing

// Thread 2 (same time):
int x = q.front();  // Reading
q.pop();

// CRASH! 💥 Both accessing simultaneously!
```

**What can go wrong:**
- Corrupted data
- Crashes
- Undefined behavior
- Intermittent bugs (hardest to debug!)

---

### **The Solution: Mutex**

**With mutex:**
```cpp
std::queue<int> q;
std::mutex m;

// Thread 1:
m.lock();      // Acquire lock
q.push(5);     // Safe!
m.unlock();    // Release lock

// Thread 2:
m.lock();      // Acquire lock (waits if Thread 1 has it)
int x = q.front();
q.pop();
m.unlock();

// ✅ Safe! Only one thread accesses at a time
```

---

## 🔍 Code Walkthrough

### **Worker Thread (Producer):**

```cpp
void StreamingThread::run() {
    // Load texture (slow)
    TextureManager::getInstance()->loadSingleStreamAsset(index);
    // ↓ Inside loadSingleStreamAsset():
    
    // Create loaded texture info
    LoadedTexture loaded;
    loaded.texture = texture;
    loaded.index = index;
    
    // Add to ready queue (THREAD-SAFE!)
    addToReadyQueue(loaded);
}

void TextureManager::addToReadyQueue(LoadedTexture loaded) {
    std::lock_guard<std::mutex> lock(queueMutex);  // Lock!
    readyQueue.push(loaded);
    // Lock released automatically
}
```

---

### **Main Thread (Consumer):**

```cpp
void TextureDisplay::update() {
    // ... start threads ...
    
    // Check ready queue
    while (TextureManager::getInstance()->hasReadyTexture()) {
        LoadedTexture loaded = TextureManager::getInstance()->popReadyTexture();
        spawnObject();
    }
}

bool TextureManager::hasReadyTexture() {
    std::lock_guard<std::mutex> lock(queueMutex);  // Lock!
    return !readyQueue.empty();
    // Lock released automatically
}

LoadedTexture TextureManager::popReadyTexture() {
    std::lock_guard<std::mutex> lock(queueMutex);  // Lock!
    LoadedTexture loaded = readyQueue.front();
    readyQueue.pop();
    return loaded;
    // Lock released automatically
}
```

---

## 📊 The Flow

```
Second 0:
  Main: Start Worker Thread 1
  Main: Continue update loop
  
  Worker 1: Loading texture...
  
Second 0.1:
  Worker 1: Done loading!
  Worker 1: addToReadyQueue(texture0)
            ↓
            LOCK mutex
            ↓
            readyQueue.push(texture0)
            ↓
            UNLOCK mutex
  
Second 0.116 (next tick):
  Main: hasReadyTexture()?
        ↓
        LOCK mutex (wait if worker has it)
        ↓
        Check !readyQueue.empty() → TRUE
        ↓
        UNLOCK mutex
        ↓
        YES! Has texture
  
  Main: popReadyTexture()
        ↓
        LOCK mutex
        ↓
        loaded = readyQueue.front()
        readyQueue.pop()
        ↓
        UNLOCK mutex
        ↓
        Return loaded
  
  Main: spawnObject()
        Icon appears!
```

---

## 🎯 Key Concepts

### **1. Critical Section**

**Code that accesses shared data:**
```cpp
// This is a critical section:
readyQueue.push(item);  // ← Multiple threads access readyQueue

// Must be protected:
{
    std::lock_guard<std::mutex> lock(mutex);
    readyQueue.push(item);  // ← Now safe!
}
```

---

### **2. Deadlock (Not in this code, but important)**

**Bad code that can deadlock:**
```cpp
std::mutex mutex1, mutex2;

// Thread 1:
mutex1.lock();
mutex2.lock();  // ← Waits for Thread 2 to release

// Thread 2:
mutex2.lock();
mutex1.lock();  // ← Waits for Thread 1 to release

// Both threads waiting forever! DEADLOCK! 💀
```

**Solution:** Always lock mutexes in same order

---

### **3. Lock Scope**

```cpp
void badExample() {
    std::lock_guard<std::mutex> lock(mutex);
    
    doWork();      // Critical section
    doMoreWork();  // Still locked (unnecessary!)
    evenMore();    // Still locked (slow!)
    
    // Lock held for entire function!
}

void goodExample() {
    {
        std::lock_guard<std::mutex> lock(mutex);
        doWork();  // Only lock what needs protection
    }  // ← Lock released here
    
    doMoreWork();  // Not locked (faster!)
    evenMore();    // Not locked
}
```

**Rule:** Lock for minimum time necessary!

---

## 🐛 Common Issues

### **Issue 1: Forgetting to Lock**

```cpp
// BAD:
void addItem(int item) {
    queue.push(item);  // No lock! Race condition!
}

// GOOD:
void addItem(int item) {
    std::lock_guard<std::mutex> lock(mutex);
    queue.push(item);
}
```

---

### **Issue 2: Locking Too Long**

```cpp
// BAD:
void process() {
    std::lock_guard<std::mutex> lock(mutex);
    
    readData();        // Needs lock
    processData();     // Doesn't need lock!
    sleep(1000);       // Definitely doesn't need lock!
    writeData();       // Needs lock
}

// GOOD:
void process() {
    Data d;
    {
        std::lock_guard<std::mutex> lock(mutex);
        d = readData();  // Lock
    }  // Release
    
    processData(d);  // No lock needed
    sleep(1000);
    
    {
        std::lock_guard<std::mutex> lock(mutex);
        writeData(d);  // Lock again
    }
}
```

---

### **Issue 3: Recursive Locking**

```cpp
// BAD:
void functionA() {
    std::lock_guard<std::mutex> lock(mutex);
    functionB();  // Calls function that also locks!
}

void functionB() {
    std::lock_guard<std::mutex> lock(mutex);  // DEADLOCK!
    // Can't acquire lock (already have it)
}

// Solution: Use std::recursive_mutex
// Or redesign to not nest locks
```

---

## 📊 v6 vs v7

| Aspect | v6 | v7 |
|--------|----|----|
| **Threading** | ✅ YES (1 thread) | ✅ YES (1 thread) |
| **Synchronization** | ❌ NO (not needed) | ✅ YES (mutex) |
| **Thread-safe queue** | ❌ NO | ✅ YES |
| **Ready for multiple threads** | ❌ NO | ✅ YES! |

**v7 prepares for v8** where multiple threads will load simultaneously!

---

## 💡 Why This Matters

**v6 worked fine with 1 thread, so why add complexity?**

**Answer:** Foundation for v8!

```
v6: One thread → No race conditions
v7: Still one thread, but adds synchronization
v8: MULTIPLE threads → Synchronization essential!
```

**v7 is the bridge:**
- Introduces concepts safely (still 1 thread)
- Tests synchronization mechanism
- Prepares codebase for parallelism

---

## ✅ What You Learned

✅ **std::mutex** - Mutual exclusion primitive  
✅ **std::lock_guard** - RAII locking pattern  
✅ **Critical sections** - Code needing protection  
✅ **Race conditions** - What can go wrong  
✅ **Producer-consumer** - Thread communication pattern  
✅ **Thread-safe queues** - Synchronized data structures  

---

## ➡️ Next: v8 - Thread Pool!

v7 has synchronization, but still only 1 thread at a time.

v8 will add:
- **ThreadPool** - Multiple worker threads
- **Task queue** - Work items waiting
- **Parallel loading** - 4+ textures simultaneously
- **True asynchronous** - No blocking icon pipeline!

Ready for thread pools? 🏊‍♂️🚀

---

## 📚 To Compile

```bash
g++ main.cpp BaseRunner.cpp GameObjectManager.cpp AGameObject.cpp \
    TextureManager.cpp BGObject.cpp SimpleFPSCounter.cpp StringUtils.cpp \
    IconObject.cpp TextureDisplay.cpp \
    IETThread.cpp StreamingThread.cpp \
    -o minimal_v7 -lsfml-graphics -lsfml-window -lsfml-system -pthread
```

**Note:** Same compile command as v6 (synchronization is in the code logic)
