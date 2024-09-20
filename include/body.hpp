#ifndef BODY_HPP_
#define BODY_HPP_

#include <vector>
#include <array>
#include <utility>

class Body {
    public:
        Body();
        Body(std::pair<float, float> position, std::pair<float, float> velocity, float mass, std::array<int, 3> rgb);
        ~Body();
        void update(std::vector<Body>& bodies);
        std::pair<float, float> getPosition();
        std::pair<float, float> getVelocity();
        float getMass();
        std::array<int, 3> getRGB();
        void setPosition(std::pair<float, float> position);
        void setVelocity(std::pair<float, float> velocity);
        void setMass(float mass);
        void setRGB(std::array<int, 3> rgb);

    private:
        std::pair<float, float> position;
        std::pair<float, float> velocity;
        float mass;
        std::array<int, 3> rgb;
};

#endif  // BODY_HPP_