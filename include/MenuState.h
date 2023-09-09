#pragma once
#include "GameState.h"

class MenuState : public GameState
{
public:
	using GameState::GameState;
	~MenuState();
	void handle_events(SDL_Event *event) override;
	void update(float dt) override;
	void render() const override;
};
