#pragma once

#include "Components.h"
#include <memory>
#include <string>

class EntityManager;

class Entity
{
    friend class EntityManager;

    bool        m_active = true;
    size_t      m_id     = 0;
    std::string m_tag    = "default";

    // constructor and destructor
    Entity(const size_t id, const std::string& tag)
        : m_id(id), m_tag(tag) {}

public:

    // component pointers
    std::unique_ptr<CTransform> cTransform;
    std::unique_ptr<CShape>     cShape;
    std::unique_ptr<CCollision> cCollision;
    std::unique_ptr<CInput>     cInput;
    std::unique_ptr<CScore>     cScore;
    std::unique_ptr<CLifespan>  cLifespan;
    std::unique_ptr<CBounds>    cBounds;

    // private member access functions
    bool isActive() const {return m_active;}
    const std::string & tag() const {return m_tag;}
    const size_t id() const {return m_id;}
    void destroy() {m_active = false;}

    void addTransform(float x=0, float y=0, float vx=0, float vy=0, float angle=0) {
        cTransform = std::make_unique<CTransform>(Vec2(x,y), Vec2(vx,vy), angle);
    }
};
