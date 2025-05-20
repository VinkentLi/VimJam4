#pragma once
#include <memory>
#include <SDL_mixer.h>
#include "gameState.h"
#include "button.h"

class MenuState : public GameState {
private:
	MenuState() {}
	static MenuState m_MenuState;
	static Button m_Button;
	Mix_Chunk *m_PlaySound;
	float m_Timer;
	float m_AddColor;

public:
	void init() override;
	void destroy() override;
	void handleEvents(SDL_Event *event) override;
	void update(float dt) override;
	void render() const override;
	static MenuState *get();
};
