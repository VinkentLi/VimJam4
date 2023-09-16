#include <SDL.h>
#include <stdio.h>
#include <memory>
#include "game_state_manager.h"
#include "menu_state.h"
#include "constants.h"
#include "font.h"

#undef main

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

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

	FontRender::init(renderer);
	GameStateManager::init(renderer);
	GameStateManager::add_state(MenuState::get());
}

void update(float dt)
{
	GameStateManager::update(dt);
}

void render()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	GameStateManager::render();

	SDL_RenderPresent(renderer);
}

void handle_events()
{
	SDL_Event e;

	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
		{
			FontRender::destroy();
			GameStateManager::destroy();
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			SDL_Quit();
			exit(EXIT_SUCCESS);

			break;
		}
		GameStateManager::handle_events(&e);
	}
}

int main()
{
	init();

	float interval = 1000.0f / 60.0f;
	uint64_t last_frame = SDL_GetTicks64();
	uint64_t current_time;
	float dt;

	while (true)
	{
		current_time = SDL_GetTicks64();
		dt = (current_time - last_frame) / interval;
		last_frame = current_time;

		handle_events();
		update(dt);
		render();
	}

	return 0;
}
