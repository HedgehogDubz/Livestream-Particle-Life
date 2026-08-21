#include "raylib.h"
#include "particle_life.hpp"
int main(void) {
    ParticleLifeEngine *ple = new ParticleLifeEngine(400, 4, 800, 800);
    ple->printAllParticles();
    ple->printAllInteractionStrengths();
}