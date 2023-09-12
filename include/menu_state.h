#pragma once
#include <memory>
#include "game_state.h"
#include "button.h"

class MenuState : public GameState
{
private:
	std::unique_ptr<Button> m_button;

public:
	MenuState(SDL_Renderer *renderer, GameStateManager *game_state_manager);
	~MenuState() override {};
	void handle_events(SDL_Event *event) override;
	void update(float dt) override;
	void render() const override;
};
