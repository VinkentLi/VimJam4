#pragma once
#include <SDL.h>

class GameState;

class GameStateManager
{
private:
	SDL_Renderer *m_renderer;
	GameState *m_current_state;

public:
	GameStateManager(SDL_Renderer *renderer);
	~GameStateManager();
	void handle_events(SDL_Event *event);
	void update(float dt);
	void render();
	void change_state(GameState *game_state);
};
