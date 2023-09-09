#pragma once
#include "game_state.h"

class MenuState : public GameState
{
private:
	

public:
	MenuState(SDL_Renderer *renderer, GameStateManager* game_state_manager);
	~MenuState();
	void handle_events(SDL_Event *event) override;
	void update(float dt) override;
	void render() const override;
};
