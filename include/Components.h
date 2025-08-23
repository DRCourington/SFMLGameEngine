#pragma once

#include "Vec2.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class CTransform
{
public:
    Vec2 pos      = { 0.0, 0.0 };
    Vec2 velocity = { 0.0, 0.0 };
    float angle   = 0;
    Vec2 scale    = {1.0f, 1.0f};

    CTransform(const Vec2& p, const Vec2& v, float a, const Vec2& s = {1.0f, 1.0f})
        : pos(p), velocity(v), angle(a), scale(s) {}
};

class CShape
{
public:
    std::unique_ptr<sf::Shape> shape;

    CShape(float radius, int points, const sf::Color& fill, const sf::Color& outline, float thickness)
    {
        auto circle = std::make_unique<sf::CircleShape>(radius, points);
        circle->setFillColor(fill);
        circle->setOutlineColor(outline);
        circle->setOutlineThickness(thickness);
        circle->setOrigin(sf::Vector2f(radius, radius));
        shape = std::move(circle);
    }

    CShape(const Vec2& size, const sf::Color& fill, const sf::Color& outline, float thickness)
    {
        auto rect = std::make_unique<sf::RectangleShape>(sf::Vector2f(size.x, size.y));
        rect->setFillColor(fill);
        rect->setOutlineColor(outline);
        rect->setOutlineThickness(thickness);
        rect->setOrigin(sf::Vector2f{size.x / 2.0f, size.y / 2.0f});
        shape = std::move(rect);
    }

    CShape(const std::vector<Vec2>& points, const sf::Color& fill, const sf::Color& outline, float thickness)
    {
        auto polygon = std::make_unique<sf::ConvexShape>(points.size());
        for (size_t i=0; i < points.size(); i++)
        {
            polygon->setPoint(i, sf::Vector2f(points[i].x, points[i].y));
        }
        polygon->setFillColor(fill);
        polygon->setOutlineColor(outline);
        polygon->setOutlineThickness(thickness);

        if (!points.empty())
        {
            sf::Vector2f centroid(0.f, 0.f);
            for (const auto& p : points)
            {
                centroid.x += p.x;
                centroid.y += p.y;
            }
            centroid.x /= points.size();
            centroid.y /= points.size();
            polygon->setOrigin(centroid);
        }

        shape = std::move(polygon);
    }

    void draw(sf::RenderTarget& target) const
    {
        if (shape) target.draw(*shape);
    }

    void setPosition(const Vec2& pos)      { shape->setPosition(sf::Vector2f{pos.x, pos.y}); }
    void move(const Vec2& delta)           { shape->move(sf::Vector2f{delta.x, delta.y}); }
    void setRotation(float angleDegrees)   { shape->setRotation(sf::degrees(angleDegrees)); }
    void rotate(float deltaDegrees)        { shape->rotate(sf::degrees(deltaDegrees)); }
    void setScale(const Vec2& factors)     { shape->setScale(sf::Vector2f{factors.x, factors.y}); }
    void scale(const Vec2& factors)        { shape->scale(sf::Vector2f{factors.x, factors.y}); }

    Vec2 getPosition() const               { auto p = shape->getPosition(); return {p.x, p.y}; }
    float getRotation() const              { return shape->getRotation().asDegrees(); }
    Vec2 getScale() const                  { auto s = shape->getScale(); return {s.x, s.y}; }

    sf::CircleShape*          asCircle()          { return dynamic_cast<sf::CircleShape*>(shape.get()); }
    const sf::CircleShape*    asCircle() const    { return dynamic_cast<const sf::CircleShape*>(shape.get()); }
    sf::RectangleShape*       asRectangle()       { return dynamic_cast<sf::RectangleShape*>(shape.get()); }
    const sf::RectangleShape* asRectangle() const { return dynamic_cast<const sf::RectangleShape*>(shape.get()); }
    sf::ConvexShape*          asPolygon()         { return dynamic_cast<sf::ConvexShape*>(shape.get()); }
    const sf::ConvexShape*    asPolygon() const   { return dynamic_cast<const sf::ConvexShape*>(shape.get()); }
};

enum class CollisionType { Circle, AABB, OBB };

class CCollision
{
public:
    CollisionType type;
    float radius = 0.f;
    Vec2 halfSize = {0, 0};



    CCollision(float r)
        : type(CollisionType::Circle), radius(r) {}

    CCollision(const Vec2& hs, bool oriented = false)
        : type(oriented ? CollisionType::OBB : CollisionType::AABB), halfSize(hs) {}

    CCollision(const CShape& cshape)
    {
        if (auto circle = cshape.asCircle())
        {
            type = CollisionType::Circle;
            radius = circle->getRadius();
        }
        else if (auto rect = cshape.asRectangle())
        {
            type = CollisionType::AABB;
            auto size = rect->getSize();
            halfSize = { size.x / 2.0f, size.y / 2.0f };
        }
        else if (auto poly = cshape.asPolygon())
        {
            type = CollisionType::OBB;
            auto bounds = poly->getLocalBounds();
            auto size = bounds.size;
            halfSize = { size.x / 2.0f, size.y / 2.0f };
        }
    }
};

class CScore
{
public:
    int score = 0;
    CScore(int s)
        : score(s) {}
};

class CLifespan
{
public:
    float remaining = 0;
    float total     = 0;
    CLifespan(float totalSeconds)
        : remaining(totalSeconds), total(totalSeconds) {}
};

class CInput
{
public:
    bool up    = false;
    bool left  = false;
    bool right = false;
    bool down  = false;
    bool shoot = false;

    CInput() {}
};

class CBounds
{
public:
    float minX = 0;
    float minY = 0;
    float maxX = 0;
    float maxY = 0;
    bool killOutOfBounds = false;

    CBounds(float xMin, float yMin, float xMax, float yMax, bool kill = false)
        : minX(xMin), minY(yMin), maxX(xMax), maxY(yMax), killOutOfBounds(kill) {}
};
