# JavaScript Promises vs C++ Threading: Side-by-Side

## 1. Creating a Promise

### JavaScript
```javascript
const promise = new Promise((resolve, reject) => {
  setTimeout(() => {
    resolve(42);
  }, 1000);
});
```

### C++
```cpp
auto promise = Promise<int>::create([](auto resolve, auto reject) {
  thread([resolve]() {
    this_thread::sleep_for(chrono::milliseconds(1000));
    resolve(42);
  }).detach();
});
```

**Key components:**
- `new Promise()` → `Promise<T>::create()`
- `resolve` callback → `ResolveFunc` passed to executor
- `setTimeout` → `thread` with `sleep_for`

---

## 2. Waiting for a Result

### JavaScript
```javascript
// Async/await (non-blocking, yields to event loop)
const result = await promise;
console.log(result);  // 42
```

### C++
```cpp
// await() (blocking, waits on condition variable)
int result = promise->await();
cout << result << endl;  // 42
```

**Internals:**

**JavaScript:**
```javascript
// Pseudocode of what happens
if (promise.state === "pending") {
  // Pause function, add to microtask queue
  yieldToEventLoop();
}
return promise.value;
```

**C++:**
```cpp
T await() {
  unique_lock<mutex> lock(mtx);
  cv.wait(lock, [this] {
    return state != PENDING;  // Wait until resolved
  });
  return value;
}
```

---

## 3. Chaining Promises

### JavaScript
```javascript
promise
  .then(value => value * 2)
  .then(doubled => doubled + 10)
  .then(result => console.log(result));  // 94
```

### C++
```cpp
promise
  ->then([](int value) { return value * 2; })
  ->then([](int doubled) { return doubled + 10; })
  ->then([](int result) { cout << result << endl; });  // 94
```

**Internals - How .then() creates a new promise:**

**JavaScript:**
```javascript
Promise.prototype.then = function(onFulfilled) {
  return new Promise((resolve, reject) => {
    if (this.state === "fulfilled") {
      resolve(onFulfilled(this.value));
    } else {
      this.reactions.push(() => {
        resolve(onFulfilled(this.value));
      });
    }
  });
};
```

**C++:**
```cpp
template<typename Func>
auto then(Func func) -> shared_ptr<Promise<ReturnType>> {
  return Promise<ReturnType>::create([this, func](auto resolve, auto reject) {
    lock_guard lock(mtx);
    if (state == FULFILLED) {
      resolve(func(value));
    } else {
      onFulfilled = [func, resolve](T val) {
        resolve(func(val));
      };
    }
  });
}
```

---

## 4. Error Handling

### JavaScript
```javascript
promise
  .then(value => {
    if (value < 0) throw new Error("Negative!");
    return value * 2;
  })
  .catch(error => {
    console.error("Error:", error.message);
  });
```

### C++
```cpp
promise
  ->then([](int value) {
    if (value < 0)
      throw runtime_error("Negative!");
    return value * 2;
  })
  ->catchError([](exception_ptr ex) {
    try {
      rethrow_exception(ex);
    } catch (const exception& e) {
      cerr << "Error: " << e.what() << endl;
    }
  });
```

**Internals - Exception propagation:**

**JavaScript:**
```javascript
try {
  result = onFulfilled(value);
  nextPromise.resolve(result);
} catch (error) {
  nextPromise.reject(error);
}
```

**C++:**
```cpp
try {
  ReturnType result = func(value);
  resolve(result);
} catch (...) {
  reject(current_exception());  // Captures exception_ptr
}
```

---

## 5. Promise.all()

### JavaScript
```javascript
const p1 = Promise.resolve(10);
const p2 = Promise.resolve(20);
const p3 = Promise.resolve(30);

const results = await Promise.all([p1, p2, p3]);
console.log(results);  // [10, 20, 30]
```

### C++
```cpp
auto p1 = Promise<int>::resolve(10);
auto p2 = Promise<int>::resolve(20);
auto p3 = Promise<int>::resolve(30);

auto results = PromiseUtils::all({p1, p2, p3})->await();
cout << results[0] << ", " << results[1] << ", " << results[2] << endl;
// 10, 20, 30
```

**Internals - How Promise.all() works:**

**JavaScript:**
```javascript
Promise.all = function(promises) {
  return new Promise((resolve, reject) => {
    const results = [];
    let completed = 0;

    promises.forEach((promise, index) => {
      promise.then(value => {
        results[index] = value;
        completed++;
        if (completed === promises.length) {
          resolve(results);
        }
      }).catch(reject);
    });
  });
};
```

**C++:**
```cpp
template<typename T>
shared_ptr<Promise<vector<T>>> all(vector<shared_ptr<Promise<T>>> promises) {
  return Promise<vector<T>>::create([promises](auto resolve, auto reject) {
    thread([promises, resolve, reject]() {
      vector<T> results;
      for (auto& p : promises) {
        try {
          results.push_back(p->await());  // Wait for each
        } catch (...) {
          reject(current_exception());
          return;
        }
      }
      resolve(results);
    }).detach();
  });
}
```

**Key difference:** JavaScript uses counters and event loop, C++ blocks a thread waiting for each promise.

---

## 6. Promise.race()

### JavaScript
```javascript
const slow = new Promise(resolve => setTimeout(() => resolve("slow"), 2000));
const fast = new Promise(resolve => setTimeout(() => resolve("fast"), 500));

const winner = await Promise.race([slow, fast]);
console.log(winner);  // "fast"
```

### C++
```cpp
auto slow = PromiseUtils::delay(string("slow"), 2000);
auto fast = PromiseUtils::delay(string("fast"), 500);

auto winner = PromiseUtils::race({slow, fast})->await();
cout << winner << endl;  // "fast"
```

**Internals:**

**JavaScript:**
```javascript
Promise.race = function(promises) {
  return new Promise((resolve, reject) => {
    promises.forEach(promise => {
      promise.then(resolve).catch(reject);
    });
  });
};
```

**C++:**
```cpp
template<typename T>
shared_ptr<Promise<T>> race(vector<shared_ptr<Promise<T>>> promises) {
  return Promise<T>::create([promises](auto resolve, auto reject) {
    auto settled = make_shared<atomic<bool>>(false);

    for (auto& p : promises) {
      thread([p, resolve, reject, settled]() {
        try {
          T result = p->await();
          if (!settled->exchange(true)) {  // First to finish wins
            resolve(result);
          }
        } catch (...) {
          if (!settled->exchange(true)) {
            reject(current_exception());
          }
        }
      }).detach();
    }
  });
}
```

**Key difference:** JavaScript relies on event loop, C++ spawns a thread per promise to race them.

---

## 7. Async Functions

### JavaScript
```javascript
async function fetchUserData(userId) {
  const response = await fetch(`/api/users/${userId}`);
  const data = await response.json();
  return data;
}

const user = await fetchUserData(123);
```

### C++
```cpp
// No async/await keywords, but same pattern:
shared_ptr<Promise<UserData>> fetchUserData(int userId) {
  return PromiseUtils::async([userId]() {
    auto response = httpGet("/api/users/" + to_string(userId));
    auto data = parseJson(response);
    return data;
  });
}

auto user = fetchUserData(123)->await();
```

**What `async` does:**

**JavaScript:**
```javascript
// This:
async function foo() {
  return 42;
}

// Is syntactic sugar for:
function foo() {
  return Promise.resolve(42);
}
```

**C++:**
```cpp
// Helper to auto-wrap in promise:
template<typename Func>
auto async(Func func) {
  using ReturnType = decltype(func());
  return Promise<ReturnType>::create([func](auto resolve, auto reject) {
    thread([func, resolve, reject]() {
      try {
        resolve(func());
      } catch (...) {
        reject(current_exception());
      }
    }).detach();
  });
}
```

---

## 8. Real-World Example: Loading Multiple Resources

### JavaScript
```javascript
async function loadGameAssets() {
  console.log("Loading assets...");

  const textures = await Promise.all([
    loadTexture("player.png"),
    loadTexture("enemy.png"),
    loadTexture("background.png")
  ]);

  console.log("Loaded", textures.length, "textures");

  const sounds = await Promise.all([
    loadSound("jump.wav"),
    loadSound("shoot.wav")
  ]);

  console.log("Loaded", sounds.length, "sounds");

  return { textures, sounds };
}

// Usage
const assets = await loadGameAssets();
game.start(assets);
```

### C++
```cpp
shared_ptr<Promise<GameAssets>> loadGameAssets() {
  return PromiseUtils::async([]() {
    cout << "Loading assets..." << endl;

    auto texturePromises = vector{
      loadTexture("player.png"),
      loadTexture("enemy.png"),
      loadTexture("background.png")
    };

    auto textures = PromiseUtils::all(texturePromises)->await();
    cout << "Loaded " << textures.size() << " textures" << endl;

    auto soundPromises = vector{
      loadSound("jump.wav"),
      loadSound("shoot.wav")
    };

    auto sounds = PromiseUtils::all(soundPromises)->await();
    cout << "Loaded " << sounds.size() << " sounds" << endl;

    return GameAssets{textures, sounds};
  });
}

// Usage
auto assets = loadGameAssets()->await();
game.start(assets);
```

---

## 9. The Fundamental Primitives

### JavaScript Event Loop
```
┌───────────────────────────┐
│   JavaScript Runtime      │
│                           │
│  ┌─────────────────────┐  │
│  │    Call Stack       │  │  ← Currently executing code
│  └─────────────────────┘  │
│           ↕               │
│  ┌─────────────────────┐  │
│  │  Microtask Queue    │  │  ← Promises (.then callbacks)
│  └─────────────────────┘  │
│           ↕               │
│  ┌─────────────────────┐  │
│  │  Macrotask Queue    │  │  ← setTimeout, I/O callbacks
│  └─────────────────────┘  │
│                           │
└───────────────────────────┘

Event loop checks:
1. Execute call stack
2. Process ALL microtasks
3. Process ONE macrotask
4. Repeat
```

### C++ Threading Model
```
┌─────────────────────────────────┐
│      Operating System           │
│                                 │
│  ┌─────┐  ┌─────┐  ┌─────┐     │
│  │ T1  │  │ T2  │  │ T3  │     │  ← Threads (run in parallel)
│  └──┬──┘  └──┬──┘  └──┬──┘     │
│     │        │        │         │
│     └────────┴────────┘         │
│              │                  │
│     ┌────────▼────────┐         │
│     │  Shared Memory  │         │  ← Mutex protects this
│     │  (Promise state)│         │
│     └────────┬────────┘         │
│              │                  │
│     ┌────────▼────────┐         │
│     │ Condition Var   │         │  ← Signals state changes
│     └─────────────────┘         │
│                                 │
└─────────────────────────────────┘

Threads communicate via:
1. Shared memory (mutex-protected)
2. Condition variables (signaling)
3. No central "loop" - OS schedules threads
```

---

## 10. Complete Equivalence Table

| JavaScript | C++ | Purpose |
|-----------|-----|---------|
| `new Promise((resolve, reject) => {})` | `Promise<T>::create([](auto resolve, auto reject) {})` | Create promise |
| `await promise` | `promise->await()` | Wait for result |
| `.then(value => {})` | `->then([](T value) {})` | Chain callbacks |
| `.catch(err => {})` | `->catchError([](exception_ptr ex) {})` | Handle errors |
| `Promise.resolve(value)` | `Promise<T>::resolve(value)` | Immediate resolve |
| `Promise.reject(error)` | `Promise<T>::reject(ex)` | Immediate reject |
| `Promise.all([...])` | `PromiseUtils::all({...})` | Wait for all |
| `Promise.race([...])` | `PromiseUtils::race({...})` | Wait for first |
| `async function` | `shared_ptr<Promise<T>> function()` | Return promise |
| Event loop | `mutex` + `condition_variable` | Synchronization |
| Microtask queue | Stored callback in `onFulfilled` | Deferred execution |
| Single-threaded | Multi-threaded | Execution model |

---

## Key Takeaway

**JavaScript Promises:**
- State machine + event loop
- Single-threaded (no races)
- Non-blocking (yields to event loop)

**C++ Promises (our implementation):**
- State machine + mutex + condition variable
- Multi-threaded (requires synchronization)
- Blocking (waits on CV) or callback-based

**Same concept, different execution model!**
