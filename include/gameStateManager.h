#pragma once
#include <SDL.h>
#include <vector>

class GameState;

class GameStateManager {
private:
	GameStateManager();
	inline static SDL_Renderer *m_Renderer = nullptr;
	inline static std::vector<GameState *> m_GameStates = {};

public:
	static void init(SDL_Renderer *renderer);
	static void destroy();
	static void handleEvents(SDL_Event *event);
	static void update(float dt);
	static void render();
	static void changeState(GameState *gameState);
	static void addState(GameState *gameState);
	static void popState();
};
