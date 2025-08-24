#include "../include/Physics.h"
#include "../include/Components.h"
#include "../include/Entity.h"
#include <cmath>

namespace {
    inline Vec2 overlapAt(const Vec2& aPos, const CBoundingBox& aBox,
                          const Vec2& bPos, const CBoundingBox& bBox)
    {
        // centers
        const Vec2 ac = {aPos.x + aBox.offset.x, aPos.y + aBox.offset.y};
        const Vec2 bc = {bPos.x + bBox.offset.x, bPos.y + bBox.offset.y};

        const float ox = (aBox.halfSize.x + bBox.halfSize.x) - std::fabs(ac.x - bc.x);
        const float oy = (aBox.halfSize.y + bBox.halfSize.y) - std::fabs(ac.y - bc.y);

        if (ox <= 0.f || oy <= 0.f) return {0.f, 0.f};
        return {ox, oy};
    }
}

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    if (!a || !b || !a->hasComponent<CBoundingBox>() || !b->hasComponent<CBoundingBox>()) return {0.f, 0.f};

    const auto& at = a->getComponent<CTransform>();
    const auto& bt = b->getComponent<CTransform>();
    const auto& ab = a->getComponent<CBoundingBox>();
    const auto& bb = b->getComponent<CBoundingBox>();

    return overlapAt(at.pos, ab, bt.pos, bb);
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    if (!a || !b || !a->hasComponent<CBoundingBox>() || !b->hasComponent<CBoundingBox>()) return {0.f, 0.f};

    const auto& at = a->getComponent<CTransform>();
    const auto& bt = b->getComponent<CTransform>();
    const auto& ab = a->getComponent<CBoundingBox>();
    const auto& bb = b->getComponent<CBoundingBox>();

    return overlapAt(at.prevPos, ab, bt.prevPos, bb);
}
