#include "../include/Animation.h"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <algorithm>

namespace {
    sf::Texture& dummyTexture() {
        static sf::Texture tex;
        return tex;
    }
}

Animation::Animation()
: m_sprite(dummyTexture())
{
}

Animation::Animation(const std::string& name, const sf::Texture& t)
: Animation(name, t, 1, 0)
{
}

Animation::Animation(const std::string& name, const sf::Texture& t, size_t frameCount, size_t speed)
: m_sprite(t)
, m_frameCount(frameCount ? frameCount : 1)
, m_currentFrame(0)
, m_speed(speed)
, m_size{ static_cast<float>(t.getSize().x) / static_cast<float>(m_frameCount),
          static_cast<float>(t.getSize().y) }
, m_name(name)
{
    const int w = static_cast<int>(m_size.x);
    const int h = static_cast<int>(m_size.y);
    m_sprite.setTextureRect(sf::IntRect(sf::Vector2i{0, 0}, sf::Vector2i{w, h}));
}

void Animation::update()
{
    const size_t step = std::max<size_t>(1, m_speed ? m_speed : 1);
    m_currentFrame = (m_currentFrame + step) % m_frameCount;

    const int left = static_cast<int>(m_currentFrame * m_size.x);
    const int w = static_cast<int>(m_size.x);
    const int h = static_cast<int>(m_size.y);
    m_sprite.setTextureRect(sf::IntRect(sf::Vector2i{left, 0}, sf::Vector2i{w, h}));
}

bool Animation::hasEnded() const
{
    return m_currentFrame + 1 == m_frameCount;
}

const std::string& Animation::getName() const { return m_name; }
const Vec2&        Animation::getSize() const { return m_size; }
sf::Sprite&        Animation::getSprite()      { return m_sprite; }
