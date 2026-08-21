#pragma once
#include <iostream>
#include <random>
#include <vector>
class Particle {
    double x_;
    double y_;
    int color_;

    int momentumX_;
    int momentumY_;

  public:
    Particle(int x, int y, int color) : x_(x), y_(y), color_(color) {}

    friend std::ostream &operator<<(std::ostream &os, const Particle &p);
};
enum InteractionPart {
    START,
    PEAK,
    END
};
class ParticleLifeEngine {
  private:
    std::vector<Particle *> particles_;
    std::vector<double> colorInteractionStrengths_;
    int numOfParticles_;
    int numOfColors_;
    int screenWidth_;
    int screenHeight_;


    std::mt19937 gen;

    int randomInt(std::uniform_int_distribution<int> distribution) {
        return distribution(gen);
    }
    double random(std::uniform_real_distribution<double> distribution) {
        return distribution(gen);
    }
  public:
    ParticleLifeEngine(int startingNumOfParticles, int startingNumOfColors, int screenWidth,
                       int screenHeight);
    void spawnParticles();
    void regenerateInteractionStrengths();
    double getColorInteractionStrength(int color1, int color2, InteractionPart interactionPart);
    
    void update();


    void printAllParticles();
    void printAllInteractionStrengths();
};