#pragma once

#include "Entity.h"
#include <vector>
#include <map>
#include <memory>
#include <string>

using EntityVec = std::vector<std::shared_ptr<Entity>>;
using EntityMap = std::map<std::string, EntityVec>;

class EntityManager
{
private:
    EntityVec m_entities;
    EntityVec m_entitiesToAdd;
    EntityMap m_entityMap;
    size_t    m_totalEntities = 0;

    void removeDeadEntities(EntityVec & vec);

public:
    EntityManager();

    void update();

    void removeDead() { removeDeadEntities(m_entities); }

    std::shared_ptr<Entity> addEntity(const std::string & tag);

    const EntityVec & getEntities() const;
    const EntityVec & getEntities(const std::string & tag) const;
};
