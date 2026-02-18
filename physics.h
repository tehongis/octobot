#ifndef PHYSICS_H
#define PHYSICS_H

#include <box2d/box2d.h>
#include <vector>
#include <string>

class PhysicsWorld {
private:
    b2World *b2_world;
    std::vector<b2Body*> bodies;
    std::vector<std::string> body_names;
    
public:
    PhysicsWorld(float gravity_x, float gravity_y);
    ~PhysicsWorld();
    
    b2Body* createBoxBody(float x, float y, float width, float height, float density, 
                         b2BodyType type, const std::string &name);
    b2Body* getBody(const std::string &name);
    void step(float timestep);
    b2World* getB2World() { return b2_world; }
};

// C API wrappers
typedef PhysicsWorld CPhysicsWorld;
typedef b2Body PhysicsBody;

enum PhysicsBodyType {
    BODY_STATIC = b2_staticBody,
    BODY_DYNAMIC = b2_dynamicBody
};

// C-style interface
CPhysicsWorld* physics_create_world(float gravity_x, float gravity_y);
void physics_destroy_world(CPhysicsWorld *world);
void physics_step_world(CPhysicsWorld *world, float timestep);
PhysicsBody* physics_create_box_body(CPhysicsWorld *world, float x, float y, float width, float height,
                                     float density, PhysicsBodyType type, const char *name);
PhysicsBody* physics_get_body(CPhysicsWorld *world, const char *name);
void physics_destroy_body(CPhysicsWorld *world, PhysicsBody *body);
void physics_apply_force(PhysicsBody *body, float fx, float fy);
void physics_apply_impulse(PhysicsBody *body, float ix, float iy);
void physics_set_velocity(PhysicsBody *body, float vx, float vy);

// Helper to get position
inline void physics_get_body_position(PhysicsBody *body, float &x, float &y) {
    b2Vec2 pos = body->GetPosition();
    x = pos.x;
    y = pos.y;
}

// Helper to get body position as pair
inline b2Vec2 physics_get_position(PhysicsBody *body) {
    return body->GetPosition();
}

#endif // PHYSICS_H
