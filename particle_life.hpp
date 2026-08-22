#pragma once
#include <iostream>
#include <random>
#include <raylib.h>
#include <vector>

extern std::vector<Color> DEFAULT_PARTICLE_COLORS;
struct Particle {
    double x;
    double y;
    int color;

    double velocityX = 0;
    double velocityY = 0;

    Particle(int x, int y, int color) : x(x), y(y), color(color) {}

    friend std::ostream &operator<<(std::ostream &os, const Particle &p);
};
enum InteractionPart { START, PEAK, END };
class ParticleLifeEngine {
  private:
    std::vector<Particle *> particles_;
    std::vector<double> colorInteractionStrengths_;
    int numOfParticles_;
    int numOfColors_;
    int screenWidth_;
    int screenHeight_;

    std::vector<Color> colors_;

    std::mt19937 gen;

    int randomInt(std::uniform_int_distribution<int> distribution) {
        return distribution(gen);
    }
    double random(std::uniform_real_distribution<double> distribution) {
        return distribution(gen);
    }

  public:
    ParticleLifeEngine(int startingNumOfParticles, int startingNumOfColors,
                       int screenWidth, int screenHeight,
                       std::vector<Color> colors = DEFAULT_PARTICLE_COLORS);
    void spawnParticles();
    void regenerateInteractionStrengths();
    double getColorInteractionStrength(int color1, int color2,
                                       InteractionPart interactionPart);

    std::vector<Particle *> getParticles() { return particles_; }
    std::vector<double> getColorInteractionStrengths() {return colorInteractionStrengths_; }
    
    double calculateAttraction(double distance, double start, double peak, double end);

    void update();
    void draw();

    void printParticles();
    void printColorInteractionStrengths();
};