#include "menu_state.h"

void GameStateManager::init(SDL_Renderer *renderer)
{
	m_renderer = renderer;
}

void GameStateManager::destroy()
{
	for (GameState *gs : m_game_states)
	{
		gs->destroy();
	}
	m_game_states.clear();
}

void GameStateManager::handle_events(SDL_Event *event)
{
	if (m_game_states.empty())
	{
		return;
	}

	m_game_states.back()->handle_events(event);
}

void GameStateManager::update(float dt)
{
	if (m_game_states.empty())
	{
		return;
	}

	m_game_states.back()->update(dt);
}

void GameStateManager::render()
{
	m_game_states.back()->render();
}

void GameStateManager::change_state(GameState *game_state)
{
	if (m_game_states.empty())
	{
		m_game_states.push_back(game_state);
		return;
	}

	m_game_states.back()->destroy();
	m_game_states.pop_back();
	m_game_states.push_back(game_state);
	game_state->init();
}

void GameStateManager::add_state(GameState *game_state)
{
	m_game_states.push_back(game_state);
	game_state->init();
}

void GameStateManager::pop_state()
{
	if (m_game_states.empty())
	{
		return;
	}

	m_game_states.back()->destroy();
	m_game_states.pop_back();
}
