#pragma once
#include "raylib.h"
#include <vector>

enum class CreatureState {
    WANDERING,
    HUNTING,
    MATING,
    FIGHTING,
    EATING,
    SICK
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
    
    // New traits
    bool isMale;
    float strength;     // Affects fighting success
    float speed;        // Affects movement speed
    float size;        // Already existed but now affects strength
    float metabolism;   // Affects energy consumption rate
    
    void UpdateState(const std::vector<Creature>& others);
    void UpdateMovement(float deltaTime);
    void UpdateColor();
    Color GetStateColor() const;
};
