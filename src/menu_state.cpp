#include "menu_state.h"
#include "constants.h"
#include "font.h"
#include "select_state.h"
#include <algorithm>
#include <stdio.h>

MenuState MenuState::m_menu_state = MenuState();
Button MenuState::m_button = Button();

void MenuState::init()
{
	m_button = Button(
		renderer,
	 	SDL_Rect {.x = 12 * FONT_SIZE, .y = 7 * FONT_SIZE, .w = WIDTH - 24 * FONT_SIZE, .h = HEIGHT - 14 * FONT_SIZE}, 
		SDL_Color {.r = 0, .g = 150, .b = 0}
	);

	m_play_sound = Mix_LoadWAV("res/sfx/play.wav");

	// SDL_Log("%d, %d, %d, %d", m_button.get_button_box()->x, m_button.get_button_box()->y, m_button.get_button_box()->w, m_button.get_button_box()->h);

	m_timer = 0.0f;
	m_add_color = 0.0f;
}

void MenuState::destroy()
{
	Mix_FreeChunk(m_play_sound);
}

void MenuState::handle_events(SDL_Event *event)
{
	// if (event->type == SDL_KEYDOWN && event->key.keysym.scancode == SDL_SCANCODE_SPACE)
	// {
	// 	GameStateManager::add_state(SelectState::get());
	// }

	if (event->type == SDL_MOUSEBUTTONDOWN)
	{
		int x, y;
		if (SDL_GetMouseState(&x, &y) == 1)
		{	
			// scale x and y
			x = x * (float(WIDTH) / float(SCREEN_WIDTH));
			y = y * (float(HEIGHT) / float(SCREEN_HEIGHT));

			SDL_Point loc = {x, y};

			if (SDL_PointInRect(&loc, m_button.get_button_box()))
			{
				m_button.press();
			}
		}
	}
	else if (event->type == SDL_MOUSEBUTTONUP)
	{
		int x, y;
		SDL_GetMouseState(&x, &y);

		x = x * (float(WIDTH) / float(SCREEN_WIDTH));
		y = y * (float(HEIGHT) / float(SCREEN_HEIGHT));

		SDL_Point loc = {x, y};

		if (SDL_PointInRect(&loc, m_button.get_button_box()) && m_button.is_pressed())
		{
			Mix_PlayChannel(-1, m_play_sound, 0);
			GameStateManager::add_state(SelectState::get()); // switch to select state
		}
		m_button.release();
	}
}

void MenuState::update(float dt)
{
	m_timer += dt;
	m_add_color = dt * 3.0f;

	m_button.add_color((int)m_add_color, (int)m_add_color / 2, (int)m_add_color / 3);
}

void MenuState::render() const
{
	// for (int y = 0; y < HEIGHT; y++)
	// {
	// 	for (int x = 0; x < WIDTH; x++)
	// 	{
	// 		int i = y + x;
	// 		int j = i * 500 + int(m_timer) * 500;

	// 		// int r = j >> 16;
	// 		// int g = (j >> 8) & 0xFF;
	// 		int b = std::clamp((j & 0xFF) + 50, 0, 255); // cool colors :D

	// 		SDL_SetRenderDrawColor(renderer, 0, 0, b, 255);
	// 		SDL_RenderDrawPoint(renderer, x, y);
	// 	}
	// }

	uint32_t j = int(m_timer);
	int b = std::clamp((int32_t)((j & 0xFF) + 50), 0, 255);

	SDL_SetRenderDrawColor(renderer, 0, 0, b, 0xFF);
	SDL_RenderFillRect(renderer, NULL);

	m_button.render();

	FontRender::render_text("Paths!", WIDTH / 2 - 3 * FONT_SIZE + FONT_SIZE / 2, 3 * FONT_SIZE);
}

MenuState *MenuState::get()
{
	return &m_menu_state;
}
