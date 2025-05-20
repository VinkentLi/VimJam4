#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include "gameStateManager.h"
#include "menuState.h"
#include "constants.h"
#include "font.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

void init() {
	srand(time(NULL));

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "SDL_Init(SDL_INIT_VIDEO) has failed! %s\n", SDL_GetError());
		exit(1);
	}

	if (!IMG_Init(IMG_INIT_PNG)) {
		fprintf(stderr, "IMG_Init(IMG_INIT_PNG) has failed! %s\n", SDL_GetError());
		exit(1);
	}

	Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 2048);

	window = SDL_CreateWindow(
		"Game",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN
	);

	if (window == nullptr) {
		fprintf(stderr, "SDL_CreateWindow has failed! %s\n", SDL_GetError());
		exit(1);
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (renderer == nullptr) {
		fprintf(stderr, "SDL_CreateRenderer has failed! %s\n", SDL_GetError());
		exit(1);
	}

	SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);

	FontRender::init(renderer);
	GameStateManager::init(renderer);
	GameStateManager::addState(MenuState::get());
}

void update(float dt) {
	GameStateManager::update(dt);
}

void render() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	GameStateManager::render();
	SDL_RenderPresent(renderer);
}

void handleEvents() {
	SDL_Event e;

	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) {
			FontRender::destroy();
			GameStateManager::destroy();
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			SDL_Quit();
			exit(EXIT_SUCCESS);
#ifdef __EMSCRIPTEN__
			emscripten_cancel_main_loop();
#endif
			break;
		}
		GameStateManager::handleEvents(&e);
	}
}

#ifdef __EMSCRIPTEN__
void mainLoop() {
    handleEvents();
    update(1);
    render();
}
#else
void mainLoop(float delta) {
    handleEvents();
    update(delta);
    render();
}
#endif

int main(int argc, char *argv[]) {
	init();
	constexpr float interval = 1000.0f / 60.0f;
	uint64_t lastFrame = SDL_GetTicks64();
	uint64_t currentTime;
	float dt;
	int frames = 0;
	float timer = 0;

#ifndef __EMSCRIPTEN__
	while (true) {
		currentTime = SDL_GetTicks64();
		dt = (currentTime - lastFrame) / interval;
		lastFrame = currentTime;

		timer += dt / 60.0f;
		if (timer >= 1.0f) {
			timer--;
			SDL_SetWindowTitle(window, std::to_string(frames).c_str());
			frames = 0;
		}
		mainLoop(dt);
		frames++;
	}
#else
	emscripten_set_main_loop(mainLoop, 60, 1);
#endif
	return 0;
}
