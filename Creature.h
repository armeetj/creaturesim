#pragma once
#include "raylib.h"
#include <vector>

enum class CreatureState {
    WANDERING,
    HUNTING,
    MATING,
    SICK,
    EATING
};

class Creature {
public:
    Creature(Vector2 pos, float size);
    
    void Update(float deltaTime, const std::vector<Creature>& others);
    void Draw() const;
    bool IsAlive() const { return health > 0; }
    
private:
    Vector2 position;
    Vector2 velocity;
    float size;
    float health;
    float energy;
    float age;
    CreatureState state;
    Color color;
    
    void UpdateState(const std::vector<Creature>& others);
    void UpdateMovement(float deltaTime);
    void UpdateColor();
    Color GetStateColor() const;
};
