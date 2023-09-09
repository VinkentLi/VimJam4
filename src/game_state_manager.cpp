#include "menu_state.h"

GameStateManager::GameStateManager(SDL_Renderer *renderer) : m_renderer(renderer)
{
	m_current_state = new MenuState(renderer, this);
}

GameStateManager::~GameStateManager()
{
	if (m_current_state != nullptr) 
	{
		delete m_current_state;
	}
}

void GameStateManager::handle_events(SDL_Event *event)
{
	m_current_state->handle_events(event);
}

void GameStateManager::update(float dt)
{
	m_current_state->update(dt);
}

void GameStateManager::render()
{
	m_current_state->render();
}

void GameStateManager::change_state(GameState *game_state)
{
	if (m_current_state == nullptr)
	{
		return;
	}

	delete m_current_state;
	m_current_state = game_state;
}
