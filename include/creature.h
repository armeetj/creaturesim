#pragma once
#include "food.h"
#include "raylib.h"
#include <vector>

enum class CreatureState {
  WANDERING,
  HUNTING,
  MATING,
  FIGHTING,
  EATING,
  SICK,
};

class Creature {
public:
  Creature(Vector2 pos, float size);
  void Update(float deltaTime, const std::vector<Creature> &others,
              std::vector<Food> &foods, std::vector<Creature> &allCreatures );
  void Draw(int rank = 0, const std::vector<Creature> &allCreatures =
                              std::vector<Creature>()) const;
  bool IsAlive() const { return health > 0; }
  Vector2 GetPosition() const { return position; }
  const std::string &GetName() const { return name; }
  float GetHealth() const { return health; }
  float GetEnergy() const { return energy; }
  void SetSelected(bool select) { selected = select; }
  bool IsSelected() const { return selected; }
  float GetAge() const { return age; }
  float GetStrength() const { return strength; }
  float GetSpeed() const { return speed; }
  float GetMetabolism() const { return metabolism; }
  bool IsMale() const { return isMale; }
  void Fight(Creature &opponent);
  float GetFightProbability(const Creature &opponent) const;

private:
  Creature *lastFightOpponent = nullptr;
  float timeSinceLastFight = 0.0f;
  Vector2 position;
  Vector2 velocity;
  float rotation; // Facing direction in degrees
  float size;     // Affects strength and visibility
  float health;
  float energy;
  float age;
  CreatureState state;
  Color color;

  // Traits
  std::string name; // Creature's unique name
  bool isMale;
  float strength;   // Affects fighting success (0-100)
  float speed;      // Affects movement speed (0.5-1.5)
  float metabolism; // Affects energy consumption rate (0.5-1.5)
  bool selected = false;

  void UpdateState(const std::vector<Creature> &others,
                   std::vector<Food> &foods,
                   std::vector<Creature> &allCreatures);
  void UpdateMovement(float deltaTime, const std::vector<Creature> &others);
  void UpdateColor();
  Color GetStateColor() const;
};
