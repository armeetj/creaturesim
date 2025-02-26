# Creature Simulation

## Overview
This project simulates creatures in a 2D environment where they move, eat, fight, mate, and interact with each other based on predefined rules. The simulation models simple behaviors such as wandering, hunting, and responding to external stimuli like food, other creatures, and their energy levels.

https://github.com/user-attachments/assets/3bc1c11c-8ef3-4d23-9972-3ff6830b724c


HD video: https://youtu.be/smVHQpItEno

I didn't spend too much time balancing environment parameters, as the system is pretty volatile. The creatures aren't very smart either, so they're almost always certain to go extinct.
To play yourself, it's quite easy: install raylib (on mac this is as simple as `brew install raylib`), and then run `make run`.


## Features
- **Creature Attributes**: Each creature has attributes like position, velocity, rotation, size, health, energy, strength, speed, metabolism, and age.
- **Behavioral States**:
  - Wandering: Default movement behavior.
  - Hunting: Searching for food when energy is low.
  - Eating: Consuming food when found.
  - Fighting: Engaging in combat with other creatures over resources.
  - Mating: Reproducing with another creature when conditions are met.
  - Sick: Recovering from low health or spreading illness.
- **Environmental Interaction**:
  - Creatures move randomly but prioritize essential actions like eating and mating.
  - Health decreases when energy is low, simulating starvation.
  - Creatures gain energy by eating food and can grow in size.
  - Speed and strength are affected by size changes.
- **Genetic Evolution**: children share traits with their parents
- **State-Based Coloring**: Each creature changes color based on its state for easy visualization.
- **Fight Mechanics**: Strength determines the probability of winning a fight, with the victor gaining energy and the loser taking damage.
- **Reproduction**: Creatures reproduce when they meet the mating criteria, mixing attributes with slight variations to simulate genetic inheritance.
- **Boundaries and Movement**:
  - Creatures move in a bounded 2D space.
  - They bounce off boundaries to stay within the simulation area.
- **Infection Mechanic**: Sick creatures can spread illness to others in proximity.

## Compilation & Usage
### Compilation
To compile the simulation, ensure you have a C++ compiler supporting C++11 or later. If using Raylib for visualization, link against it:
```sh
make
```

### Running the Simulation
```sh
make run
```

## Code Structure
- `Creature::Update()`: Main update loop handling creature behavior.
- `Creature::UpdateState()`: Determines the state based on energy, health, and environmental factors.
- `Creature::UpdateMovement()`: Handles movement and boundary constraints.
- `Creature::Fight()`: Manages combat mechanics.
- `Creature::Draw()`: Visualizes creatures and their attributes.

## Future Improvements
- **AI-driven behavior**: Implement more complex decision-making algorithms.
- **Environmental Changes**: Introduce dynamic events such as seasons affecting food availability.
- **Predator-Prey Dynamics**: Add different species with unique behaviors.
- **Multiplayer/Networking**: Enable interactive simulations with multiple users.

This project serves as a foundation for simulating emergent behaviors in artificial ecosystems and could be extended into more complex evolutionary simulations.
