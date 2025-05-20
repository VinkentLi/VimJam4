#pragma once
#include <SDL.h>
#include <vector>

struct Object;

enum EnemyType {
    SPRINTER,
    JUMPER
};

enum Dir {
    LEFT,
    RIGHT
};

class Enemy {
private:
    std::vector<Object> *m_Objects;
    SDL_Texture *m_Texture;
    SDL_FPoint m_Position;
    SDL_FRect m_Hitbox;
    float m_YVelocity;
    static constexpr int m_SIZE = 16;
    static constexpr float m_GRAVITY = 0.4f;
    static constexpr float m_JUMP_STRENGTH = -5.0f;
    static constexpr float m_VELO = 2.0f;
    static constexpr float m_TERMINAL_VELO = 6.5f;
    int m_Type;
    int m_Direction;
    bool m_Grounded;
    bool m_Jumping;
    float m_TimeSinceLastJump;

public:
    Enemy(std::vector<Object> *objs, SDL_FPoint pos);
    ~Enemy();
    void update(float dt);
    void render(SDL_FPoint cam_pos) const;
    SDL_FRect *getHitbox();
};
