#include "../include/Assets.h"
#include <fstream>
#include <sstream>
#include <iostream>

static bool isCommentOrBlank(const std::string& s) {
    for (char ch : s) {
        if (ch == '#') return true;
        if (!std::isspace(static_cast<unsigned char>(ch))) return false;
    }
    return true;
}

void Assets::loadTexture(const std::string& name, const std::string& path, bool smooth) {
    sf::Texture t;
    if (!t.loadFromFile(path)) {
        std::cerr << "[Assets] Failed texture: " << name << " <- " << path << "\n";
    }
    t.setSmooth(smooth);
    m_textures[name] = std::move(t);
}

void Assets::loadFont(const std::string& name, const std::string& path) {
    sf::Font f;
    if (!f.openFromFile(path)) {
        std::cerr << "[Assets] Failed font: " << name << " <- " << path << "\n";
    }
    m_fonts[name] = std::move(f);
}

void Assets::loadSound(const std::string& name, const std::string& path) {
    sf::SoundBuffer b;
    if (!b.loadFromFile(path)) {
        std::cerr << "[Assets] Failed sound: " << name << " <- " << path << "\n";
    }
    m_sounds[name] = std::move(b);
}

// Simple config loader.
// Lines like:
//   Texture   Name path/to/tex.png
//   Font      Name path/to/font.ttf
//   Sound     Name path/to/sound.wav
//   Animation Name TextureName frameCount speed
void Assets::loadFromFile(const std::string& path) {
    std::ifstream fin(path);
    if (!fin) {
        std::cerr << "[Assets] Cannot open config: " << path << "\n";
        return;
    }

    std::string line;
    size_t ln = 0;

    while (std::getline(fin, line)) {
        ++ln;
        if (isCommentOrBlank(line)) continue;

        std::istringstream iss(line);
        std::string kind;
        iss >> kind;

        if (kind == "Texture") {
            std::string name, file;
            iss >> name >> file;
            if (name.empty() || file.empty()) {
                std::cerr << "[Assets] Bad Texture line " << ln << "\n";
                continue;
            }
            loadTexture(name, file, true);
        } else if (kind == "Font") {
            std::string name, file;
            iss >> name >> file;
            if (name.empty() || file.empty()) {
                std::cerr << "[Assets] Bad Font line " << ln << "\n";
                continue;
            }
            loadFont(name, file);
        } else if (kind == "Sound") {
            std::string name, file;
            iss >> name >> file;
            if (name.empty() || file.empty()) {
                std::cerr << "[Assets] Bad Sound line " << ln << "\n";
                continue;
            }
            loadSound(name, file);
        } else if (kind == "Animation") {
            std::string name, texName;
            std::size_t frameCount = 1, speed = 0;
            iss >> name >> texName >> frameCount >> speed;
            if (name.empty() || texName.empty()) {
                std::cerr << "[Assets] Bad Animation line " << ln << "\n";
                continue;
            }
            auto it = m_textures.find(texName);
            if (it == m_textures.end()) {
                std::cerr << "[Assets] Animation texture missing: " << texName
                          << " (line " << ln << ")\n";
                continue;
            }
            addAnimation(name, Animation(name, it->second, frameCount, speed));
        } else {
            std::cerr << "[Assets] Unknown kind '" << kind << "' on line " << ln << "\n";
        }
    }
}

const sf::Texture& Assets::getTexture(const std::string& n) const {
    static sf::Texture dummy;
    auto it = m_textures.find(n);
    if (it == m_textures.end()) { std::cerr << "[Assets] Missing texture: " << n << "\n"; return dummy; }
    return it->second;
}

const sf::Font& Assets::getFont(const std::string& n) const {
    static sf::Font dummy;
    auto it = m_fonts.find(n);
    if (it == m_fonts.end()) { std::cerr << "[Assets] Missing font: " << n << "\n"; return dummy; }
    return it->second;
}

const sf::SoundBuffer& Assets::getSound(const std::string& n) const {
    static sf::SoundBuffer dummy;
    auto it = m_sounds.find(n);
    if (it == m_sounds.end()) { std::cerr << "[Assets] Missing sound: " << n << "\n"; return dummy; }
    return it->second;
}

const Animation& Assets::getAnimation(const std::string& n) const {
    static Animation dummy;
    auto it = m_anims.find(n);
    if (it == m_anims.end()) { std::cerr << "[Assets] Missing animation: " << n << "\n"; return dummy; }
    return it->second;
}


std::vector<sf::IntRect> Assets::makeGridFrames(int w,int h,int cols,int rows,
                                                int start,int end,int margin,int spacing) const {
    std::vector<sf::IntRect> frames; frames.reserve(end-start+1);
    int idx=0;
    for(int r=0;r<rows;r++){
        for(int c=0;c<cols;c++,idx++){
            if(idx<start||idx>end) continue;
            int x = margin + c*(w+spacing);
            int y = margin + r*(h+spacing);
            frames.emplace_back(sf::Vector2i{x,y},sf::Vector2i{w,h});
        }
    }
    return frames;
}
