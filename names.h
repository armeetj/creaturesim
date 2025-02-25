#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include <cstdlib>

namespace Names {

    const std::vector<std::string> first_names = {
        "luna", "nova", "orion", "atlas", "vega", "lyra", "sirius", "rigel",
        "hydra", "phoenix", "draco", "cetus", "aries", "leo", "pavo", "aquila",
        "cygnus", "andromeda", "perseus", "hercules", "cassiopeia", "pegasus", "ursa",
        "corvus", "carina", "delphinus", "fornax", "grus", "lacerta", "monoceros", "ophiuchus",
        "pyxis", "scutum", "serpens", "volans", "vulpecula", "bootes", "columba", "dorado",
        "lynx", "crater", "vela", "tucana", "musca", "camelopardalis", "eridanus", "pictor",
        "telescopium", "triangulum", "auriga", "lupus", "procyon", "betelgeuse", "antares",
        "aldebaran", "deneb", "pollux", "spica", "caster", "altair", "mizar", "polaris",
        "capella", "mirach", "achernar", "algol", "alphard", "arakis", "bellatrix", "canopus",
        "castor", "fomalhaut", "hadar", "markab", "meissa", "menkar", "mintaka", "naos",
        "nunki", "rasalhague", "saiph", "scheat", "shaula", "zubenelgenubi"
    };

    const std::vector<std::string> last_names = {
        "star", "nebula", "comet", "galaxy", "quasar", "pulsar",
        "storm", "cloud", "wind", "rain", "thunder", "frost", "flame",
        "eclipse", "shadow", "flare", "cosmos", "zenith", "meteor", "twilight",
        "spectrum", "void", "orbit", "celestial", "plasma", "solstice", "lunar",
        "solar", "stellar", "halo", "horizon", "polar", "radiance", "astral",
        "vacuum", "ragnarok", "supernova", "umbra", "spectral", "aura", "binary",
        "singularity", "portal", "quantum", "corona", "perihelion", "aphelion", "hemisphere",
        "quark", "eon", "millennium", "epoch", "equinox", "eventide", "specter", "electra",
        "nebularis", "zenon", "starlight", "darkmatter", "hypernova", "neutrino", "radiant",
        "graviton", "chronos", "helios", "orionis", "eonflux", "singulon", "exoplanet"
    };

    inline std::string generate_name() {
        static std::unordered_set<std::string> used_names;

        std::string name;
        do {
            name = first_names[rand() % first_names.size()] + " " +
                   last_names[rand() % last_names.size()];
        } while (used_names.find(name) != used_names.end());

        used_names.insert(name);
        return name;
    }
}
