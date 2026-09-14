#include "particle_life.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

std::vector<Color> DEFAULT_PARTICLE_COLORS = std::vector<Color>{
    RED,    GREEN, BLUE, YELLOW, PURPLE, ORANGE, (Color){0, 255, 255, 255},
    MAGENTA, PINK, BROWN, WHITE};


static constexpr double MAX_REPEL = 45.0;
static constexpr double MAX_ATTRACTION = MAX_REPEL * 3.0;

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
    const size_t numOfInteractions = numOfColors_ * numOfColors_ * 3;
    colorInteractionStrengths_.clear();
    colorInteractionStrengths_.reserve(numOfInteractions);

    // Every colour pair shares the same radii; only the strength varies.
    // RMAX must be ~3x the mean particle spacing (sqrt(width*height/count),
    // ~26px at 1500 particles on 1080x960) or the sim breaks into hundreds of
    // specks instead of forming large structures.
    // BETA is the core-to-range ratio, which puts the bond length at about one
    // particle spacing. Lowering it to give filaments more room backfires: the
    // attraction thins out and the whole thing melts into an even gas.
    const double RMAX = 85.0;
    const double BETA = 0.30;
    const double PEAK = 15.0;

    for (size_t i{0}; i < numOfInteractions; i += 3) {
        double peak =
            random(std::uniform_real_distribution<double>(-PEAK, PEAK));
        //start
        colorInteractionStrengths_.push_back(RMAX * BETA);
        //interaction strength
        colorInteractionStrengths_.push_back(peak);
        //end
        colorInteractionStrengths_.push_back(RMAX);
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
    const size_t n = particles_.size();
    std::vector<double> repelX(n, 0.0), repelY(n, 0.0);     // never clamped
    std::vector<double> attractX(n, 0.0), attractY(n, 0.0); // clamped below

    // Nothing interacts past the largest END radius, so reject distant pairs on
    // squared distance before paying for a sqrt and six matrix lookups.
    double maxRadius = 0.0;
    for (size_t k{2}; k < colorInteractionStrengths_.size(); k += 3) {
        maxRadius = std::max(maxRadius, colorInteractionStrengths_[k]);
    }
    const double maxRadiusSq = maxRadius * maxRadius;

    for (size_t i{0}; i + 1 < n; i++) {
        for (size_t j{i + 1}; j < n; j++) {
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

            double distanceSq = dx * dx + dy * dy;
            if (distanceSq > maxRadiusSq || distanceSq == 0.0) {
                continue;
            }
            double distance = std::sqrt(distanceSq);
            double normalized_x = dx / distance;
            double normalized_y = dy / distance;

            double start1 = getColorInteractionStrength(p1->color, p2->color,
                                                        InteractionPart::START);
            double peak1 = getColorInteractionStrength(p1->color, p2->color,
                                                       InteractionPart::PEAK);
            double end1 = getColorInteractionStrength(p1->color, p2->color,
                                                      InteractionPart::END);
            double attraction1 =
                calculateAttraction(distance, start1, peak1, end1);
            if (distance <= start1) {
                repelX[i] += normalized_x * attraction1;
                repelY[i] += normalized_y * attraction1;
            } else {
                attractX[i] += normalized_x * attraction1;
                attractY[i] += normalized_y * attraction1;
            }

            double start2 = getColorInteractionStrength(p2->color, p1->color,
                                                        InteractionPart::START);
            double peak2 = getColorInteractionStrength(p2->color, p1->color,
                                                       InteractionPart::PEAK);
            double end2 = getColorInteractionStrength(p2->color, p1->color,
                                                      InteractionPart::END);
            double attraction2 =
                calculateAttraction(distance, start2, peak2, end2);
            if (distance <= start2) {
                repelX[j] -= normalized_x * attraction2;
                repelY[j] -= normalized_y * attraction2;
            } else {
                attractX[j] -= normalized_x * attraction2;
                attractY[j] -= normalized_y * attraction2;
            }
        }
    }

    // Clamp only the shell attraction: a crowd of distant particles must never
    // out-pull one close neighbour, or they collapse on top of each other.
    for (size_t i{0}; i < n; i++) {
        double magnitude = std::sqrt(attractX[i] * attractX[i] +
                                     attractY[i] * attractY[i]);
        if (magnitude > MAX_ATTRACTION) {
            attractX[i] *= MAX_ATTRACTION / magnitude;
            attractY[i] *= MAX_ATTRACTION / magnitude;
        }

        Particle *p = particles_[i];
        p->velocityX += (repelX[i] + attractX[i]) * 1.0 / 60.0;
        p->velocityY += (repelY[i] + attractY[i]) * 1.0 / 60.0;

        p->x += p->velocityX;
        p->y += p->velocityY;
        p->velocityX *= 0.85;
        p->velocityY *= 0.85;

        p->x = std::fmod(std::fmod(p->x, screenWidth_) + screenWidth_, (double)screenWidth_);
        p->y = std::fmod(std::fmod(p->y, screenHeight_) + screenHeight_, (double)screenHeight_);
    }
}

double ParticleLifeEngine::calculateAttraction(double distance, double start,
                                               double peak, double end) {
    if (distance <= start) {
        double max_repel = -MAX_REPEL;
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
    for (size_t i{0}; i < particles_.size(); i++) {
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
