#include "menu_state.h"

MenuState::MenuState(SDL_Renderer *renderer, GameStateManager* game_state_manager)
	: GameState::GameState(renderer, game_state_manager)
{
	m_button = new Button(m_renderer, SDL_Rect {.x = 50, .y = 50, .w = 220, .h = 100}, "res/gfx/grass.png");
}

MenuState::~MenuState()
{
}

void MenuState::handle_events(SDL_Event *event)
{
}

void MenuState::update(float dt)
{
}

void MenuState::render() const
{
	m_button->render();
}
