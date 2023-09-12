#pragma once
#include "game_state_manager.h"

class GameState
{
protected:
	SDL_Renderer *m_renderer;
	GameStateManager *m_game_state_manager;

public:
	GameState(SDL_Renderer *renderer, GameStateManager *game_state_manager) : m_renderer(renderer), m_game_state_manager(game_state_manager) {}
	virtual ~GameState() {}
	virtual void handle_events(SDL_Event *event) = 0;
	virtual void update(float dt) = 0;
	virtual void render() const = 0;
};
