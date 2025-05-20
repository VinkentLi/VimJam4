#pragma once
#include "gameStateManager.h"

class GameState
{
public:
	virtual void init() = 0;
	virtual void destroy() = 0;
	virtual void handleEvents(SDL_Event *event) = 0;
	virtual void update(float dt) = 0;
	virtual void render() const = 0;
};
