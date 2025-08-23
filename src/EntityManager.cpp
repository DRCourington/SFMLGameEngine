#include "../include/EntityManager.h"

EntityManager::EntityManager() {}

void EntityManager::update()
{
    for (auto &e : m_entitiesToAdd)
        m_entities.push_back(e), m_entityMap[e->tag()].push_back(e);
    m_entitiesToAdd.clear();

    for (auto & [tag, vec] : m_entityMap)
        removeDeadEntities(vec);
    removeDeadEntities(m_entities);
}

void EntityManager::removeDeadEntities(EntityVec &vec)
{
    vec.erase(std::remove_if(vec.begin(), vec.end(),
                             [](const std::shared_ptr<Entity> &e){ return !e->isActive(); }),
              vec.end());
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string &tag)
{
    auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
    m_entitiesToAdd.push_back(entity);
    return entity;
}

const EntityVec &EntityManager::getEntities() const { return m_entities; }

const EntityVec &EntityManager::getEntities(const std::string &tag) const
{
    auto it = m_entityMap.find(tag);
    if (it != m_entityMap.end()) return it->second;
    static EntityVec emptyVec;
    return emptyVec;
}
