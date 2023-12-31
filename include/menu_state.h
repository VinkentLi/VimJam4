#pragma once
#include <memory>
#include <SDL_mixer.h>
#include "game_state.h"
#include "button.h"

class MenuState : public GameState
{
private:
	MenuState() {}

	static MenuState m_menu_state;
	static Button m_button;
	Mix_Chunk *m_play_sound;
	float m_timer;
	float m_add_color;

public:
	void init() override;
	void destroy() override;
	void handle_events(SDL_Event *event) override;
	void update(float dt) override;
	void render() const override;
	static MenuState *get();
};
