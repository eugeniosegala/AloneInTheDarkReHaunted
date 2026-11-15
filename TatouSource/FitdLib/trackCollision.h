#pragma once

#include <algorithm>
#include <cstdint>
#include <limits>
#include <vector>

namespace trackCollision
{
struct Box
{
    int x1, x2, y1, y2, z1, z2;
};

inline bool overlaps(const Box& a, const Box& b)
{
    return a.x1 < b.x2 && b.x1 < a.x2 && a.y1 < b.y2 && b.y1 < a.y2 &&
           a.z1 < b.z2 && b.z1 < a.z2;
}

inline Box translated(Box box, int x, int z)
{
    box.x1 += x; box.x2 += x;
    box.z1 += z; box.z2 += z;
    return box;
}

// A scripted waypoint can be close enough for GOTO while a large creature's
// volume still crosses the window sill. Find a nearby clear *goal*, allowing
// normal animation to carry the creature there; never relocate the actor.
inline bool findClearExit(const Box& atGoal, const std::vector<Box>& solids,
                          int maxShift, int& offsetX, int& offsetZ)
{
    std::vector<int> xs{0}, zs{0};
    for (const auto& solid : solids)
    {
        if (atGoal.y1 >= solid.y2 || solid.y1 >= atGoal.y2) continue;
        const int xOffsets[] = {solid.x1 - atGoal.x2 - 1, solid.x2 - atGoal.x1 + 1};
        const int zOffsets[] = {solid.z1 - atGoal.z2 - 1, solid.z2 - atGoal.z1 + 1};
        for (int x : xOffsets) if (x >= -maxShift && x <= maxShift) xs.push_back(x);
        for (int z : zOffsets) if (z >= -maxShift && z <= maxShift) zs.push_back(z);
    }
    std::sort(xs.begin(), xs.end()); xs.erase(std::unique(xs.begin(), xs.end()), xs.end());
    std::sort(zs.begin(), zs.end()); zs.erase(std::unique(zs.begin(), zs.end()), zs.end());
    std::int64_t best = std::numeric_limits<std::int64_t>::max();
    for (int x : xs) for (int z : zs)
    {
        const std::int64_t distance = std::int64_t(x) * x + std::int64_t(z) * z;
        if (distance >= best || distance > std::int64_t(maxShift) * maxShift) continue;
        const Box candidate = translated(atGoal, x, z);
        if (std::any_of(solids.begin(), solids.end(), [&](const Box& solid) { return overlaps(candidate, solid); }))
            continue;
        best = distance;
        offsetX = x; offsetZ = z;
    }
    return best != std::numeric_limits<std::int64_t>::max();
}
}
