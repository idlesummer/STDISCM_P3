#pragma once
#include "Promise.h"
#include <vector>
#include <thread>

/**
 * Utility functions for working with Promises
 */
namespace PromiseUtils {

    /**
     * Promise.all() - Wait for all promises to complete
     * (like: Promise.all([p1, p2, p3]))
     *
     * Returns a promise that resolves with a vector of all results
     */
    template<typename T>
    std::shared_ptr<Promise<std::vector<T>>> all(
        const std::vector<std::shared_ptr<Promise<T>>>& promises) {

        return Promise<std::vector<T>>::create(
            [promises](auto resolve, auto reject) {
                // Launch thread to wait for all promises
                std::thread([promises, resolve, reject]() {
                    std::vector<T> results;
                    results.reserve(promises.size());

                    try {
                        for (auto& promise : promises) {
                            results.push_back(promise->await());  // Wait for each
                        }
                        resolve(results);
                    } catch (...) {
                        reject(std::current_exception());
                    }
                }).detach();
            }
        );
    }

    /**
     * Promise.race() - Wait for first promise to complete
     * (like: Promise.race([p1, p2, p3]))
     */
    template<typename T>
    std::shared_ptr<Promise<T>> race(
        const std::vector<std::shared_ptr<Promise<T>>>& promises) {

        return Promise<T>::create(
            [promises](auto resolve, auto reject) {
                auto settled = std::make_shared<std::atomic<bool>>(false);

                for (auto& promise : promises) {
                    std::thread([promise, resolve, reject, settled]() {
                        try {
                            T result = promise->await();
                            if (!settled->exchange(true)) {
                                resolve(result);
                            }
                        } catch (...) {
                            if (!settled->exchange(true)) {
                                reject(std::current_exception());
                            }
                        }
                    }).detach();
                }
            }
        );
    }

    /**
     * Delay - Create a promise that resolves after a delay
     * (like: new Promise(resolve => setTimeout(resolve, ms)))
     */
    template<typename T>
    std::shared_ptr<Promise<T>> delay(T value, int milliseconds) {
        return Promise<T>::create([value, milliseconds](auto resolve, auto reject) {
            std::thread([value, milliseconds, resolve]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
                resolve(value);
            }).detach();
        });
    }

    /**
     * Async executor - Run a function asynchronously and return a promise
     * (like: new Promise(resolve => resolve(someFunction())))
     */
    template<typename Func>
    auto async(Func func) -> std::shared_ptr<Promise<decltype(func())>> {
        using ReturnType = decltype(func());

        return Promise<ReturnType>::create([func](auto resolve, auto reject) {
            std::thread([func, resolve, reject]() {
                try {
                    if constexpr (std::is_void_v<ReturnType>) {
                        func();
                        resolve();
                    } else {
                        resolve(func());
                    }
                } catch (...) {
                    reject(std::current_exception());
                }
            }).detach();
        });
    }

    /**
     * Retry - Retry a promise-returning function on failure
     */
    template<typename Func>
    auto retry(Func promiseFunc, int maxAttempts)
        -> decltype(promiseFunc()) {

        using PromiseType = decltype(promiseFunc());
        using ValueType = typename std::decay_t<decltype(*promiseFunc())>::value_type;

        return Promise<ValueType>::create(
            [promiseFunc, maxAttempts](auto resolve, auto reject) {
                std::thread([promiseFunc, maxAttempts, resolve, reject]() {
                    for (int i = 0; i < maxAttempts; i++) {
                        try {
                            auto promise = promiseFunc();
                            auto result = promise->await();
                            resolve(result);
                            return;
                        } catch (const std::exception& e) {
                            if (i == maxAttempts - 1) {
                                reject(std::current_exception());
                                return;
                            }
                            std::cout << "[PromiseUtils] Retry " << (i + 1)
                                     << "/" << maxAttempts << " failed: "
                                     << e.what() << std::endl;
                        }
                    }
                }).detach();
            }
        );
    }
}
