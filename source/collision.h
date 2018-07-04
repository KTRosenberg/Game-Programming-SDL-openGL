#ifndef COLLISION_H
#define COLLISION_H

#include "common_utils.h"
#include "common_utils_cpp.hpp"
#include "core_utils.h"
#include "opengl.hpp"

//typedef CollisionStatus (*Fn_CollisionHandler)(glm::vec3 incoming);

struct Collider {
    glm::vec3 a;
    glm::vec3 b;
    //Fn_CollisionHandler handler;
};

void Collider_print(Collider* c);

// x, y, z, collided
struct CollisionStatus {
    Collider* collider;

    glm::vec3 intersection;

    inline bool collided(void)
    {
        return this->collider != nullptr;
    }
};

void CollisionStatus_init(CollisionStatus* out);

#define MAX_COLLIDERS (2048)
extern Buffer<Collider, MAX_COLLIDERS> collision_map; 



glm::vec3 temp_test_collision(Player* you, Collider* c);

#endif

#ifdef COLLISION_IMPLEMENTATION

void CollisionStatus_init(CollisionStatus* out)
{
    out->collider = nullptr;
    out->intersection = glm::vec3(POSITIVE_INFINITY);
}

Buffer<Collider, MAX_COLLIDERS> collision_map;


void Collider_print(Collider* c)
{
    printf("[[%f, %f, %f][%f, %f, %f]]", c->a.x, c->a.y, c->a.z, c->b.x, c->b.y, c->b.z);  
}


#undef COLLISION_IMPLEMENTATION
#endif
