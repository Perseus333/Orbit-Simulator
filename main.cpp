#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include <iostream>
#include <random>
#include <vector>
#include <windows.h>
#include <array>

using namespace std;

const int WIDTH = 800;
const int HEIGHT = 800;

const double G = 6.67e3;
const int numBodies = 10;
const int MAX_V = 500000;
const int MAX_INIT_V = 10;
const int MIN_MASS = 300; // Mass of the moon
const int MAX_MASS = 50000; // Mass of the sun
const int DISTANCE_SCALE = 1;
const float epsilon = 10.0f;
const float timeStep = 1e-2f;
const int MAX_SIZE = 7;
const int FUSION_TRESHOLD = 10;

const bool trails = false;
const bool NO_ESCAPE = true;
const bool RESTART_V = true;
const bool fusion = false;

// From here: https://stackoverflow.com/a/20136256
template<typename T>
T random(T range_from, T range_to) {
    random_device                  rand_dev;
    mt19937                        generator(rand_dev());
    uniform_int_distribution<T>    distr(range_from, range_to);
    return distr(generator);
}

class Body {
    public:
    float x;
    float y;
    float vx;
    float vy;
    int mass;
    array<int, 3> rgb;

    // Update method to take vector instead of array
    void getInfluenced(vector<Body>& bodies) {
        for (auto it = bodies.begin(); it != bodies.end(); /* no increment here */) {
            Body& otherBody = *it;

            // Skip self
            if (&otherBody == this) {
                ++it;
                continue;
            }

            float dx = otherBody.x - this->x * DISTANCE_SCALE;
            float dy = otherBody.y - this->y * DISTANCE_SCALE;
            float distanceSquared = pow(dx, 2) + pow(dy, 2) + epsilon;

            // A bit broken
            if (fusion) {
                // If the bodies are close enough, fuse them
                if (sqrt(distanceSquared) < FUSION_TRESHOLD) {
                    // Apply mass-weighted average for velocity
                    this->vx = (this->vx * this->mass + otherBody.vx * otherBody.mass) / (this->mass + otherBody.mass);
                    this->vy = (this->vy * this->mass + otherBody.vy * otherBody.mass) / (this->mass + otherBody.mass);

                    this->mass = min(otherBody.mass + this->mass, MAX_MASS);

                    it = bodies.erase(it);  // Remove the other body
                    continue;  // Skip the rest of the loop for this iteration
                }
            }

            // Gravitational interaction logic
            float force = G * (otherBody.mass) / distanceSquared;

            float deltaVx = force * (dx / sqrt(distanceSquared));
            float deltaVy = force * (dy / sqrt(distanceSquared));

            this->vx += deltaVx/this->mass;
            this->vy += deltaVy/this->mass;

            // Cap velocity to avoid extreme speeds
            if (abs(vx) > MAX_V) {
                vx = (vx > 0 ? 1 : -1) * MAX_V;
            }
            if (abs(vy) > MAX_V) {
                vy = (vy > 0 ? 1 : -1) * MAX_V;
            }

            ++it;  // Increment the iterator normally
        }

        // Update position based on the current velocity
        this->x += vx * timeStep;
        this->y += vy * timeStep;
        // NO ESCAPE
        if (NO_ESCAPE) {
            if (this->x > WIDTH) {
                this->x = this->x - WIDTH;
                if (RESTART_V) {
                    this->vx = random(1, MAX_INIT_V);
                    this->vy = random(1, MAX_INIT_V);
                }
            }
            if (this->x < 0) {
                this->x = WIDTH - this->x;
                if (RESTART_V) {
                    this->vx = random(1, MAX_INIT_V);
                    this->vy = random(1, MAX_INIT_V);
                }
            }
            if (this->y > HEIGHT) {
                this->y = this->y - HEIGHT;
                if (RESTART_V) {
                    this->vx = random(1, MAX_INIT_V);
                    this->vy = random(1, MAX_INIT_V);
                }
            }
            if (this->y < 0) {
                this->y = HEIGHT - this->y;
                if (RESTART_V) {
                    this->vx = random(1, MAX_INIT_V);
                    this->vy = random(1, MAX_INIT_V);
                }
            }
        }
    }
};

// Use a vector for the bodies instead of an array
vector<Body> bodies;

int initializeBodies() {
    for (int i = 0; i < numBodies; ++i) {
        int x = random(0, WIDTH);
        int y = random(0, HEIGHT);
        int mass = random(MIN_MASS, MAX_MASS);

        Body newBody;
        newBody.x = x;
        newBody.y = y;
        float vx = random(0, MAX_INIT_V);
        float vy = random(0, MAX_INIT_V);
        newBody.vx = vx;
        newBody.vy = vy;
        newBody.mass = mass;

        // Assigning color by mass
        
        int r = 255;
        int g = 255 - ((mass / (float)MAX_MASS) * 255);
        int b = 0;
        

       // Assigning color by speed
       /*
        int r = 255;
        int g = 255 - ((sqrt(vx*vx + vy*vy) / (float)MAX_V) * 255);
        int b = 0;
        */

        // Assigning random colors
        /*
        int r = random(1, 255);
        int g = random(1, 255);
        int b = random(1, 255);
        */
        array<int, 3> rgb = {r, g, b};
        newBody.rgb = rgb;

        // Add new body to the vector
        bodies.push_back(newBody);
    }

    return 1;
}

void drawBody(SDL_Renderer* renderer, int x, int y, int mass, Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    float size = ((float)mass / (float)MAX_MASS) * MAX_SIZE;
    SDL_Rect pixel = { x, y, (int)size, (int)size };
    SDL_RenderFillRect(renderer, &pixel);
}

bool initializeSDL(SDL_Window** window, SDL_Renderer** renderer) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }

    // Create the window
    *window = SDL_CreateWindow("Orbit Sim", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (*window == nullptr) {
        cout << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        SDL_Quit();
        return false;
    }

    // Create the renderer
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == nullptr) {
        cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return false;
    }

    return true;
}

int main(int argc, char* argv[]) {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    // Initialize SDL
    if (!initializeSDL(&window, &renderer)) {
        return 1;  // Exit if initialization failed
    }

    // Main Logic
    initializeBodies();

    bool running = true;
    SDL_Event event;

    // Main loop
    while (running) {
        // Event handling
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Clear screen with black color
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        if (!trails) {
            SDL_RenderClear(renderer);
        }

        // Draw celestial bodies
        for (Body& body : bodies) {
            body.getInfluenced(bodies);
            drawBody(renderer, (int)body.x, (int)body.y, body.mass, body.rgb[0], body.rgb[1], body.rgb[2]);
        }

        // Present the renderer
        SDL_RenderPresent(renderer);

        Sleep(50);
    }

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
