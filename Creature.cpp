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
    , speed((float)GetRandomValue(50, 120) / 100.0f) // Cap max speed at 1.2
    , metabolism((float)GetRandomValue(50, 150) / 100.0f) {
}

void Creature::Update(float deltaTime, const std::vector<Creature>& others, std::vector<Food>& foods) {
    age += deltaTime;
    energy -= deltaTime * 2 * metabolism; // Consume energy based on metabolism
    
    if (energy < 0) {
        health -= deltaTime * 5;
    }
    
    // Try to eat if hungry
    if (state == CreatureState::HUNTING) {
        for (auto& food : foods) {
            if (!food.IsConsumed()) {
                Vector2 foodPos = food.GetPosition();
                float dx = position.x - foodPos.x;
                float dy = position.y - foodPos.y;
                float distance = sqrt(dx*dx + dy*dy);
                
                if (distance < size * 2) {
                    food.Consume();
                    energy += 30;
                    if (energy > 100) energy = 100;
                    state = CreatureState::EATING;
                    break;
                }
            }
        }
    }

    UpdateState(others);
    UpdateMovement(deltaTime);
    UpdateColor();
}

void Creature::UpdateState(const std::vector<Creature>& others) {
    // Priority-based state machine
    if (energy < 30 || state == CreatureState::EATING) {
        // Eating is highest priority when hungry
        state = CreatureState::HUNTING;
    } else if (health < 40) {
        // Very low health is an emergency
        state = CreatureState::SICK;
    } else if (energy > 60 && age > 10) {
        // Mating is high priority when healthy and mature
        state = CreatureState::MATING;
    } else if (health < 70) {
        // Rest when somewhat injured
        state = CreatureState::SICK;
    } else {
        state = CreatureState::WANDERING;
    }
}

void Creature::UpdateMovement(float deltaTime) {
    if (state == CreatureState::HUNTING) {
        // More directed movement when hunting
        velocity.x += (float)GetRandomValue(-10, 10) / 100.0f;
        velocity.y += (float)GetRandomValue(-10, 10) / 100.0f;
    } else {
        // Normal random movement
        velocity.x += (float)GetRandomValue(-20, 20) / 100.0f;
        velocity.y += (float)GetRandomValue(-20, 20) / 100.0f;
    }
    
    // Limit velocity
    float speed = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    float maxSpeed = 2.0f * speed; // Use speed trait
    if (speed > maxSpeed) {
        velocity.x = (velocity.x / speed) * maxSpeed;
        velocity.y = (velocity.y / speed) * maxSpeed;
    }
    
    position.x += velocity.x * deltaTime * 30.0f * speed;
    position.y += velocity.y * deltaTime * 30.0f * speed;
    
    // Bounce off boundaries
    if (position.x < 0) {
        position.x = 0;
        velocity.x *= -0.8f;
    }
    if (position.x > GetScreenWidth()) {
        position.x = GetScreenWidth();
        velocity.x *= -0.8f;
    }
    if (position.y < 0) {
        position.y = 0;
        velocity.y *= -0.8f;
    }
    if (position.y > GetScreenHeight()) {
        position.y = GetScreenHeight();
        velocity.y *= -0.8f;
    }
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
    // Draw status text
    const char* stateText;
    switch (state) {
        case CreatureState::WANDERING: stateText = "Wandering"; break;
        case CreatureState::HUNTING: stateText = "Hunting"; break;
        case CreatureState::MATING: stateText = "Mating"; break;
        case CreatureState::FIGHTING: stateText = "Fighting"; break;
        case CreatureState::EATING: stateText = "Eating"; break;
        case CreatureState::SICK: stateText = "Sick"; break;
        default: stateText = "Unknown";
    }
    
    DrawText(TextFormat("%s %s\nE:%.0f H:%.0f", 
             isMale ? "♂" : "♀", stateText, 
             energy, health),
             position.x - size, position.y - size - 30, 10, WHITE);

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
