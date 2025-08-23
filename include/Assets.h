// Assets.h
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <vector>
#include "Animation.h"

class Assets {
    std::unordered_map<std::string, sf::Texture>     m_textures;
    std::unordered_map<std::string, sf::Font>        m_fonts;
    std::unordered_map<std::string, sf::SoundBuffer> m_sounds;
    std::unordered_map<std::string, Animation>       m_anims;

public:
    void loadTexture(const std::string& name, const std::string& path, bool smooth=true);
    void loadFont(const std::string& name, const std::string& path);
    void loadSound(const std::string& name, const std::string& path);

    const sf::Texture&    tex(const std::string& name)   const { return m_textures.at(name); }
    const sf::Font&       font(const std::string& name)  const { return m_fonts.at(name); }
    const sf::SoundBuffer& snd(const std::string& name)  const { return m_sounds.at(name); }

    void addAnimation(const std::string& name, const Animation& a) { m_anims[name]=a; }
    const Animation& anim(const std::string& name) const { return m_anims.at(name); }

    std::vector<sf::IntRect> makeGridFrames(int frameW,int frameH,int cols,int rows,
                                            int startIndex,int endIndex,
                                            int margin=0,int spacing=0) const;
};
