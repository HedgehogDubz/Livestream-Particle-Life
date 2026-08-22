#include "particle_life.hpp"
#include <cmath>
#include <stdexcept>

std::vector<Color> DEFAULT_PARTICLE_COLORS = std::vector<Color>{
    RED,    GREEN, BLUE, YELLOW, PURPLE, ORANGE, (Color){0, 255, 255, 255},
    MAGENTA, PINK, BROWN, WHITE};

std::ostream &operator<<(std::ostream &os, const Particle &p) {
    os << p.x << ',' << p.y << ',' << p.color;
    return os;
}

ParticleLifeEngine::ParticleLifeEngine(int startingNumOfParticles,
                                       int startingNumOfColors, int screenWidth,
                                       int screenHeight,
                                       std::vector<Color> colors)
    : numOfParticles_(startingNumOfParticles),
      numOfColors_(startingNumOfColors), screenWidth_(screenWidth),
      screenHeight_(screenHeight), colors_(colors) {

    std::random_device rd;
    gen.seed(rd());
    spawnParticles();
    regenerateInteractionStrengths();
}
void ParticleLifeEngine::spawnParticles() {
    std::uniform_real_distribution<double> distribX(0, screenWidth_);
    std::uniform_real_distribution<double> distribY(0, screenHeight_);
    std::uniform_int_distribution<int> distribColor(0, numOfColors_ - 1);
    particles_.clear();
    particles_.reserve(numOfParticles_);
    for (int i{0}; i < numOfParticles_; ++i) {
        particles_.push_back(new Particle(random(distribX), random(distribY),
                                          randomInt(distribColor)));
    }
}
void ParticleLifeEngine::regenerateInteractionStrengths() {
    // index = color * numOfColors + color
    int numOfInteractions = numOfColors_ * numOfColors_ * 3;
    colorInteractionStrengths_.clear();
    colorInteractionStrengths_.reserve(numOfInteractions);
    for (size_t i{0}; i < numOfInteractions; i += 3) {
        double end = 
            random(std::uniform_real_distribution<double>(60.0, 100.0));
        colorInteractionStrengths_.push_back(end / 4);
        colorInteractionStrengths_.push_back(
            random(std::uniform_real_distribution<double>(-9.0, 9.0)));
        colorInteractionStrengths_.push_back(end);
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
        throw std::runtime_error(
            "Invalid Interaction Part: " +
            std::to_string(static_cast<int>(interactionPart)));
    }
    return colorInteractionStrengths_[color1 * numOfColors_ * 3 + color2 * 3 +
                                      interactionPart];
}

void ParticleLifeEngine::update() {
    for (size_t i{0}; i + 1 < particles_.size(); i++) {
        for (size_t j{i + 1}; j < particles_.size(); j++) {
            Particle *p1 = particles_[i];
            Particle *p2 = particles_[j];

            double dx = p2->x - p1->x;
            double dy = p2->y - p1->y;

            if (dx > screenWidth_ / 2.0) {
                dx -= screenWidth_;
            }
            if (dx < -screenWidth_ / 2.0) {
                dx += screenWidth_;
            }
            if (dy > screenHeight_ / 2.0) {
                dy -= screenHeight_;
            }
            if (dy < -screenHeight_ / 2.0) {
                dy += screenHeight_;
            }

            double distance = std::sqrt(((dx) * (dx)) + ((dy) * (dy)));
            if (distance == 0) {
                continue;
            }
            double start1 = getColorInteractionStrength(p1->color, p2->color,
                                                        InteractionPart::START);
            double peak1 = getColorInteractionStrength(p1->color, p2->color,
                                                       InteractionPart::PEAK);
            double end1 = getColorInteractionStrength(p1->color, p2->color,
                                                      InteractionPart::END);
            double attraction1 =
                calculateAttraction(distance, start1, peak1, end1);
            double start2 = getColorInteractionStrength(p2->color, p1->color,
                                                        InteractionPart::START);
            double peak2 = getColorInteractionStrength(p2->color, p1->color,
                                                       InteractionPart::PEAK);
            double end2 = getColorInteractionStrength(p2->color, p1->color,
                                                      InteractionPart::END);
            double attraction2 =
                calculateAttraction(distance, start2, peak2, end2);
            double normalized_x = dx / distance;
            double normalized_y = dy / distance;
            p1->velocityX += normalized_x * attraction1 * 1.0 / 60.0;
            p1->velocityY += normalized_y * attraction1 * 1.0 / 60.0;
            p2->velocityX -= normalized_x * attraction2 * 1.0 / 60.0;
            p2->velocityY -= normalized_y * attraction2 * 1.0 / 60.0;
        }
    }
    for (Particle *p : particles_) {
        p->x += p->velocityX;
        p->y += p->velocityY;
        p->velocityX *= 0.75;
        p->velocityY *= 0.75;

        p->x = std::fmod(std::fmod(p->x, screenWidth_) + screenWidth_, (double)screenWidth_);
        p->y = std::fmod(std::fmod(p->y, screenHeight_) + screenHeight_, + (double)screenHeight_);
    }
}

double ParticleLifeEngine::calculateAttraction(double distance, double start,
                                               double peak, double end) {
    if (distance <= start) {
        double max_repel = -45;
        double range = start;
        return (-max_repel * distance * distance) / (range * range) + max_repel;
    }
    if (distance <= (start + end) / 2) {
        return (2.0 * peak * (distance - start)) / (end - start);
    }
    if (distance <= end) {
        return (-2.0 * peak * (distance - end)) / (end - start);
    }
    return 0.0;
}

void ParticleLifeEngine::draw() {
    for (size_t i{0}; i < particles_.size() - 1; i++) {
        DrawCircle(particles_[i]->x, particles_[i]->y, 5,
                   colors_[particles_[i]->color % colors_.size()]);
    }
}

void ParticleLifeEngine::printParticles() {
    for (Particle *p : particles_) {
        std::cout << *p << '\n';
    }
}
void ParticleLifeEngine::printColorInteractionStrengths() {
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
