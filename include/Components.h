#pragma once

#include "Vec2.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class CTransform {
public:
    Vec2  pos      {0.f, 0.f};
    Vec2  velocity {0.f, 0.f};
    float angle = 0.f;
    Vec2  scale    {1.f, 1.f};

    CTransform(const Vec2& p, const Vec2& v, float a, const Vec2& s = {1.f, 1.f})
        : pos(p), velocity(v), angle(a), scale(s) {}
};

class CShape {
public:
    std::unique_ptr<sf::Shape> shape;

    CShape(float radius, int points, const sf::Color& fill, const sf::Color& outline, float thickness) {
        auto circle = std::make_unique<sf::CircleShape>(radius, points);
        circle->setFillColor(fill);
        circle->setOutlineColor(outline);
        circle->setOutlineThickness(thickness);
        circle->setOrigin({radius, radius});
        shape = std::move(circle);
    }

    CShape(const Vec2& size, const sf::Color& fill, const sf::Color& outline, float thickness) {
        auto rect = std::make_unique<sf::RectangleShape>(sf::Vector2f(size.x, size.y));
        rect->setFillColor(fill);
        rect->setOutlineColor(outline);
        rect->setOutlineThickness(thickness);
        rect->setOrigin({size.x * 0.5f, size.y * 0.5f});
        shape = std::move(rect);
    }

    CShape(const std::vector<Vec2>& pts, const sf::Color& fill, const sf::Color& outline, float thickness) {
        auto poly = std::make_unique<sf::ConvexShape>(pts.size());
        for (size_t i = 0; i < pts.size(); ++i) poly->setPoint(i, {pts[i].x, pts[i].y});
        poly->setFillColor(fill);
        poly->setOutlineColor(outline);
        poly->setOutlineThickness(thickness);
        sf::Vector2f c{0.f, 0.f};
        for (auto& p : pts) { c.x += p.x; c.y += p.y; }
        if (!pts.empty()) { c.x /= pts.size(); c.y /= pts.size(); }
        poly->setOrigin(c);
        shape = std::move(poly);
    }

    void  draw(sf::RenderTarget& target) const            { if (shape) target.draw(*shape); }
    void  setPosition(const Vec2& p)                      { shape->setPosition({p.x, p.y}); }
    void  move(const Vec2& d)                             { shape->move({d.x, d.y}); }
    void  setRotation(float deg)                          { shape->setRotation(sf::degrees(deg)); }
    void  rotate(float ddeg)                              { shape->rotate(sf::degrees(ddeg)); }
    void  setScale(const Vec2& s)                         { shape->setScale({s.x, s.y}); }
    void  scale(const Vec2& s)                            { shape->scale({s.x, s.y}); }
    Vec2  getPosition() const                             { auto p = shape->getPosition(); return {p.x, p.y}; }
    float getRotation() const                             { return shape->getRotation().asDegrees(); }
    Vec2  getScale() const                                { auto s = shape->getScale(); return {s.x, s.y}; }

    sf::CircleShape*          asCircle()                  { return dynamic_cast<sf::CircleShape*>(shape.get()); }
    const sf::CircleShape*    asCircle() const            { return dynamic_cast<const sf::CircleShape*>(shape.get()); }
    sf::RectangleShape*       asRectangle()               { return dynamic_cast<sf::RectangleShape*>(shape.get()); }
    const sf::RectangleShape* asRectangle() const         { return dynamic_cast<const sf::RectangleShape*>(shape.get()); }
    sf::ConvexShape*          asPolygon()                 { return dynamic_cast<sf::ConvexShape*>(shape.get()); }
    const sf::ConvexShape*    asPolygon() const           { return dynamic_cast<const sf::ConvexShape*>(shape.get()); }
};

enum class CollisionType { Circle, AABB, OBB };

class CCollision {
public:
    CollisionType type = CollisionType::Circle;
    float radius   = 0.f;
    Vec2  halfSize {0.f, 0.f};

    CCollision(float r) : type(CollisionType::Circle), radius(r) {}
    CCollision(const Vec2& hs, bool oriented = false)
        : type(oriented ? CollisionType::OBB : CollisionType::AABB), halfSize(hs) {}

    CCollision(const CShape& cshape) {
        if (auto c = cshape.asCircle()) {
            type = CollisionType::Circle; radius = c->getRadius();
        } else if (auto r = cshape.asRectangle()) {
            type = CollisionType::AABB; auto size = r->getSize(); halfSize = {size.x * 0.5f, size.y * 0.5f};
        } else if (auto poly = cshape.asPolygon()) {
            type = CollisionType::OBB;
            auto bounds = poly->getLocalBounds();
            auto size = bounds.size;
            halfSize = {size.x * 0.5f, size.y * 0.5f};
        }
    }
};

class CScore {
public:
    int score = 0;
    explicit CScore(int s) : score(s) {}
};

class CLifespan {
public:
    float remaining = 0.f;
    float total     = 0.f;
    explicit CLifespan(float totalSeconds) : remaining(totalSeconds), total(totalSeconds) {}
};

class CInput {
public:
    bool up=false, left=false, right=false, down=false, shoot=false;
};

class CBounds {
public:
    float minX = 0.f, minY = 0.f, maxX = 0.f, maxY = 0.f;
    bool  killOutOfBounds = false;
    CBounds(float xMin, float yMin, float xMax, float yMax, bool kill=false)
        : minX(xMin), minY(yMin), maxX(xMax), maxY(yMax), killOutOfBounds(kill) {}
};
