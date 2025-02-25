#pragma once

namespace Constants {
    // Creature traits
    constexpr float INITIAL_HEALTH = 100.0f;
    constexpr float INITIAL_ENERGY = 100.0f;
    constexpr float MIN_STRENGTH = 40.0f;
    constexpr float MAX_STRENGTH = 100.0f;
    constexpr float MIN_SPEED = 0.5f;
    constexpr float MAX_SPEED = 1.2f;
    constexpr float MIN_METABOLISM = 0.5f;
    constexpr float MAX_METABOLISM = 1.5f;
    
    // State thresholds
    constexpr float HUNGRY_THRESHOLD = 30.0f;
    constexpr float CRITICAL_HEALTH = 20.0f;
    constexpr float LOW_HEALTH = 50.0f;
    constexpr float MATING_ENERGY = 60.0f;
    constexpr float MATING_AGE = 10.0f;
    
    // Movement
    constexpr float BASE_MOVEMENT_SPEED = 30.0f;
    constexpr float MAX_VELOCITY = 2.0f;
    constexpr float BOUNDARY_BOUNCE = -0.8f;
    constexpr float FOOD_SEEK_FORCE = 0.5f;
    constexpr float SEPARATION_RADIUS_FACTOR = 2.0f;  // Multiplied by creature size
    constexpr float SEPARATION_STRENGTH = 5.0f;
    
    // Energy & Health
    constexpr float ENERGY_CONSUMPTION_RATE = 1.f;
    constexpr float HEALTH_DECAY_RATE = 5.0f;
    constexpr float FOOD_ENERGY_VALUE = 30.0f;
    
    // Simulation
    constexpr float PHYSICS_TIMESTEP = 1.0f/60.0f;
    constexpr float FOOD_SPAWN_INTERVAL = 7.f;
    constexpr int FOOD_SPAWN_COUNT = 1;
    constexpr int INITIAL_CREATURE_COUNT = 30;
    constexpr float INITIAL_CREATURE_SIZE = 10.0f;
    
    // Screen
    constexpr int SCREEN_WIDTH = 1200;
    constexpr int SCREEN_HEIGHT = 800;
}
