#pragma once
#include <string>
#include <functional>
#include <map>

enum class ActionType { Start, End };

using ActionMap = std::map<int, std::string>;

class Action {
    std::string m_name;
    ActionType  m_type;

public:
    Action(std::string name, ActionType type) : m_name(std::move(name)), m_type(type) {}

    const std::string& name() const noexcept { return m_name; }
    ActionType         type() const noexcept { return m_type; }

    bool isStart() const noexcept { return m_type == ActionType::Start; }
    bool isEnd()   const noexcept { return m_type == ActionType::End; }

    // comparisons for containers/tests
    bool operator==(const Action& rhs) const noexcept {
        return m_type == rhs.m_type && m_name == rhs.m_name;
    }
    bool operator!=(const Action& rhs) const noexcept { return !(*this == rhs); }
};

// allow use as key in unordered containers
namespace std {
template<>
struct hash<Action> {
    size_t operator()(const Action& a) const noexcept {
        // simple combine: hash(name) ^ (type<<1)
        return std::hash<std::string>{}(a.name()) ^ (static_cast<size_t>(a.type()) << 1);
    }
};
}
