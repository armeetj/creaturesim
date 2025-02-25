#pragma once
#include <string>
#include <vector>

namespace Names {
    const std::vector<std::string> first_names = {
        "Luna", "Nova", "Orion", "Atlas", "Vega", "Lyra", "Sirius", "Rigel",
        "Hydra", "Phoenix", "Draco", "Cetus", "Aries", "Leo", "Pavo", "Aquila"
    };
    
    const std::vector<std::string> last_names = {
        "Star", "Nebula", "Comet", "Galaxy", "Quasar", "Pulsar", "Nova",
        "Storm", "Cloud", "Wind", "Rain", "Thunder", "Frost", "Flame"
    };
    
    inline std::string generate_name() {
        return first_names[GetRandomValue(0, first_names.size() - 1)] + " " +
               last_names[GetRandomValue(0, last_names.size() - 1)];
    }
}
