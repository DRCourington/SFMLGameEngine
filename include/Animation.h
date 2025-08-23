#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

struct Animation {
    std::string texture;
    std::vector<sf::IntRect> frames;
    float frameTime = 0.1f;
    bool loop = true;

    // runtime
    float time = 0.f;
    std::size_t index = 0;
    bool finished = false;

    Animation() = default;
    Animation(std::string tex, std::vector<sf::IntRect> f, float ft, bool lp=true)
    : texture(std::move(tex)), frames(std::move(f)), frameTime(ft), loop(lp) {}

    void reset() { time=0.f; index=0; finished=false; }
    void update(float dt) {
        if (finished || frames.empty()) return;
        time += dt;
        while (time >= frameTime) {
            time -= frameTime;
            index++;
            if (index >= frames.size()) {
                if (loop) index = 0;
                else { index = frames.size()-1; finished = true; }
            }
        }
    }
    const sf::IntRect& rect() const { return frames[index]; }
};
