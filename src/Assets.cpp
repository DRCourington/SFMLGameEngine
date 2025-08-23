#include "../include/Assets.h"
#include <iostream>

void Assets::loadTexture(const std::string& name,const std::string& path,bool smooth){
    sf::Texture t;
    if(!t.loadFromFile(path)) std::cerr<<"[Assets] tex fail "<<path<<"\n";
    t.setSmooth(smooth);
    m_textures[name]=std::move(t);
}
void Assets::loadFont(const std::string& name,const std::string& path){
    sf::Font f; if(!f.openFromFile(path)) std::cerr<<"[Assets] font fail "<<path<<"\n";
    m_fonts[name]=std::move(f);
}
void Assets::loadSound(const std::string& name,const std::string& path){
    sf::SoundBuffer b; if(!b.loadFromFile(path)) std::cerr<<"[Assets] snd fail "<<path<<"\n";
    m_sounds[name]=std::move(b);
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
            frames.emplace_back(x,y,w,h);
        }
    }
    return frames;
}
