#include "../include/body.hpp"
#include "../include/params.hpp"
#include "../include/utils.hpp"
#include <cmath>
#include <algorithm>

Body::Body()
{
    position = std::make_pair(0.0f, 0.0f);
    velocity = std::make_pair(0.0f, 0.0f);
    mass = 0.0f;
    rgb = {0, 0, 0};
}

Body::Body(std::pair<float, float> position, std::pair<float, float> velocity, float mass, std::array<int, 3> rgb)
{
    this->position = position;
    this->velocity = velocity;
    this->mass = mass;
    this->rgb = rgb;
}

Body::~Body() {}

void Body::update(std::vector<Body> &bodies)
{
    for (unsigned int i = 0; i < bodies.size(); ++i)
    {
        if (this == &bodies.at(i))
        {
            continue;
        }

        std::pair<float, float> dPosition = {bodies.at(i).position.first - position.first, bodies.at(i).position.second - position.second};
        float distanceSquared = std::pow(dPosition.first, 2) + std::pow(dPosition.second, 2) + params::epsilon;
    
        if (params::fusion)
        {
            if (std::sqrt(distanceSquared) < params::FUSION_TRESHOLD)
            {
                velocity.first = (velocity.first * mass + bodies.at(i).getVelocity().first * bodies.at(i).getMass()) / (mass + bodies.at(i).getMass());
                velocity.second = (velocity.second * mass + bodies.at(i).getVelocity().second * bodies.at(i).getMass()) / (mass + bodies.at(i).getMass());

                mass = std::min(bodies.at(i).getMass() + mass, params::MAX_MASS);

                bodies.erase(bodies.begin() + i);
                continue;
            }
        }

        float force = params::G * (bodies.at(i).getMass()) / distanceSquared;

        std::pair<float, float> dVelocity = {force * (dPosition.first / std::sqrt(distanceSquared)), force * (dPosition.second / std::sqrt(distanceSquared))};

        velocity.first += dVelocity.first / mass;
        velocity.second += dVelocity.second / mass;

        if (std::abs(velocity.first) > params::MAX_V)
        {
            velocity.first = (velocity.first > 0 ? 1 : -1) * params::MAX_V;
        }
        if (std::abs(velocity.second) > params::MAX_V)
        {
            velocity.second = (velocity.second > 0 ? 1 : -1) * params::MAX_V;
        }
    }

    // Update position based on the current velocity
    position.first += velocity.first * params::timeStep;
    position.second += velocity.second * params::timeStep;

    if (params::NO_ESCAPE)
    {
        if (position.first > params::WIDTH)
        {
            position.first = position.first - params::WIDTH;
            if (params::RESTART_V)
            {
                velocity.first = utils::random_float(1, params::MAX_INIT_V);
                velocity.second = utils::random_float(1, params::MAX_INIT_V);
            }
        }
        if (position.first < 0)
        {
            position.first = params::WIDTH - position.first;
            if (params::RESTART_V)
            {
                velocity.first = utils::random_float(1, params::MAX_INIT_V);
                velocity.second = utils::random_float(1, params::MAX_INIT_V);
            }
        }
        if (position.second > params::HEIGHT)
        {
            position.second = position.second - params::HEIGHT;
            if (params::RESTART_V)
            {
                velocity.first = utils::random_float(1, params::MAX_INIT_V);
                velocity.second = utils::random_float(1, params::MAX_INIT_V);
            }
        }
        if (position.second < 0)
        {
            position.second = params::HEIGHT - position.second;
            if (params::RESTART_V)
            {
                velocity.first = utils::random_float(1, params::MAX_INIT_V);
                velocity.second = utils::random_float(1, params::MAX_INIT_V);
            }
        }
    }
}

void Body::setPosition(std::pair<float, float> position)
{
    this->position = position;
}

void Body::setVelocity(std::pair<float, float> velocity)
{
    this->velocity = velocity;
}

void Body::setMass(float mass)
{
    this->mass = mass;
}

void Body::setRGB(std::array<int, 3> rgb)
{
    this->rgb = rgb;
}

std::pair<float, float> Body::getPosition()
{
    return position;
}

std::pair<float, float> Body::getVelocity()
{
    return velocity;
}

float Body::getMass()
{
    return mass;
}

std::array<int, 3> Body::getRGB()
{
    return rgb;
}