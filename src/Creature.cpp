#include "Creature.h"
#include <cmath>

#include "Constants.h"
// No changes needed to implementation

Creature::Creature(Vector2 pos, float size)
    : position(pos), velocity({0, 0}), rotation(0.0f), size(size),
      health(Constants::INITIAL_HEALTH), energy(Constants::INITIAL_ENERGY),
      age(0), state(CreatureState::WANDERING), color(GREEN),
      name(Names::generate_name()), isMale(GetRandomValue(0, 1) == 1),
      strength(
          GetRandomValue(Constants::MIN_STRENGTH, Constants::MAX_STRENGTH)),
      speed((float)GetRandomValue(Constants::MIN_SPEED * 100,
                                  Constants::MAX_SPEED * 100) /
            100.0f),
      metabolism((float)GetRandomValue(Constants::MIN_METABOLISM * 100,
                                       Constants::MAX_METABOLISM * 100) /
                 100.0f) {}

// Helper function
float Clamp(float value, float min, float max) {
  if (value < min)
    return min;
  if (value > max)
    return max;
  return value;
}

void Creature::Update(float deltaTime, const std::vector<Creature> &others,
                      std::vector<Food> &foods,
                      std::vector<Creature> &allCreatures) {
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
    for (auto &food : foods) {
      if (!food.IsConsumed()) {
        Vector2 foodPos = food.GetPosition();
        float dx = position.x - foodPos.x;
        float dy = position.y - foodPos.y;
        float distance = sqrt(dx * dx + dy * dy);

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

          // Grow in size when eating
          size += Constants::FOOD_GROW_SIZE;

          // Adjust speed and strength based on size
          speed =
              Clamp(speed * 0.95f, Constants::MIN_SPEED, Constants::MAX_SPEED);
          strength = Clamp(strength * 1.05f, Constants::MIN_STRENGTH,
                           Constants::MAX_STRENGTH);

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
      float dist = sqrt(dx * dx + dy * dy);
      if (dist > 0) {
        velocity.x += (dx / dist) * Constants::FOOD_SEEK_FORCE;
        velocity.y += (dy / dist) * Constants::FOOD_SEEK_FORCE;
      }
    }
  }

  UpdateState(others, foods, allCreatures); // Pass foods to UpdateState
  UpdateMovement(deltaTime, others);
  UpdateColor();
}

void Creature::UpdateState(const std::vector<Creature> &others,
                           std::vector<Food> &foods,
                           std::vector<Creature> &allCreatures) {
  timeSinceLastFight += GetFrameTime();

  // Priority-based state machine
  if (state == CreatureState::EATING) {
    // Stay in eating state for a very short duration
    static float eatTimer = 0;
    eatTimer += GetFrameTime();
    if (eatTimer > 0.2f) { // Reduced from 1.0f to 0.2f
      eatTimer = 0;
      state = CreatureState::WANDERING;
    }
  } else if (energy < Constants::HUNGRY_THRESHOLD) {
    // Hunting is highest priority when hungry
    bool foundFood = false;

    // First, check for available food
    for (const auto &food : foods) {
      if (!food.IsConsumed()) {
        foundFood = true;
        break;
      }
    }

    // If no food, look for creatures eating
    if (!foundFood) {
      for (const auto &other : others) {
        if (&other != this && other.state == CreatureState::EATING) {
          Vector2 otherPos = other.GetPosition();
          float dx = position.x - otherPos.x;
          float dy = position.y - otherPos.y;
          float dist = sqrt(dx * dx + dy * dy);

          if (dist < size * 2) { // Close enough to fight
            if (GetFightProbability(other) > 0.5f) {
              state = CreatureState::FIGHTING;
              Fight(const_cast<Creature &>(other));

              // If fight is won, simulate getting the food energy
              if (energy < 0) {
                energy += Constants::FOOD_ENERGY_VALUE * 0.8f;
              }

              break;
            }
          }
        }
      }
    }

    // If no successful fight, continue hunting
    if (state != CreatureState::FIGHTING) {
      state = CreatureState::HUNTING;
    }
  } else if (health < Constants::CRITICAL_HEALTH) {
    // Very low health is an emergency
    state = CreatureState::SICK;
  } else if (energy > Constants::MATING_ENERGY && age > Constants::MATING_AGE) {
    // Check for nearby potential mates and competition
    for (const auto &other : others) {
      if (&other != this && other.GetEnergy() > Constants::MATING_ENERGY &&
          other.GetAge() > Constants::MATING_AGE &&
          other.IsMale() != isMale) { // Must be opposite sex

        Vector2 otherPos = other.GetPosition();
        float dx = position.x - otherPos.x;
        float dy = position.y - otherPos.y;
        float dist = sqrt(dx * dx + dy * dy);

        if (dist < size * 3) { // Close enough to compete
          // If another male is nearby, fight for mating rights
          if (!isMale && other.IsMale()) {
            if (GetFightProbability(other) > 0.5f) {
              state = CreatureState::FIGHTING;
              Fight(const_cast<Creature &>(other));
              break;
            }
          }

          // Proceed with mating if no fight occurs
          if (state != CreatureState::FIGHTING) {
            Vector2 newPos = {(position.x + otherPos.x) / 2,
                              (position.y + otherPos.y) / 2};

            // Mix parents' traits with some variation
            float mixStrength = (strength + other.GetStrength()) / 2;
            float mixSpeed = (speed + other.GetSpeed()) / 2;
            float mixMetabolism = (metabolism + other.GetMetabolism()) / 2;

            // Add some random variation (-10% to +10%)
            mixStrength *= (1.0f + (GetRandomValue(-10, 10) / 100.0f));
            mixSpeed *= (1.0f + (GetRandomValue(-10, 10) / 100.0f));
            mixMetabolism *= (1.0f + (GetRandomValue(-10, 10) / 100.0f));

            // Clamp values
            mixStrength = Clamp(mixStrength, Constants::MIN_STRENGTH,
                                Constants::MAX_STRENGTH);
            mixSpeed = Clamp(mixSpeed + 0.1f, Constants::MIN_SPEED,
                             Constants::MAX_SPEED);
            mixMetabolism = Clamp(mixMetabolism, Constants::MIN_METABOLISM,
                                  Constants::MAX_METABOLISM);

            // Create new creature
            allCreatures.emplace_back(newPos, size);
            auto &child = allCreatures.back();
            child.strength = mixStrength;
            child.speed = mixSpeed;
            child.metabolism = mixMetabolism;

            // Reset energy after reproduction
            energy *= 0.7f; // Cost of reproduction

            state = CreatureState::MATING;
            break;
          }
        }
      }
    }
  } else if (health < Constants::LOW_HEALTH) {
    // Rest when somewhat injured
    state = CreatureState::SICK;
  } else {
    state = CreatureState::WANDERING;
  }

  // Implement contagion for sick creatures
  if (state == CreatureState::SICK) {
    for (auto &other : const_cast<std::vector<Creature> &>(others)) {
      if (&other != this) {
        Vector2 otherPos = other.GetPosition();
        float dx = position.x - otherPos.x;
        float dy = position.y - otherPos.y;
        float dist = sqrt(dx * dx + dy * dy);

        // If close enough, chance of spreading sickness
        if (dist < size * 2) {
          if (GetRandomValue(0, 100) < 10) { // 10% chance of infection
            other.health -= 5.0f;            // Reduce health
            if (other.health < Constants::CRITICAL_HEALTH) {
              other.state = CreatureState::SICK;
            }
          }
        }
      }
    }
  }
}

void Creature::UpdateMovement(float deltaTime,
                              const std::vector<Creature> &others) {
  // Don't move while eating, fighting, or mating
  if (state == CreatureState::EATING || state == CreatureState::FIGHTING ||
      state == CreatureState::MATING) {
    velocity = {0, 0};
    return;
  }

  // Normal random movement
  if (state != CreatureState::HUNTING) {
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
  if (speed > 0.1f) { // Only update rotation if moving significantly
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

void Creature::UpdateColor() { color = GetStateColor(); }

Color Creature::GetStateColor() const {
  switch (state) {
  case CreatureState::WANDERING:
    return GRAY;
  case CreatureState::HUNTING:
    return GREEN;
  case CreatureState::MATING:
    return PINK;
  case CreatureState::SICK:
    return BROWN;
  case CreatureState::EATING:
    return ORANGE;
  case CreatureState::FIGHTING:
    return RED;
  default:
    return GRAY;
  }
}

void Creature::Fight(Creature &opponent) {
  // Determine fight outcome based on strength
  float fightProbability = GetFightProbability(opponent);

  if (GetRandomValue(0, 100) / 100.0f < fightProbability) {
    // Winner gets energy and health boost
    energy += 10.0f;
    health += 5.0f;

    // Loser loses energy and health
    opponent.energy -= 15.0f;
    opponent.health -= 10.0f;
  } else {
    // Loser scenario
    energy -= 15.0f;
    health -= 10.0f;

    // Winner gets energy and health boost
    opponent.energy += 10.0f;
    opponent.health += 5.0f;
  }

  // Reset fight timer
  timeSinceLastFight = 0.0f;
  lastFightOpponent = &opponent;
}

float Creature::GetFightProbability(const Creature &opponent) const {
  // Calculate fight probability based on strength difference
  float strengthDiff = strength - opponent.strength;
  float baseProbability = 0.5f + (strengthDiff / (Constants::MAX_STRENGTH * 2));

  // Add some randomness
  baseProbability += (GetRandomValue(-10, 10) / 100.0f);

  // Clamp probability between 0 and 1
  return Clamp(baseProbability, 0.0f, 1.0f);
}

void Creature::Draw(int rank, const std::vector<Creature> &allCreatures) const {
  // Draw status text
  const char *stateText;
  switch (state) {
  case CreatureState::WANDERING:
    stateText = "Wandering";
    break;
  case CreatureState::HUNTING:
    stateText = "Hunting";
    break;
  case CreatureState::MATING:
    stateText = "Mating";
    break;
  case CreatureState::FIGHTING:
    stateText = "Fighting";
    break;
  case CreatureState::EATING:
    stateText = "Eating";
    break;
  case CreatureState::SICK:
    stateText = "Sick";
    break;
  default:
    stateText = "Unknown";
  }

  // Determine color based on selection state
  Color baseColor = color;
  Color nameColor = WHITE;
  Color statusColor = LIGHTGRAY;

  // Check if any creature is selected
  bool anyCreatureSelected =
      std::any_of(allCreatures.begin(), allCreatures.end(),
                  [](const Creature &c) { return c.IsSelected(); });

  // Dim all creatures except the selected one
  if (anyCreatureSelected && !selected) {
    baseColor = ColorAlpha(color, 0.3f);
    nameColor = ColorAlpha(WHITE, 0.3f);
    statusColor = ColorAlpha(LIGHTGRAY, 0.2f);
  }
  DrawText(TextFormat("#%d %s", rank, name.c_str()), position.x - size,
           position.y - size - 40, 10, nameColor);
  DrawText(TextFormat("[%.1fs]\n(%s)", age, stateText), position.x - size,
           position.y - size - 30, 8, statusColor);

  // Draw creature body
  DrawPoly(position, isMale ? 3 : 6, size, rotation + 90.0f, baseColor);

  // Draw health bar background and bar
  DrawRectangle(
      position.x - size, position.y - size - 10, size * 2, 4,
      ColorAlpha(RED, anyCreatureSelected && !selected ? 0.1f : 0.2f));
  DrawRectangle(
      position.x - size, position.y - size - 10, size * 2 * (health / 100.0f),
      4, ColorAlpha(RED, anyCreatureSelected && !selected ? 0.4f : 0.8f));
  DrawText(TextFormat("H:%.0f", health), position.x - size - 35,
           position.y - size - 10, 6,
           ColorAlpha(RED, anyCreatureSelected && !selected ? 0.2f : 0.8f));

  // Draw energy bar background and bar
  DrawRectangle(
      position.x - size, position.y - size - 6, size * 2, 4,
      ColorAlpha(YELLOW, anyCreatureSelected && !selected ? 0.1f : 0.2f));
  DrawRectangle(
      position.x - size, position.y - size - 6, size * 2 * (energy / 100.0f), 4,
      ColorAlpha(YELLOW, anyCreatureSelected && !selected ? 0.4f : 0.8f));
  DrawText(TextFormat("E:%.0f", energy), position.x + size * 2 - 2,
           position.y - size - 10, 6,
           ColorAlpha(YELLOW, anyCreatureSelected && !selected ? 0.2f : 0.8f));

  // Draw strength indicator (outline thickness)
  DrawPolyLines(
      position, isMale ? 3 : 6, size, rotation + 90.0f,
      ColorAlpha(WHITE, selected ? strength / 100.0f : strength / 300.0f));

  // Draw selection indicator
  if (selected) {
    DrawCircleLines(position.x, position.y, size * 1.5f, WHITE);

    // Draw attributes in smaller text with colors
    DrawText(TextFormat("st:%.0f", strength), position.x - size,
             position.y + size + 2, 10, ORANGE);
    DrawText(TextFormat("\nsp:%.1f", speed), position.x - size,
             position.y + size + 12, 10, SKYBLUE);
    DrawText(TextFormat("\n\nmt:%.1f", metabolism), position.x - size,
             position.y + size + 22, 10, GREEN);
    DrawText(TextFormat("\n\n\nsize:%.1f", size), position.x - size,
             position.y + size + 32, 10, PURPLE);
  }
}
