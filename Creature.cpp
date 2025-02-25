#include "Creature.h"
#include <cmath>

Creature::Creature(Vector2 pos, float size) 
    : position(pos)
    , velocity({0, 0})
    , size(size)
    , health(100)
    , energy(100)
    , age(0)
    , state(CreatureState::WANDERING)
    , color(GREEN) {
}

void Creature::Update(float deltaTime, const std::vector<Creature>& others) {
    age += deltaTime;
    energy -= deltaTime * 2; // Consume energy over time
    
    if (energy < 0) {
        health -= deltaTime * 5;
    }
    
    UpdateState(others);
    UpdateMovement(deltaTime);
    UpdateColor();
}

void Creature::UpdateState(const std::vector<Creature>& others) {
    // Simple state machine
    if (energy < 30) {
        state = CreatureState::HUNTING;
    } else if (health < 50) {
        state = CreatureState::SICK;
    } else if (energy > 80 && age > 10) {
        state = CreatureState::MATING;
    } else {
        state = CreatureState::WANDERING;
    }
}

void Creature::UpdateMovement(float deltaTime) {
    // Add random movement
    velocity.x += (float)GetRandomValue(-100, 100) / 100.0f;
    velocity.y += (float)GetRandomValue(-100, 100) / 100.0f;
    
    // Limit velocity
    float speed = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    if (speed > 5.0f) {
        velocity.x = (velocity.x / speed) * 5.0f;
        velocity.y = (velocity.y / speed) * 5.0f;
    }
    
    position.x += velocity.x * deltaTime * 60.0f;
    position.y += velocity.y * deltaTime * 60.0f;
    
    // Keep in bounds
    if (position.x < 0) position.x = 0;
    if (position.x > GetScreenWidth()) position.x = GetScreenWidth();
    if (position.y < 0) position.y = 0;
    if (position.y > GetScreenHeight()) position.y = GetScreenHeight();
}

void Creature::UpdateColor() {
    color = GetStateColor();
}

Color Creature::GetStateColor() const {
    switch (state) {
        case CreatureState::WANDERING: return GREEN;
        case CreatureState::HUNTING: return RED;
        case CreatureState::MATING: return PINK;
        case CreatureState::SICK: return PURPLE;
        case CreatureState::EATING: return ORANGE;
        default: return WHITE;
    }
}

void Creature::Draw() const {
    DrawPoly(position, 6, size, 0, color);
    
    // Draw health bar
    DrawRectangle(position.x - size, position.y - size - 10, 
                  size * 2 * (health/100.0f), 4, RED);
}
