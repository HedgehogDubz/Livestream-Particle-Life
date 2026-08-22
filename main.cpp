#include "raylib.h"
#include "particle_life.hpp"
int main(void) {
    InitWindow(1080, 960, "Particle Life");
    SetTargetFPS(60);
    ParticleLifeEngine *ple = new ParticleLifeEngine(1500, 8, 1080, 960);
    ple->printParticles();
    ple->printColorInteractionStrengths();

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        ple->update();
        ple->draw();
        EndDrawing();
    }
}