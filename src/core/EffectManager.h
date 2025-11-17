#pragma once

#include <functional>
#include <vector>
#include <any>
#include <memory>
#include <unordered_map>

namespace Reactive {

class Component; // Forward declaration

struct Effect {
    std::function<void()> callback;
    std::vector<std::any> dependencies;
    std::function<void()> cleanup;  // Cleanup function (like useEffect return value)
};

// Manages effects for components (like React's useEffect)
class EffectManager {
public:
    static EffectManager& getInstance() {
        static EffectManager instance;
        return instance;
    }

    void registerEffect(Component* component, std::function<void()> callback, std::vector<std::any> dependencies = {}) {
        auto key = getEffectKey(component);

        Effect effect;
        effect.callback = callback;
        effect.dependencies = dependencies;

        effects[key] = effect;
        effectsToRun.push_back(key);
    }

    void registerEffectWithCleanup(Component* component, std::function<std::function<void()>()> effect, std::vector<std::any> dependencies = {}) {
        auto key = getEffectKey(component);

        Effect e;
        e.dependencies = dependencies;
        e.callback = [effect, &e]() {
            e.cleanup = effect();  // Store cleanup function
        };

        effects[key] = e;
        effectsToRun.push_back(key);
    }

    void runEffects() {
        for (const auto& key : effectsToRun) {
            if (effects.find(key) != effects.end()) {
                auto& effect = effects[key];
                if (shouldRunEffect(key, effect)) {
                    // Run cleanup from previous effect
                    if (effect.cleanup)
                        effect.cleanup();

                    // Run new effect
                    effect.callback();

                    // Store dependencies for next comparison
                    previousDependencies[key] = effect.dependencies;
                }
            }
        }
        effectsToRun.clear();
    }

    void cleanup(Component* component) {
        // Run all cleanup functions for this component
        auto prefix = std::to_string(reinterpret_cast<uintptr_t>(component));
        for (auto& [key, effect] : effects) {
            if (key.find(prefix) == 0 && effect.cleanup) {
                effect.cleanup();
            }
        }
    }

    void reset() {
        effects.clear();
        effectsToRun.clear();
        previousDependencies.clear();
        effectCounter = 0;
    }

private:
    EffectManager() : effectCounter(0) {}

    std::string getEffectKey(Component* component) {
        return std::to_string(reinterpret_cast<uintptr_t>(component)) + "_effect_" + std::to_string(effectCounter++);
    }

    bool shouldRunEffect(const std::string& key, const Effect& effect) {
        // Empty dependencies = run every time
        if (effect.dependencies.empty())
            return true;

        // First run
        if (previousDependencies.find(key) == previousDependencies.end())
            return true;

        // Compare dependencies (simplified - in real implementation would need deep comparison)
        auto& prevDeps = previousDependencies[key];
        if (prevDeps.size() != effect.dependencies.size())
            return true;

        // Dependencies changed
        for (size_t i = 0; i < prevDeps.size(); i++) {
            if (prevDeps[i].type() != effect.dependencies[i].type())
                return true;
            // Note: Proper comparison would need type-specific logic
        }

        return false;
    }

    std::unordered_map<std::string, Effect> effects;
    std::vector<std::string> effectsToRun;
    std::unordered_map<std::string, std::vector<std::any>> previousDependencies;
    size_t effectCounter;
};

} // namespace Reactive
