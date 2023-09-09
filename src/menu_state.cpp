#include "menu_state.h"

MenuState::MenuState(SDL_Renderer *renderer, GameStateManager* game_state_manager)
	: GameState::GameState(renderer, game_state_manager)
{
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
}
