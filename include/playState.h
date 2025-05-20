#pragma once
#include <SDL_mixer.h>
#include "gameState.h"
#include "level.h"

class PlayState : public GameState {
private:
    PlayState() {}
    static PlayState m_PlayState;
    Level *m_Level;
    SDL_FPoint m_CameraPosition;
    SDL_FPoint m_PlayerPosition;
    SDL_FPoint m_LatestCheckpointPosition;
    std::vector<SDL_FRect> m_CheckpointRects;
    SDL_FRect m_PlayerHitbox;
    float m_PlayerXVelocity;
    float m_PlayerYVelocity;
    float m_InvincibilityTimer;
    inline static bool m_UsedCheckpoint = false;
    inline static int m_Coins = 6;
    inline static int m_Checkpoints = 5;
    static constexpr int m_PLAYER_SIZE = 16;
    static constexpr float m_X_ACCEL = 0.2f;
    static constexpr float m_GRAVITY = 0.4f;
    static constexpr float m_JUMP_STRENGTH = -2.7f;
    static constexpr float m_MAX_VELO = 1.0f;
    static constexpr float m_TERMINAL_VELO = 6.5f;
    static constexpr float m_MAX_SPRINT_VELO = 2.0f;
    SDL_Texture *m_PlayerTexture;
    SDL_Texture *m_CheckpointTexture;
    Mix_Chunk *m_CheckpointSound;
    Mix_Chunk *m_CoinSound;
    Mix_Chunk *m_DeathSound;
    Mix_Chunk *m_JumpSound;
    bool m_LeftPressed, m_RightPressed, m_JumpPressed, m_SprintPressed;
    bool m_Grounded;
    bool m_Jumping;
    bool m_Paused;
    bool m_Dead;

    void kill();

public:
    void init() override;
	void destroy() override;
	void handleEvents(SDL_Event *event) override;
	void update(float dt) override;
	void render() const override;
    void changeUsedCheckpoint();
    static void addCoins(int n);
    static void addCheckpoints(int n);
    static int getCoins();
    static int getCheckpoints();
    static PlayState *get();
    SDL_Texture *getCheckpointTexture();
};
