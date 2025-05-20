#include "menuState.h"
#include "constants.h"
#include "font.h"
#include "selectState.h"
#include <algorithm>
#include <stdio.h>

MenuState MenuState::m_MenuState = MenuState();
Button MenuState::m_Button = Button();

void MenuState::init() {
	m_Button = Button(
		renderer,
	 	SDL_Rect {.x = 12 * FONT_SIZE, .y = 7 * FONT_SIZE, .w = WIDTH - 24 * FONT_SIZE, .h = HEIGHT - 14 * FONT_SIZE}, 
		SDL_Color {.r = 0, .g = 150, .b = 0}
	);
	m_PlaySound = Mix_LoadWAV("res/sfx/play.wav");

	// SDL_Log("%d, %d, %d, %d", m_button.getButtonBox()->x, m_button.getButtonBox()->y, m_button.getButtonBox()->w, m_button.getButtonBox()->h);

	m_Timer = 0.0f;
	m_AddColor = 0.0f;
}

void MenuState::destroy() {
	Mix_FreeChunk(m_PlaySound);
}

void MenuState::handleEvents(SDL_Event *event) {
	// if (event->type == SDL_KEYDOWN && event->key.keysym.scancode == SDL_SCANCODE_SPACE)
	// {
	// 	GameStateManager::addState(SelectState::get());
	// }
	if (event->type == SDL_MOUSEBUTTONDOWN) {
		int x, y;
		if (SDL_GetMouseState(&x, &y) == 1) {	
			// scale x and y
			x = x * (float(WIDTH) / float(SCREEN_WIDTH));
			y = y * (float(HEIGHT) / float(SCREEN_HEIGHT));

			SDL_Point loc = {x, y};

			if (SDL_PointInRect(&loc, m_Button.getButtonBox())) {
				m_Button.press();
			}
		}
	} else if (event->type == SDL_MOUSEBUTTONUP) {
		int x, y;
		SDL_GetMouseState(&x, &y);

		x = x * (float(WIDTH) / float(SCREEN_WIDTH));
		y = y * (float(HEIGHT) / float(SCREEN_HEIGHT));

		SDL_Point loc = {x, y};

		if (SDL_PointInRect(&loc, m_Button.getButtonBox()) && m_Button.isPressed()) {
			Mix_PlayChannel(-1, m_PlaySound, 0);
			GameStateManager::addState(SelectState::get()); // switch to select state
		}
		m_Button.release();
	}
}

void MenuState::update(float dt) {
	m_Timer += dt;
	m_AddColor = dt * 3.0f;
	m_Button.addColor((int)m_AddColor, (int)m_AddColor / 2, (int)m_AddColor / 3);
}

void MenuState::render() const {
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

	uint32_t j = int(m_Timer);
	int b = std::clamp((int32_t)((j & 0xFF) + 50), 0, 255);

	SDL_SetRenderDrawColor(renderer, 0, 0, b, 0xFF);
	SDL_RenderFillRect(renderer, NULL);

	m_Button.render();

	FontRender::renderText("Paths!", WIDTH / 2 - 3 * FONT_SIZE + FONT_SIZE / 2, 3 * FONT_SIZE);
}

MenuState *MenuState::get() {
	return &m_MenuState;
}
