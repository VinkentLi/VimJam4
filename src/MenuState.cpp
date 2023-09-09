#include "MenuState.h"

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
	SDL_Rect rect = {0, 0, 320, 180};

	SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(m_renderer, &rect);
}
