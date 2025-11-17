#pragma once

#include <functional>
#include <vector>
#include <any>
#include <unordered_map>
#include <string>


class Component; // Forward declaration

struct Effect {
    std::function<void()> callback;
    std::vector<std::any> deps;
    std::function<void()> cleanup;  // Cleanup function (like useEffect return value)
};

// Manages effects for components (like React's useEffect)
class EffectManager {
public:
    static EffectManager& getInstance() {
        static auto instance = EffectManager();
        return instance;
    }

    void registerEffect(Component* component, std::function<void()> callback, std::vector<std::any> deps = {}) {
        auto key = this->getEffectKey(component);
        auto effect = Effect();

        effect.callback = callback;
        effect.deps = deps;

        this->effects[key] = effect;
        this->effectsToRun.push_back(key);
    }

    void registerEffectWithCleanup(Component* component, std::function<std::function<void()>()> effect, std::vector<std::any> deps = {}) {
        auto key = this->getEffectKey(component);
        auto e = Effect();

        e.deps = deps;
        e.callback = [effect, &e]() { e.cleanup = effect(); };  // Store cleanup function

        this->effects[key] = e;
        this->effectsToRun.push_back(key);
    }

    void runEffects() {
        for (const auto& key : this->effectsToRun) {
            if (this->effects.find(key) == this->effects.end())
                continue;

            auto& effect = this->effects[key];
            if (!this->shouldRunEffect(key, effect))
                continue;

            // Run cleanup from previous effect
            if (effect.cleanup)
                effect.cleanup();

            // Run new effect
            effect.callback();

            // Store deps for next comparison
            this->previousDependencies[key] = effect.deps;
        }
        this->effectsToRun.clear();
    }

    void cleanup(Component* component) {
        // Run all cleanup functions for this component
        auto prefix = std::to_string(reinterpret_cast<uintptr_t>(component));
        for (auto& [key, effect] : this->effects) {
            if (key.find(prefix) != 0) continue;
            if (!effect.cleanup)       continue;
            effect.cleanup();
        }
    }

    void reset() {
        this->effects.clear();
        this->effectsToRun.clear();
        this->previousDependencies.clear();
        this->effectCounter = 0;
    }

private:
    EffectManager() : effectCounter(0) {}

    std::string getEffectKey(Component* component) {
        return std::to_string(reinterpret_cast<uintptr_t>(component)) + "_effect_" + std::to_string(this->effectCounter++);
    }

    bool shouldRunEffect(const std::string& key, const Effect& effect) {
        // Empty deps = run every time
        if (effect.deps.empty())
            return true;

        // First run
        if (this->previousDependencies.find(key) == this->previousDependencies.end())
            return true;

        // Compare deps (simplified - in real implementation would need deep comparison)
        auto& prevDeps = this->previousDependencies[key];
        if (prevDeps.size() != effect.deps.size())
            return true;

        // deps changed
        for (size_t i = 0; i < prevDeps.size(); i++) {
            if (prevDeps[i].type() != effect.deps[i].type())
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
