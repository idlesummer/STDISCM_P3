# Promise Pattern: JavaScript vs C++ Threading

This document explains how JavaScript's Promise/async-await maps to C++ threading primitives.

## Core Concept Mapping

### JavaScript Promise Internals
```javascript
class Promise {
  [[PromiseState]]: "pending" | "fulfilled" | "rejected"
  [[PromiseResult]]: any
  [[PromiseFulfillReactions]]: Array<Function>
  [[PromiseRejectReactions]]: Array<Function>
}
```

### C++ Promise Implementation
```cpp
struct SharedState {
  State state;                    // [[PromiseState]]
  T value;                        // [[PromiseResult]]
  function<void(T)> onFulfilled;  // [[PromiseFulfillReactions]]
  function<void(ex)> onRejected;  // [[PromiseRejectReactions]]

  // Threading primitives for synchronization
  mutex mutex;                    // Protects state access
  condition_variable cv;          // Signals state changes
};
```

---

## Detailed Primitive Mapping

### 1. Promise State Management

#### JavaScript (Event Loop)
```javascript
let state = "pending";
let value = undefined;

function resolve(val) {
  if (state !== "pending") return;
  state = "fulfilled";
  value = val;
  // Event loop handles callbacks
}
```

#### C++ (Mutex + State)
```cpp
State state = PENDING;
T value;
mutex mtx;

void resolve(T val) {
  lock_guard<mutex> lock(mtx);  // Lock for thread safety
  if (state != PENDING) return;
  state = FULFILLED;
  value = val;
  cv.notify_all();  // Wake waiting threads
}
```

**Key difference:** JavaScript is single-threaded (no mutex needed), C++ is multi-threaded (requires mutex).

---

### 2. Waiting for Results

#### JavaScript (Event Loop)
```javascript
// Non-blocking - yields to event loop
const result = await promise;
```

#### C++ (Condition Variable)
```cpp
// Blocking - waits on condition variable
T await() {
  unique_lock<mutex> lock(mtx);
  cv.wait(lock, [this] {
    return state != PENDING;  // Wait until settled
  });
  return value;
}
```

**Mapping:**
- `await` → `cv.wait()` (blocks thread instead of yielding)
- Event loop → Condition variable notification
- Microtask queue → No direct equivalent (we use callbacks)

---

### 3. Chaining (.then)

#### JavaScript
```javascript
promise
  .then(value => value * 2)
  .then(doubled => console.log(doubled));
```

**How it works internally:**
1. Each `.then()` creates a new Promise
2. Previous promise stores callback in `[[PromiseFulfillReactions]]`
3. When resolved, event loop executes callbacks

#### C++
```cpp
promise
  ->then([](int value) { return value * 2; })
  ->then([](int doubled) { cout << doubled; });
```

**How it works:**
```cpp
template<typename Func>
auto then(Func func) -> shared_ptr<Promise<...>> {
  // Create new promise for chaining
  return Promise<ReturnType>::create([this, func](auto resolve, auto reject) {
    lock_guard lock(mtx);

    if (state == FULFILLED) {
      // Already resolved - call immediately
      resolve(func(value));
    } else {
      // Store callback to call later
      onFulfilled = [func, resolve, reject](T val) {
        resolve(func(val));
      };
    }
  });
}
```

**Key insight:** `.then()` always returns a NEW promise (in both JS and C++).

---

### 4. Promise.all()

#### JavaScript
```javascript
const results = await Promise.all([p1, p2, p3]);
```

**How it works:**
- Creates a new promise
- Counts pending promises
- When all resolve, resolves with array of results
- If any reject, immediately rejects

#### C++
```cpp
auto results = PromiseUtils::all({p1, p2, p3})->await();
```

**How it works:**
```cpp
template<typename T>
shared_ptr<Promise<vector<T>>> all(vector<shared_ptr<Promise<T>>> promises) {
  return Promise<vector<T>>::create([promises](auto resolve, auto reject) {
    thread([promises, resolve, reject]() {
      vector<T> results;
      for (auto& p : promises) {
        results.push_back(p->await());  // Wait for each
      }
      resolve(results);
    }).detach();
  });
}
```

**Difference:** C++ version blocks a thread, JavaScript uses event loop counters.

---

## Threading Primitive Reference

### Mutex (std::mutex)
**Purpose:** Mutual exclusion - only one thread can hold the lock
**JS Equivalent:** Not needed (single-threaded)

```cpp
mutex mtx;
lock_guard<mutex> lock(mtx);  // RAII lock
// Critical section
// Automatically unlocked when lock goes out of scope
```

**When to use:**
- Protecting shared state (promise value, state)
- Preventing race conditions

---

### Condition Variable (std::condition_variable)
**Purpose:** Thread signaling - wait for a condition to become true
**JS Equivalent:** Event loop + microtask queue

```cpp
condition_variable cv;
mutex mtx;

// Thread 1: Wait
unique_lock<mutex> lock(mtx);
cv.wait(lock, [&] { return ready; });  // Blocks until ready == true

// Thread 2: Notify
{
  lock_guard<mutex> lock(mtx);
  ready = true;
}
cv.notify_all();  // Wake all waiting threads
```

**How it maps to await:**
```javascript
// JavaScript
const value = await promise;
// Event loop checks promise state, yields if pending

// C++
T value = promise->await();
// cv.wait() blocks thread, wakes when notified
```

---

### Thread (std::thread)
**Purpose:** Execute code concurrently
**JS Equivalent:** Web Workers (or async functions on event loop)

```cpp
thread([resolve]() {
  // This runs in parallel
  auto result = doWork();
  resolve(result);
}).detach();  // Fire and forget
```

**JavaScript doesn't need threads** because:
- I/O is non-blocking
- Event loop handles concurrency
- Web Workers for true parallelism

**C++ needs threads** because:
- I/O is blocking by default
- No event loop (unless you build one)
- Threads for concurrency AND parallelism

---

## Complete Comparison Table

| Feature | JavaScript | C++ Primitives |
|---------|-----------|----------------|
| **Promise state** | `[[PromiseState]]` | `State state` + `mutex` |
| **Promise value** | `[[PromiseResult]]` | `T value` + `mutex` |
| **Resolve** | Internal state change | `mutex` lock + state change + `cv.notify()` |
| **Await** | Yield to event loop | `cv.wait()` (blocks thread) |
| **Then chaining** | Microtask queue | New promise + stored callback |
| **Error handling** | Try/catch + reject | `exception_ptr` + `rethrow` |
| **Concurrency model** | Single-threaded event loop | Multi-threaded |
| **Synchronization** | Not needed (single thread) | `mutex` + `condition_variable` |

---

## Why the Differences?

### JavaScript (Single-threaded)
```
   ┌─────────────────┐
   │   Event Loop    │
   │                 │
   │ 1. Call Stack   │
   │ 2. Microtasks   │ ← Promises queued here
   │ 3. Macrotasks   │ ← setTimeout, I/O
   └─────────────────┘
```
- **No races:** Only one thing runs at a time
- **No mutexes needed:** No concurrent access
- **Cooperative:** Functions yield control

### C++ (Multi-threaded)
```
   ┌─────┐  ┌─────┐  ┌─────┐
   │ T1  │  │ T2  │  │ T3  │  ← Threads run in parallel
   └──┬──┘  └──┬──┘  └──┬──┘
      │        │        │
      └────────┴────────┘
            │
     ┌──────▼──────┐
     │ Shared Data │  ← Must protect with mutex!
     └─────────────┘
```
- **Races possible:** Multiple threads access data
- **Mutexes required:** Protect shared state
- **Preemptive:** OS switches threads

---

## Practical Example: Texture Loading

### JavaScript Style
```javascript
async function loadTextures(indices) {
  const promises = indices.map(i =>
    fetch(`/textures/${i}.png`).then(r => r.blob())
  );
  const textures = await Promise.all(promises);
  return textures;
}

// Usage
const textures = await loadTextures([0, 1, 2, 3, 4]);
```

### C++ Style (Your Current Code)
```cpp
void TextureManager::loadBatchAsync(int start, int count) {
  for (int i = 0; i < count; i++) {
    threadPool->enqueueTask([this, i]() {
      loadTexture(i);  // Runs on worker thread
      addToReadyQueue(texture);  // Thread-safe queue
    });
  }
}

// Poll for results (callback pattern)
while (hasReadyTexture()) {
  auto texture = popReadyTexture();
  useTexture(texture);
}
```

### C++ Style (With Promise)
```cpp
shared_ptr<Promise<vector<Texture*>>> loadBatchAsync(int start, int count) {
  vector<shared_ptr<Promise<Texture*>>> promises;

  for (int i = 0; i < count; i++) {
    auto promise = PromiseUtils::async([this, i]() {
      return loadTexture(i);  // Runs on worker thread
    });
    promises.push_back(promise);
  }

  return PromiseUtils::all(promises);
}

// Usage (promise pattern)
auto textures = loadBatchAsync(0, 5)->await();
for (auto* tex : textures) {
  useTexture(tex);
}
```

---

## When to Use Each Pattern

### Callback Queue (Your Current Implementation)
**Good for:**
- Streaming results as they complete
- UI updates (texture arrives → spawn sprite immediately)
- Non-blocking polling

**Example:**
```cpp
loadBatchAsync(0, 100);
// In game loop:
if (hasReadyTexture()) spawnSprite(popReadyTexture());
```

### Promise/Future Pattern
**Good for:**
- Waiting for all results before proceeding
- Error handling (one place to catch errors)
- Composing async operations

**Example:**
```cpp
auto textures = loadBatchAsync(0, 100)->await();
// All textures ready, now process them
```

### Hybrid Approach
**Best of both worlds:**
```cpp
// Return promise but also populate callback queue
auto promise = loadBatchAsync(0, 100);

// Use queue for streaming updates
gameLoop: if (hasReadyTexture()) spawnSprite();

// Use promise to wait for completion
promise->await();
showLoadingComplete();
```

---

## Summary

**The hack to make threading work like promises:**

1. **Mutex** replaces single-threaded guarantees
2. **Condition variable** replaces event loop notifications
3. **Callbacks + new promises** replace microtask queue
4. **Blocking wait** replaces cooperative yielding

**Core insight:** JavaScript promises are fundamentally about **managing asynchronous state changes** in a single-threaded environment. C++ promises do the same but add **thread synchronization** because of multi-threading.

The Promise class we built is essentially:
```
Promise = Shared State + Mutex + Condition Variable + Callbacks
```

This gives you the same ergonomics as JavaScript, but with the power of multi-threading!
