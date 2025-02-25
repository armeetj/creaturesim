#include "Creature.h"
#include <cmath>

#include "Constants.h"

Creature::Creature(Vector2 pos, float size) 
    : position(pos)
    , velocity({0, 0})
    , rotation(0.0f)
    , size(size)
    , health(Constants::INITIAL_HEALTH)
    , energy(Constants::INITIAL_ENERGY)
    , age(0)
    , state(CreatureState::WANDERING)
    , color(GREEN)
    , name(Names::generate_name())
    , isMale(GetRandomValue(0, 1) == 1)
    , strength(GetRandomValue(Constants::MIN_STRENGTH, Constants::MAX_STRENGTH))
    , speed((float)GetRandomValue(Constants::MIN_SPEED * 100, Constants::MAX_SPEED * 100) / 100.0f)
    , metabolism((float)GetRandomValue(Constants::MIN_METABOLISM * 100, Constants::MAX_METABOLISM * 100) / 100.0f) {
}

void Creature::Update(float deltaTime, const std::vector<Creature>& others, std::vector<Food>& foods) {
    age += deltaTime;
    energy -= deltaTime * Constants::ENERGY_CONSUMPTION_RATE * metabolism;
    
    if (energy < 0) {
        health -= deltaTime * Constants::HEALTH_DECAY_RATE;
    }
    
    // Try to eat if hungry
    if (state == CreatureState::HUNTING || state == CreatureState::EATING) {
        bool foundFood = false;
        Vector2 nearestFoodPos = {0, 0};
        float nearestDist = INFINITY;

        // Find nearest food
        for (auto& food : foods) {
            if (!food.IsConsumed()) {
                Vector2 foodPos = food.GetPosition();
                float dx = position.x - foodPos.x;
                float dy = position.y - foodPos.y;
                float distance = sqrt(dx*dx + dy*dy);
                
                if (distance < nearestDist) {
                    nearestDist = distance;
                    nearestFoodPos = foodPos;
                    foundFood = true;
                }
                
                // Check if we're close enough to eat
                if (distance < (size + Food::SIZE) * 0.5f) {
                    food.Consume();
                    energy += Constants::FOOD_ENERGY_VALUE;
                    if (energy > Constants::INITIAL_ENERGY) {
                        energy = Constants::INITIAL_ENERGY;
                    }
                    state = CreatureState::EATING;
                    // Stop moving while eating
                    velocity = {0, 0};
                    break;
                }
            }
        }

        // Move towards nearest food if hunting
        if (state == CreatureState::HUNTING && foundFood) {
            float dx = nearestFoodPos.x - position.x;
            float dy = nearestFoodPos.y - position.y;
            float dist = sqrt(dx*dx + dy*dy);
            if (dist > 0) {
                velocity.x += (dx/dist) * Constants::FOOD_SEEK_FORCE;
                velocity.y += (dy/dist) * Constants::FOOD_SEEK_FORCE;
            }
        }
    }

    UpdateState(others);
    UpdateMovement(deltaTime);
    UpdateColor();
}

void Creature::UpdateState(const std::vector<Creature>& others) {
    // Priority-based state machine
    if (state == CreatureState::EATING) {
        // Stay in eating state for a short duration
        static float eatTimer = 0;
        eatTimer += GetFrameTime();
        if (eatTimer > 1.0f) {
            eatTimer = 0;
            state = CreatureState::WANDERING;
        }
    } else if (energy < Constants::HUNGRY_THRESHOLD) {
        // Hunting is highest priority when hungry
        state = CreatureState::HUNTING;
    } else if (health < Constants::CRITICAL_HEALTH) {
        // Very low health is an emergency
        state = CreatureState::SICK;
    } else if (energy > Constants::MATING_ENERGY && age > Constants::MATING_AGE) {
        // Mating is high priority when healthy and mature
        state = CreatureState::MATING;
    } else if (health < Constants::LOW_HEALTH) {
        // Rest when somewhat injured
        state = CreatureState::SICK;
    } else {
        state = CreatureState::WANDERING;
    }
}

void Creature::UpdateMovement(float deltaTime) {
    // Don't move while eating
    if (state == CreatureState::EATING) {
        velocity = {0, 0};
        return;
    }
    
    if (state != CreatureState::HUNTING) {
        // Normal random movement
        velocity.x += (float)GetRandomValue(-20, 20) / 100.0f;
        velocity.y += (float)GetRandomValue(-20, 20) / 100.0f;
    }
    
    // Limit velocity
    float speed = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    if (speed > Constants::MAX_VELOCITY) {
        velocity.x = (velocity.x / speed) * Constants::MAX_VELOCITY;
        velocity.y = (velocity.y / speed) * Constants::MAX_VELOCITY;
    }
    
    // Update position
    position.x += velocity.x * deltaTime * Constants::BASE_MOVEMENT_SPEED * speed;
    position.y += velocity.y * deltaTime * Constants::BASE_MOVEMENT_SPEED * speed;
    
    // Update rotation to face movement direction
    if (speed > 0.1f) {  // Only update rotation if moving significantly
        rotation = atan2f(velocity.y, velocity.x) * RAD2DEG;
    }
    
    // Bounce off boundaries
    if (position.x < 0) {
        position.x = 0;
        velocity.x *= Constants::BOUNDARY_BOUNCE;
    }
    if (position.x > GetScreenWidth() - size) {
        position.x = GetScreenWidth() - size;
        velocity.x *= -0.8f;
    }
    if (position.y < 0) {
        position.y = 0;
        velocity.y *= -0.8f;
    }
    if (position.y > GetScreenHeight() - size) {
        position.y = GetScreenHeight() - size;
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
    
    DrawText(TextFormat("%s\n(%s)", name.c_str(), stateText),
             position.x - size, position.y - size - 40, 10, WHITE);

    // Draw creature body
    DrawPoly(position, isMale ? 3 : 6, size, rotation + 90.0f, color);
    
    // Draw health bar and value
    DrawRectangle(position.x - size, position.y - size - 10, 
                  size * 2 * (health/100.0f), 4, RED);
    DrawText(TextFormat("H:%.0f", health),
             position.x - size - 35, position.y - size - 10, 6, RED);
    
    // Draw energy bar and value
    DrawRectangle(position.x - size, position.y - size - 6, 
                  size * 2 * (energy/100.0f), 4, YELLOW);
    DrawText(TextFormat("E:%.0f", energy),
             position.x + size * 2 - 2, position.y - size - 10, 6, YELLOW);

    
    // Draw strength indicator (outline thickness)
    DrawPolyLines(position, isMale ? 3 : 6, size, rotation + 90.0f, 
                  ColorAlpha(WHITE, strength/100.0f));
                  
    // Draw selection indicator
    if (selected) {
        DrawCircleLines(position.x, position.y, size * 1.5f, WHITE);

        // Draw attributes in smaller text with colors
        DrawText(TextFormat("st:%.0f", strength), position.x - size,
                 position.y + size + 2, 1, ORANGE);
        DrawText(TextFormat("\nsp:%.1f", speed), position.x - size,
                 position.y + size + 2, 1, SKYBLUE);
        DrawText(TextFormat("\n\nmt:%.1f", metabolism), position.x - size,
                 position.y + size + 2, 1, GREEN);
    }
}
