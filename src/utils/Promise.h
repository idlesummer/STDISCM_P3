#pragma once
#include <functional>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <exception>
#include <iostream>

/**
 * Promise<T> - A JavaScript-style promise implementation using C++ threading primitives
 *
 * Usage:
 *   auto promise = Promise<int>::create([](auto resolve, auto reject) {
 *       // Do async work
 *       resolve(42);
 *   });
 *
 *   promise->then([](int value) {
 *       std::cout << "Got: " << value << std::endl;
 *       return value * 2;
 *   })->then([](int doubled) {
 *       std::cout << "Doubled: " << doubled << std::endl;
 *   });
 */
template<typename T>
class Promise {
public:
    enum class State {
        PENDING,
        FULFILLED,
        REJECTED
    };

    using ResolveFunc = std::function<void(T)>;
    using RejectFunc = std::function<void(std::exception_ptr)>;
    using ExecutorFunc = std::function<void(ResolveFunc, RejectFunc)>;

private:
    // Shared state between promise and continuations
    struct SharedState {
        std::mutex mutex;
        std::condition_variable cv;
        State state = State::PENDING;
        T value;
        std::exception_ptr exception;

        // Callbacks registered via .then()
        std::function<void(T)> onFulfilled;
        std::function<void(std::exception_ptr)> onRejected;
    };

    std::shared_ptr<SharedState> state;

    Promise(std::shared_ptr<SharedState> sharedState)
        : state(sharedState) {}

public:
    /**
     * Create a new Promise (like: new Promise((resolve, reject) => {}))
     */
    static std::shared_ptr<Promise<T>> create(ExecutorFunc executor) {
        auto sharedState = std::make_shared<SharedState>();
        auto promise = std::shared_ptr<Promise<T>>(new Promise<T>(sharedState));

        // Create resolve callback
        auto resolve = [sharedState](T value) {
            std::unique_lock<std::mutex> lock(sharedState->mutex);

            if (sharedState->state != State::PENDING) {
                std::cout << "[Promise] Already settled, ignoring resolve" << std::endl;
                return;
            }

            std::cout << "[Promise] Resolving with value" << std::endl;
            sharedState->state = State::FULFILLED;
            sharedState->value = value;

            // Call registered callback if it exists
            if (sharedState->onFulfilled) {
                auto callback = sharedState->onFulfilled;
                lock.unlock();  // Release lock before calling callback
                callback(value);
            }

            sharedState->cv.notify_all();
        };

        // Create reject callback
        auto reject = [sharedState](std::exception_ptr ex) {
            std::unique_lock<std::mutex> lock(sharedState->mutex);

            if (sharedState->state != State::PENDING) {
                std::cout << "[Promise] Already settled, ignoring reject" << std::endl;
                return;
            }

            std::cout << "[Promise] Rejecting with exception" << std::endl;
            sharedState->state = State::REJECTED;
            sharedState->exception = ex;

            // Call registered error callback if it exists
            if (sharedState->onRejected) {
                auto callback = sharedState->onRejected;
                lock.unlock();  // Release lock before calling callback
                callback(ex);
            }

            sharedState->cv.notify_all();
        };

        // Execute the executor function (usually spawns a thread)
        try {
            executor(resolve, reject);
        } catch (...) {
            reject(std::current_exception());
        }

        return promise;
    }

    /**
     * Chain a callback (like: promise.then(value => {}))
     * Returns a new Promise for chaining
     */
    template<typename Func>
    auto then(Func func) -> std::shared_ptr<Promise<decltype(func(std::declval<T>()))>> {
        using ReturnType = decltype(func(std::declval<T>()));

        return Promise<ReturnType>::create(
            [this, func](auto resolve, auto reject) {
                std::unique_lock<std::mutex> lock(this->state->mutex);

                // If already fulfilled, call immediately
                if (this->state->state == State::FULFILLED) {
                    T value = this->state->value;
                    lock.unlock();

                    try {
                        if constexpr (std::is_void_v<ReturnType>) {
                            func(value);
                            resolve();  // void promises resolve with no value
                        } else {
                            resolve(func(value));
                        }
                    } catch (...) {
                        reject(std::current_exception());
                    }
                    return;
                }

                // If already rejected, propagate error
                if (this->state->state == State::REJECTED) {
                    auto ex = this->state->exception;
                    lock.unlock();
                    reject(ex);
                    return;
                }

                // Still pending - register callback
                this->state->onFulfilled = [func, resolve, reject](T value) {
                    try {
                        if constexpr (std::is_void_v<ReturnType>) {
                            func(value);
                            resolve();
                        } else {
                            resolve(func(value));
                        }
                    } catch (...) {
                        reject(std::current_exception());
                    }
                };
            }
        );
    }

    /**
     * Catch errors (like: promise.catch(err => {}))
     */
    std::shared_ptr<Promise<T>> catchError(std::function<void(std::exception_ptr)> func) {
        std::unique_lock<std::mutex> lock(this->state->mutex);

        // If already rejected, call immediately
        if (this->state->state == State::REJECTED) {
            auto ex = this->state->exception;
            lock.unlock();
            func(ex);
            return shared_from_this();
        }

        // Register error callback
        this->state->onRejected = func;
        return shared_from_this();
    }

    /**
     * Block and wait for result (like: await promise)
     * Returns the value when fulfilled, throws on rejection
     */
    T await() {
        std::unique_lock<std::mutex> lock(this->state->mutex);

        // Wait until settled
        this->state->cv.wait(lock, [this] {
            return this->state->state != State::PENDING;
        });

        if (this->state->state == State::FULFILLED) {
            return this->state->value;
        } else {
            std::rethrow_exception(this->state->exception);
        }
    }

    /**
     * Check if promise is settled
     */
    bool isSettled() const {
        std::lock_guard<std::mutex> lock(this->state->mutex);
        return this->state->state != State::PENDING;
    }

    /**
     * Get current state
     */
    State getState() const {
        std::lock_guard<std::mutex> lock(this->state->mutex);
        return this->state->state;
    }

    /**
     * Static helper: Create an already-resolved promise
     * (like: Promise.resolve(value))
     */
    static std::shared_ptr<Promise<T>> resolve(T value) {
        return create([value](auto resolve, auto reject) {
            resolve(value);
        });
    }

    /**
     * Static helper: Create an already-rejected promise
     * (like: Promise.reject(error))
     */
    static std::shared_ptr<Promise<T>> reject(std::exception_ptr ex) {
        return create([ex](auto resolve, auto reject) {
            reject(ex);
        });
    }
};

// Specialization for void promises
template<>
class Promise<void> {
public:
    enum class State {
        PENDING,
        FULFILLED,
        REJECTED
    };

    using ResolveFunc = std::function<void()>;
    using RejectFunc = std::function<void(std::exception_ptr)>;
    using ExecutorFunc = std::function<void(ResolveFunc, RejectFunc)>;

private:
    struct SharedState {
        std::mutex mutex;
        std::condition_variable cv;
        State state = State::PENDING;
        std::exception_ptr exception;

        std::function<void()> onFulfilled;
        std::function<void(std::exception_ptr)> onRejected;
    };

    std::shared_ptr<SharedState> state;

    Promise(std::shared_ptr<SharedState> sharedState)
        : state(sharedState) {}

public:
    static std::shared_ptr<Promise<void>> create(ExecutorFunc executor) {
        auto sharedState = std::make_shared<SharedState>();
        auto promise = std::shared_ptr<Promise<void>>(new Promise<void>(sharedState));

        auto resolve = [sharedState]() {
            std::unique_lock<std::mutex> lock(sharedState->mutex);
            if (sharedState->state != State::PENDING) return;

            sharedState->state = State::FULFILLED;

            if (sharedState->onFulfilled) {
                auto callback = sharedState->onFulfilled;
                lock.unlock();
                callback();
            }

            sharedState->cv.notify_all();
        };

        auto reject = [sharedState](std::exception_ptr ex) {
            std::unique_lock<std::mutex> lock(sharedState->mutex);
            if (sharedState->state != State::PENDING) return;

            sharedState->state = State::REJECTED;
            sharedState->exception = ex;

            if (sharedState->onRejected) {
                auto callback = sharedState->onRejected;
                lock.unlock();
                callback(ex);
            }

            sharedState->cv.notify_all();
        };

        try {
            executor(resolve, reject);
        } catch (...) {
            reject(std::current_exception());
        }

        return promise;
    }

    void await() {
        std::unique_lock<std::mutex> lock(this->state->mutex);

        this->state->cv.wait(lock, [this] {
            return this->state->state != State::PENDING;
        });

        if (this->state->state == State::REJECTED) {
            std::rethrow_exception(this->state->exception);
        }
    }

    static std::shared_ptr<Promise<void>> resolve() {
        return create([](auto resolve, auto reject) {
            resolve();
        });
    }
};
