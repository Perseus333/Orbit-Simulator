#ifndef PARAMS_HPP_
#define PARAMS_HPP_

namespace params {
    constexpr int WIDTH = 800;
    constexpr int HEIGHT = 600;

    constexpr float G = 6.67e3;
    constexpr int numBodies = 30;
    constexpr int MAX_V = 500000;
    constexpr int MAX_INIT_V = 10;
    constexpr float MIN_MASS = 300.0f;   // Minimum mass of a body
    constexpr float MAX_MASS = 50000.0f;  // Maximum mass of a body
    constexpr int DISTANCE_SCALE = 1;
    constexpr float epsilon = 10.0f;
    constexpr float timeStep = 1e-2f;
    constexpr int MAX_SIZE = 7;
    constexpr int FUSION_TRESHOLD = 10;

    constexpr bool trails = false;
    constexpr bool NO_ESCAPE = true;
    constexpr bool RESTART_V = true;
    constexpr bool fusion = false;
}

#endif // PARAMS_HPP