#include "particle_life.hpp"
#include "raylib.h"

int main() {
    InitWindow(800, 800, "Particle Life");
    SetTargetFPS(60);
    ParticleLifeEngine *ple = new ParticleLifeEngine(400, 4, 800, 800);
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        for (int i {0}; i < 800; i++) {
            double y = 400 + ple->calculateAttraction((i * i)/ 400, 200.0, 300.0, 600.0);

            DrawPixel(i, y, WHITE);
        }
        EndDrawing();
    }
}


// #include <gtest/gtest.h>

// TEST(ParticleLifeTests, InteractionStartCorrect) {
//     int numOfColors = 4;
//     ParticleLifeEngine *ple =
//         new ParticleLifeEngine(400, numOfColors, 800, 800);
//     std::vector<double> cis = ple->getColorInteractionStrengths();
//     for (int i{0}; i < numOfColors; i++) {
//         for (int j{0}; j < numOfColors; j++) {
//             for (int k{0}; k < 3; k++) {
//                 if (k == 0) {
//                     EXPECT_GT(ple->getColorInteractionStrength(
//                                   i, j, InteractionPart::START),
//                               1);
//                 }
//                 if (k == 2) {
//                     EXPECT_GT(ple->getColorInteractionStrength(
//                                   i, j, InteractionPart::END),
//                               ple->getColorInteractionStrength(
//                                   i, j, InteractionPart::START) + 10);
//                 }
//             }
//         }
//     }
// }