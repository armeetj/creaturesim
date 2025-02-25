#pragma once
#include "raylib.h"
#include "Food.h"
#include "names.h"
#include <vector>
#include <string>

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
    
    void Update(float deltaTime, const std::vector<Creature>& others, std::vector<Food>& foods);
    void Draw() const;
    bool IsAlive() const { return health > 0; }
    Vector2 GetPosition() const { return position; }
    const std::string& GetName() const { return name; }
    float GetHealth() const { return health; }
    float GetEnergy() const { return energy; }
    
private:
    Vector2 position;
    Vector2 velocity;
    float rotation;    // Facing direction in degrees
    float size;        // Affects strength and visibility
    float health;
    float energy;
    float age;
    CreatureState state;
    Color color;
    
    // Traits
    std::string name;   // Creature's unique name
    bool isMale;
    float strength;     // Affects fighting success (0-100)
    float speed;        // Affects movement speed (0.5-1.5)
    float metabolism;   // Affects energy consumption rate (0.5-1.5)
    
    void UpdateState(const std::vector<Creature>& others);
    void UpdateMovement(float deltaTime);
    void UpdateColor();
    Color GetStateColor() const;
};
