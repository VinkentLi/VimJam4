#include "enemy.h"
#include "level.h"
#include "constants.h"
#include <SDL_image.h>

Enemy::Enemy(std::vector<Object> *objs, SDL_FPoint pos) : m_objs(objs), m_pos(pos)
{
    m_tex = IMG_LoadTexture(renderer, "res/gfx/enemy.png");
    m_hitbox = SDL_FRect {.x = m_pos.x + 3, .y = m_pos.y + 3, .w = 10, .h = 10};
    m_y_velo = 0.0f;
    m_type = rand() % 2;
    m_dir = LEFT;
    m_grounded = false;
    m_jumping = false;
    m_time_since_last_jump = 0.0f;
}

Enemy::~Enemy()
{
    SDL_DestroyTexture(m_tex);
}

void Enemy::update(float dt)
{
    if (m_type == SPRINTER)
    {
        m_hitbox.x += (m_dir * 2 - 1) * m_VELO * dt;
    }
    else if (m_type == JUMPER)
    {
        m_time_since_last_jump += dt;

        if (m_time_since_last_jump >= 30.0f && m_grounded)
        {
            m_dir = rand() % 2;
            m_time_since_last_jump = 0.0f;
            m_jumping = true;
            m_y_velo = m_JUMP_STRENGTH;
        }

        if (m_jumping)
        {
            m_time_since_last_jump -= dt;
            m_hitbox.x += (m_dir * 2 - 1) * m_VELO * dt;
        }
    }

    m_y_velo += m_GRAVITY * dt;

    m_grounded = false;

    // check x collisions
    for (Object &o : *m_objs)
    {
        SDL_FRect intersect;

        if (SDL_IntersectFRect(&m_hitbox, &o.obj, &intersect))
        {
            if (m_dir == RIGHT)
            {
                m_hitbox.x -= intersect.w;
            }
            else
            {
                m_hitbox.x += intersect.w;
            }

            m_dir = !m_dir;
        }
    }

    m_hitbox.y += m_y_velo * dt;

    // check y collisions
    for (Object &o : *m_objs)
    {
        SDL_FRect intersect;

        if (SDL_IntersectFRect(&m_hitbox, &o.obj, &intersect))
        {
            if (m_y_velo >= 0.0f)
            {
                m_hitbox.y -= intersect.h;
                m_grounded = true;
                m_jumping = false;
            }
            else
            {
                m_hitbox.y += intersect.h;
            }

            m_y_velo = 0.0f;
        }
    }

    m_pos.x = m_hitbox.x - 3;
    m_pos.y = m_hitbox.y - 3;
}

void Enemy::render(SDL_FPoint cam_pos) const
{
    SDL_FRect dst = SDL_FRect {.x = m_pos.x - cam_pos.x, .y = m_pos.y - cam_pos.y, .w = m_SIZE, .h = m_SIZE};

    SDL_RenderCopyF(renderer, m_tex, NULL, &dst);
}

SDL_FRect *Enemy::get_hitbox()
{
    return &m_hitbox;
}
