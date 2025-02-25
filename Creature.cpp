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
    , color(GREEN)
    , isMale(GetRandomValue(0, 1) == 1)
    , strength(GetRandomValue(40, 100))
    , speed((float)GetRandomValue(50, 150) / 100.0f)
    , metabolism((float)GetRandomValue(50, 150) / 100.0f) {
}

void Creature::Update(float deltaTime, const std::vector<Creature>& others) {
    age += deltaTime;
    energy -= deltaTime * 2 * metabolism; // Consume energy based on metabolism
    
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
    float maxSpeed = 5.0f * speed; // Use speed trait
    if (speed > maxSpeed) {
        velocity.x = (velocity.x / speed) * maxSpeed;
        velocity.y = (velocity.y / speed) * maxSpeed;
    }
    
    position.x += velocity.x * deltaTime * 60.0f * speed;
    position.y += velocity.y * deltaTime * 60.0f * speed;
    
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
    // Draw creature body
    DrawPoly(position, isMale ? 3 : 6, size, 0, color);
    
    // Draw health bar
    DrawRectangle(position.x - size, position.y - size - 10, 
                  size * 2 * (health/100.0f), 4, RED);
    
    // Draw energy bar
    DrawRectangle(position.x - size, position.y - size - 6, 
                  size * 2 * (energy/100.0f), 4, YELLOW);
    
    // Draw strength indicator (outline thickness)
    DrawPolyLines(position, isMale ? 3 : 6, size, 0, 
                  ColorAlpha(WHITE, strength/100.0f));
}
