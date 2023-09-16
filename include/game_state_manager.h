#pragma once
#include <SDL.h>
#include <vector>

class GameState;

class GameStateManager
{
private:
	GameStateManager();
	inline static SDL_Renderer *m_renderer = nullptr;
	inline static std::vector<GameState *> m_game_states = {};

public:
	static void init(SDL_Renderer *renderer);
	static void destroy();
	static void handle_events(SDL_Event *event);
	static void update(float dt);
	static void render();
	static void change_state(GameState *game_state);
	static void add_state(GameState *game_state);
	static void pop_state();
};
