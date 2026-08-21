#include <stdexcept>

#include "particle_life.hpp"

std::ostream &operator<<(std::ostream &os, const Particle &p) {
    os << p.x_ << ',' << p.y_ << ',' << p.color_;
    return os;
}

ParticleLifeEngine::ParticleLifeEngine(int startingNumOfParticles,
                                       int startingNumOfColors, int screenWidth,
                                       int screenHeight)
    : numOfParticles_(startingNumOfParticles), screenWidth_(screenWidth),
      screenHeight_(screenHeight), numOfColors_(startingNumOfColors) {

    std::random_device rd;
    gen.seed(rd());
    spawnParticles();
    regenerateInteractionStrengths();
}
void ParticleLifeEngine::spawnParticles() {
    std::uniform_real_distribution<double> distribX(0, screenWidth_);
    std::uniform_real_distribution<double> distribY(0, screenHeight_);
    std::uniform_int_distribution<int> distribColor(0, numOfColors_ - 1);

    particles_.reserve(numOfParticles_);
    for (int i{0}; i < numOfParticles_; ++i) {
        particles_.push_back(
            new Particle(random(distribX), random(distribY), randomInt(distribColor)));
    }
}
void ParticleLifeEngine::regenerateInteractionStrengths() {
    // index = color * numOfColors + color
    int numOfInteractions = numOfColors_ * numOfColors_ * 3;
    colorInteractionStrengths_.reserve(numOfInteractions);
    for (size_t i{0}; i < numOfInteractions; ++i) {
        colorInteractionStrengths_.push_back(
            random(std::uniform_real_distribution<double>(-3.0, 3.0)));
    }
}

double ParticleLifeEngine::getColorInteractionStrength(
    int color1, int color2, InteractionPart interactionPart) {

    if (color1 >= numOfColors_ || color2 >= numOfColors_) {
        throw std::runtime_error("Invalid Colors: " + std::to_string(color1) +
                                 ", " + std::to_string(color2) +
                                 " | Max: " + std::to_string(numOfColors_ - 1));
    }
    if (interactionPart >= 3) {
        throw std::runtime_error("Invalid Interaction Part: " +
                                 std::to_string(static_cast<int>(interactionPart)));
    }
    return colorInteractionStrengths_[color1 * numOfColors_ * 3 + color2 * 3 +
                                      interactionPart];
}

void ParticleLifeEngine::update() {
    for (size_t i{0}; i < particles_.size() - 1; i++) {
        for (size_t j{i}; j < particles_.size(); j++) {
        }
    }
}

void ParticleLifeEngine::printAllParticles() {
    for (Particle *p : particles_) {
        std::cout << *p << '\n';
    }
}
void ParticleLifeEngine::printAllInteractionStrengths() {
    for (int i{0}; i < numOfColors_; i++) {
        for (int j{0}; j < numOfColors_; j++) {
            for (int k{0}; k < 3; k++) {
                std::cout << std::to_string(i) << " --> " << std::to_string(j)
                          << " (" << std::to_string(k) << "): "
                          << getColorInteractionStrength(
                                 i, j, static_cast<InteractionPart>(k))
                          << '\n';
            }
        }
    }
}
