#include "../include/Entity.h"

Entity::Entity(std::size_t id, const std::string& tag)
: m_id(id)
, m_tag(tag)
, m_components{}
{}

bool Entity::isActive() const { return m_active; }
const std::string& Entity::tag() const { return m_tag; }
