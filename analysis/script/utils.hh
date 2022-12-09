#pragma once

#include <vector>
#include <functional>

typedef remollGenericDetectorHit_t RemollHit;
typedef std::vector<RemollHit> hit_list;
typedef std::function<bool(RemollHit)> hitfunc;

typedef remollEventParticle_t RemollPart;
typedef std::vector<RemollPart> part_list;
