#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include <iostream>
#include "../include/body.hpp"
#include "../include/params.hpp"
#include "../include/utils.hpp"

// Use a vector for the bodies instead of an array
std::vector<Body> bodies;

void initializeBodies()
{
    for (int i = 0; i < params::numBodies; ++i)
    {
        float x = utils::random_float(0, params::WIDTH);
        float y = utils::random_float(0, params::HEIGHT);
        float vx = utils::random_float(0, params::MAX_INIT_V);
        float vy = utils::random_float(0, params::MAX_INIT_V);
        float mass = utils::random_float(params::MIN_MASS, params::MAX_MASS);

        // Calculate color based on mass
        int r = 255;
        int g = 255 - static_cast<int>((mass / params::MAX_MASS) * 255);
        int b = 0;
        std::array<int, 3> rgb = {r, g, b};

        Body newBody(std::make_pair(x, y), std::make_pair(vx, vy), mass, rgb);

        // Add new body to the vector
        bodies.push_back(newBody);
    }
}

void drawBody(SDL_Renderer *renderer, int x, int y, float mass, Uint8 r, Uint8 g, Uint8 b)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    float size = (mass / params::MAX_MASS) * params::MAX_SIZE;
    SDL_Rect pixel = {x, y, (int)size, (int)size};
    SDL_RenderFillRect(renderer, &pixel);
}

bool initializeSDL(SDL_Window **window, SDL_Renderer **renderer)
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create the window
    *window = SDL_CreateWindow("Orbit Sim", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, params::WIDTH, params::HEIGHT, SDL_WINDOW_SHOWN);
    if (*window == nullptr)
    {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    // Create the renderer
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == nullptr)
    {
        std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return false;
    }

    return true;
}

int main()
{
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    // Initialize SDL
    if (!initializeSDL(&window, &renderer))
    {
        return 1; // Exit if initialization failed
    }

    // Main Logic
    initializeBodies();

    bool running = true;
    SDL_Event event;

    // Main loop
    while (running)
    {
        // Event handling
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
        }

        // Clear screen with black color
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        if (!params::trails)
        {
            SDL_RenderClear(renderer);
        }

        // Draw celestial bodies
        for (Body &body : bodies)
        {
            body.update(bodies);
            drawBody(renderer, body.getPosition().first, body.getPosition().second, body.getMass(), body.getRGB().at(0), body.getRGB().at(1), body.getRGB().at(2));
        }

        // Present the renderer
        SDL_RenderPresent(renderer);

        SDL_Delay(50);
    }

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
