#include <SDL.h>
#include <stdio.h>
#include "game_state_manager.h"

#undef main

#define WIDTH 320
#define HEIGHT 180
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

GameStateManager *game_state_manager = nullptr;

bool running = true;

void init()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		fprintf(stderr, "SDL_Init(SDL_INIT_VIDEO) has failed! %s\n", SDL_GetError());
		exit(1);
	}

	window = SDL_CreateWindow(
		"Game",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN
	);

	if (window == nullptr)
	{
		fprintf(stderr, "SDL_CreateWindow has failed! %s\n", SDL_GetError());
		exit(1);
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (renderer == nullptr)
	{
		fprintf(stderr, "SDL_CreateRenderer has failed! %s\n", SDL_GetError());
		exit(1);
	}

	SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);

	game_state_manager = new GameStateManager(renderer);
}

void update(float dt)
{
	game_state_manager->update(dt);
}

void render()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	game_state_manager->render();

	SDL_RenderPresent(renderer);
}

void handle_events()
{
	SDL_Event e;

	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
		{
			running = false;
			break;
		}
		game_state_manager->handle_events(&e);
	}
}

int main()
{
	init();

	float interval = 1000.0f / 60.0f;
	uint64_t last_frame = SDL_GetTicks64();
	uint64_t current_time;
	float dt;

	while (running)
	{
		current_time = SDL_GetTicks64();
		dt = (current_time - last_frame) / interval;
		last_frame = current_time;

		handle_events();
		update(dt);
		render();
	}

	delete game_state_manager;

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
