#include "menuState.h"

void GameStateManager::init(SDL_Renderer *renderer) {
	m_Renderer = renderer;
}

void GameStateManager::destroy() {
	for (GameState *gs : m_GameStates) {
		gs->destroy();
	}
	m_GameStates.clear();
}

void GameStateManager::handleEvents(SDL_Event *event) {
	if (m_GameStates.empty()) {
		return;
	}
	m_GameStates.back()->handleEvents(event);
}

void GameStateManager::update(float dt) {
	if (m_GameStates.empty()) {
		return;
	}
	m_GameStates.back()->update(dt);
}

void GameStateManager::render() {
	m_GameStates.back()->render();
}

void GameStateManager::changeState(GameState *gameState) {
	if (m_GameStates.empty()) {
		m_GameStates.push_back(gameState);
		return;
	}
	m_GameStates.back()->destroy();
	m_GameStates.pop_back();
	m_GameStates.push_back(gameState);
	gameState->init();
}

void GameStateManager::addState(GameState *gameState) {
	m_GameStates.push_back(gameState);
	gameState->init();
}

void GameStateManager::popState() {
	if (m_GameStates.empty()) {
		return;
	}
	m_GameStates.back()->destroy();
	m_GameStates.pop_back();
}
