#include "../src/utils/Promise.h"
#include "../src/utils/PromiseUtils.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

/**
 * Examples showing how C++ Promise class works like JavaScript Promises
 */

// ========== EXAMPLE 1: Basic Promise Creation ==========
void example1_BasicPromise() {
    cout << "\n=== Example 1: Basic Promise ===" << endl;

    auto promise = Promise<int>::create([](auto resolve, auto reject) {
        // This runs in the executor
        cout << "[Executor] Starting async work..." << endl;

        // Simulate async work in a new thread
        thread([resolve]() {
            this_thread::sleep_for(chrono::milliseconds(500));
            cout << "[Async Thread] Work complete!" << endl;
            resolve(42);  // Resolve with value
        }).detach();
    });

    cout << "[Main] Promise created, waiting for result..." << endl;
    int result = promise->await();  // Block until resolved (like await)
    cout << "[Main] Got result: " << result << endl;
}

// ========== EXAMPLE 2: Promise Chaining (.then) ==========
void example2_PromiseChaining() {
    cout << "\n=== Example 2: Promise Chaining ===" << endl;

    auto promise = Promise<int>::create([](auto resolve, auto reject) {
        thread([resolve]() {
            this_thread::sleep_for(chrono::milliseconds(500));
            resolve(10);
        }).detach();
    });

    // Chain .then() calls (like JavaScript)
    promise->then([](int value) {
        cout << "[Then 1] Got: " << value << endl;
        return value * 2;
    })->then([](int doubled) {
        cout << "[Then 2] Doubled: " << doubled << endl;
        return doubled + 5;
    })->then([](int final) {
        cout << "[Then 3] Final: " << final << endl;
    });

    // Keep program alive to see results
    this_thread::sleep_for(chrono::seconds(1));
}

// ========== EXAMPLE 3: Error Handling (reject) ==========
void example3_ErrorHandling() {
    cout << "\n=== Example 3: Error Handling ===" << endl;

    auto promise = Promise<int>::create([](auto resolve, auto reject) {
        thread([reject]() {
            this_thread::sleep_for(chrono::milliseconds(500));
            // Simulate an error
            reject(make_exception_ptr(runtime_error("Something went wrong!")));
        }).detach();
    });

    promise->catchError([](exception_ptr ex) {
        try {
            rethrow_exception(ex);
        } catch (const exception& e) {
            cout << "[Catch] Error caught: " << e.what() << endl;
        }
    });

    this_thread::sleep_for(chrono::seconds(1));
}

// ========== EXAMPLE 4: Promise.all() ==========
void example4_PromiseAll() {
    cout << "\n=== Example 4: Promise.all() ===" << endl;

    // Create multiple promises
    vector<shared_ptr<Promise<int>>> promises;

    for (int i = 0; i < 5; i++) {
        auto promise = Promise<int>::create([i](auto resolve, auto reject) {
            thread([i, resolve]() {
                int delay = (i + 1) * 200;  // Different delays
                this_thread::sleep_for(chrono::milliseconds(delay));
                cout << "[Promise " << i << "] Resolved after " << delay << "ms" << endl;
                resolve(i * 10);
            }).detach();
        });
        promises.push_back(promise);
    }

    // Wait for all to complete
    auto allPromise = PromiseUtils::all(promises);

    cout << "[Main] Waiting for all promises..." << endl;
    vector<int> results = allPromise->await();

    cout << "[Main] All promises completed! Results: ";
    for (int result : results) {
        cout << result << " ";
    }
    cout << endl;
}

// ========== EXAMPLE 5: Async Helper ==========
void example5_AsyncHelper() {
    cout << "\n=== Example 5: Async Helper ===" << endl;

    // Run a function asynchronously
    auto promise = PromiseUtils::async([]() {
        this_thread::sleep_for(chrono::milliseconds(500));
        cout << "[Async] Computing expensive operation..." << endl;
        return 100 + 200;
    });

    cout << "[Main] Started async work" << endl;
    int result = promise->await();
    cout << "[Main] Result: " << result << endl;
}

// ========== EXAMPLE 6: Texture Loading (Like Your Use Case) ==========

struct MockTexture {
    int id;
    string name;
};

shared_ptr<Promise<MockTexture*>> loadTextureAsync(int id, const string& name) {
    return Promise<MockTexture*>::create([id, name](auto resolve, auto reject) {
        thread([id, name, resolve]() {
            // Simulate loading time
            this_thread::sleep_for(chrono::milliseconds(300));

            auto texture = new MockTexture{id, name};
            cout << "[Loader] Loaded texture: " << name << " (id: " << id << ")" << endl;

            resolve(texture);
        }).detach();
    });
}

void example6_TextureLoading() {
    cout << "\n=== Example 6: Texture Loading (Your Use Case) ===" << endl;

    // Load multiple textures
    vector<shared_ptr<Promise<MockTexture*>>> promises;

    for (int i = 0; i < 3; i++) {
        auto promise = loadTextureAsync(i, "texture_" + to_string(i) + ".png");
        promises.push_back(promise);
    }

    cout << "[Main] Started loading 3 textures..." << endl;

    // Wait for all textures to load
    auto allPromise = PromiseUtils::all(promises);
    vector<MockTexture*> textures = allPromise->await();

    cout << "[Main] All textures loaded!" << endl;
    for (auto* texture : textures) {
        cout << "  - " << texture->name << " (id: " << texture->id << ")" << endl;
        delete texture;
    }
}

// ========== EXAMPLE 7: JavaScript Comparison ==========
void example7_JavaScriptComparison() {
    cout << "\n=== Example 7: JavaScript vs C++ Comparison ===" << endl;

    cout << "JavaScript:" << endl;
    cout << "  const promise = new Promise((resolve, reject) => {" << endl;
    cout << "    setTimeout(() => resolve(42), 500);" << endl;
    cout << "  });" << endl;
    cout << "  const result = await promise;" << endl;
    cout << "  console.log(result);" << endl;
    cout << endl;

    cout << "C++ Equivalent:" << endl;

    auto promise = Promise<int>::create([](auto resolve, auto reject) {
        thread([resolve]() {
            this_thread::sleep_for(chrono::milliseconds(500));
            resolve(42);
        }).detach();
    });

    int result = promise->await();
    cout << "  result = " << result << endl;
}

// ========== EXAMPLE 8: How It Works Internally ==========
void example8_InternalMechanism() {
    cout << "\n=== Example 8: Internal Mechanism ===" << endl;
    cout << "How Promise works under the hood:" << endl;
    cout << "1. SharedState holds: mutex, condition_variable, state, value" << endl;
    cout << "2. resolve() callback:" << endl;
    cout << "   - Locks mutex" << endl;
    cout << "   - Sets state = FULFILLED" << endl;
    cout << "   - Stores value" << endl;
    cout << "   - Notifies condition_variable" << endl;
    cout << "3. await() method:" << endl;
    cout << "   - Locks mutex" << endl;
    cout << "   - Waits on condition_variable until state != PENDING" << endl;
    cout << "   - Returns value (or throws exception)" << endl;
    cout << endl;

    cout << "This is exactly like:" << endl;
    cout << "  JavaScript: Promise internal [[PromiseState]] and [[PromiseResult]]" << endl;
    cout << "  C++: SharedState with mutex/cv for synchronization" << endl;
}

int main() {
    cout << "========================================" << endl;
    cout << "  Promise Examples - JS vs C++" << endl;
    cout << "========================================" << endl;

    example1_BasicPromise();
    example2_PromiseChaining();
    example3_ErrorHandling();
    example4_PromiseAll();
    example5_AsyncHelper();
    example6_TextureLoading();
    example7_JavaScriptComparison();
    example8_InternalMechanism();

    cout << "\n========================================" << endl;
    cout << "  All Examples Complete!" << endl;
    cout << "========================================" << endl;

    return 0;
}
