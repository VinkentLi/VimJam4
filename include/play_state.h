#pragma once
#include "game_state.h"
#include "level.h"

class PlayState : public GameState
{
private:
    PlayState() {}

    static PlayState m_play_state;
    Level *m_level;
    SDL_FPoint m_cam_pos;
    SDL_FPoint m_player_pos;
    SDL_FPoint m_latest_checkpoint_pos;
    std::vector<SDL_FRect> m_checkpoint_rects;
    SDL_FRect m_player_hitbox;
    float m_player_x_velo;
    float m_player_y_velo;
    float m_invinc_timer;
    inline static bool m_used_checkpoint = false;
    inline static int m_coins = 6;
    inline static int m_checkpoints = 5;
    static constexpr int m_PLAYER_SIZE = 16;
    static constexpr float m_X_ACCEL = 0.2f;
    static constexpr float m_GRAVITY = 0.4f;
    static constexpr float m_JUMP_STRENGTH = -2.7f;
    static constexpr float m_MAX_VELO = 1.0f;
    static constexpr float m_TERMINAL_VELO = 6.5f;
    static constexpr float m_MAX_SPRINT_VELO = 2.0f;
    SDL_Texture *m_player_tex;
    SDL_Texture *m_checkpoint_tex;
    bool m_left_pressed, m_right_pressed, m_jump_pressed, m_sprint_pressed;
    bool m_grounded;
    bool m_jumping;
    bool m_paused;
    bool m_dead;

    void kill();

public:
    void init() override;
	void destroy() override;
	void handle_events(SDL_Event *event) override;
	void update(float dt) override;
	void render() const override;
    void change_used_checkpoint();
    static void add_coins(int n);
    static void add_checkpoints(int n);
    static int get_coins();
    static int get_checkpoints();
    static PlayState *get();
    SDL_Texture *get_checkpoint_tex();
};
