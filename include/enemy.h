#pragma once
#include <SDL.h>
#include <vector>

struct Object;

enum EnemyType
{
    SPRINTER,
    JUMPER
};

enum Dir
{
    LEFT,
    RIGHT
};

class Enemy
{
private:
    std::vector<Object> *m_objs;
    SDL_Texture *m_tex;
    SDL_FPoint m_pos;
    SDL_FRect m_hitbox;
    float m_y_velo;
    static constexpr int m_SIZE = 16;
    static constexpr float m_GRAVITY = 0.4f;
    static constexpr float m_JUMP_STRENGTH = -5.0f;
    static constexpr float m_VELO = 2.0f;
    static constexpr float m_TERMINAL_VELO = 6.5f;
    int m_type;
    int m_dir;
    bool m_grounded;
    bool m_jumping;
    float m_time_since_last_jump;

public:
    Enemy(std::vector<Object> *objs, SDL_FPoint pos);
    ~Enemy();
    void update(float dt);
    void render(SDL_FPoint cam_pos) const;
    SDL_FRect *get_hitbox();
};
