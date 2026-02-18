#include "physics.h"
#include <iostream>
#include <cstring>

PhysicsWorld::PhysicsWorld(float gravity_x, float gravity_y) {
    b2Vec2 gravity(gravity_x, gravity_y);
    b2_world = new b2World(gravity);
}

PhysicsWorld::~PhysicsWorld() {
    delete b2_world;
}

b2Body* PhysicsWorld::createBoxBody(float x, float y, float width, float height, float density, 
                                   b2BodyType type, const std::string &name) {
    b2BodyDef bodyDef;
    bodyDef.type = type;
    bodyDef.position.Set(x, y);
    bodyDef.linearDamping = 0.3f;
    
    b2Body *b2_body = b2_world->CreateBody(&bodyDef);
    
    // Create box shape
    b2PolygonShape shape;
    shape.SetAsBox(width / 2.0f, height / 2.0f);
    
    // Create fixture
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = density;
    fixtureDef.friction = 0.3f;
    fixtureDef.restitution = 0.0f;
    
    b2_body->CreateFixture(&fixtureDef);
    
    bodies.push_back(b2_body);
    body_names.push_back(name);
    
    return b2_body;
}

b2Body* PhysicsWorld::getBody(const std::string &name) {
    for (size_t i = 0; i < body_names.size(); i++) {
        if (body_names[i] == name) {
            return bodies[i];
        }
    }
    return nullptr;
}

void PhysicsWorld::step(float timestep) {
    const int32 velocityIterations = 6;
    const int32 positionIterations = 2;
    b2_world->Step(timestep, velocityIterations, positionIterations);
}

// C API wrappers
CPhysicsWorld* physics_create_world(float gravity_x, float gravity_y) {
    return new PhysicsWorld(gravity_x, gravity_y);
}

void physics_destroy_world(CPhysicsWorld *world) {
    delete world;
}

void physics_step_world(CPhysicsWorld *world, float timestep) {
    if (world) {
        world->step(timestep);
    }
}

PhysicsBody* physics_create_box_body(CPhysicsWorld *world, float x, float y, float width, float height,
                                     float density, PhysicsBodyType type, const char *name) {
    if (!world) return nullptr;
    return world->createBoxBody(x, y, width, height, density, (b2BodyType)type, name);
}

PhysicsBody* physics_get_body(CPhysicsWorld *world, const char *name) {
    if (!world || !name) return nullptr;
    return world->getBody(name);
}

void physics_destroy_body(CPhysicsWorld *world, PhysicsBody *body) {
    if (!world || !body) return;
    world->getB2World()->DestroyBody(body);
}

void physics_apply_force(PhysicsBody *body, float fx, float fy) {
    if (!body) return;
    b2Vec2 force(fx, fy);
    body->ApplyForceToCenter(force, true);
}

void physics_apply_impulse(PhysicsBody *body, float ix, float iy) {
    if (!body) return;
    b2Vec2 impulse(ix, iy);
    body->ApplyLinearImpulseToCenter(impulse, true);
}

void physics_set_velocity(PhysicsBody *body, float vx, float vy) {
    if (!body) return;
    b2Vec2 velocity(vx, vy);
    body->SetLinearVelocity(velocity);
}
