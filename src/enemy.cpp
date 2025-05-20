#include "enemy.h"
#include "level.h"
#include "constants.h"
#include <SDL_image.h>

Enemy::Enemy(std::vector<Object> *objs, SDL_FPoint pos) : m_Objects(objs), m_Position(pos) {
    m_Texture = IMG_LoadTexture(renderer, "res/gfx/enemy.png");
    m_Hitbox = SDL_FRect {.x = m_Position.x + 3, .y = m_Position.y + 3, .w = 10, .h = 10};
    m_YVelocity = 0.0f;
    m_Type = rand() % 2;
    m_Direction = LEFT;
    m_Grounded = false;
    m_Jumping = false;
    m_TimeSinceLastJump = 0.0f;
}

Enemy::~Enemy() {
    SDL_DestroyTexture(m_Texture);
}

void Enemy::update(float dt) {
    if (m_Type == SPRINTER) {
        m_Hitbox.x += (m_Direction * 2 - 1) * m_VELO * dt;
    } else if (m_Type == JUMPER) {
        m_TimeSinceLastJump += dt;
        if (m_TimeSinceLastJump >= 30.0f && m_Grounded) {
            m_Direction = rand() % 2;
            m_TimeSinceLastJump = 0.0f;
            m_Jumping = true;
            m_YVelocity = m_JUMP_STRENGTH;
        }

        if (m_Jumping) {
            m_TimeSinceLastJump -= dt;
            m_Hitbox.x += (m_Direction * 2 - 1) * m_VELO * dt;
        }
    }
    m_YVelocity += m_GRAVITY * dt;
    m_Grounded = false;

    // check x collisions
    for (Object &o : *m_Objects) {
        SDL_FRect intersect;
        if (SDL_IntersectFRect(&m_Hitbox, &o.obj, &intersect)) {
            if (m_Direction == RIGHT) {
                m_Hitbox.x -= intersect.w;
            } else {
                m_Hitbox.x += intersect.w;
            }
            m_Direction = !m_Direction;
        }
    }
    m_Hitbox.y += m_YVelocity * dt;
    // check y collisions
    for (Object &o : *m_Objects) {
        SDL_FRect intersect;

        if (SDL_IntersectFRect(&m_Hitbox, &o.obj, &intersect)) {
            if (m_YVelocity >= 0.0f) {
                m_Hitbox.y -= intersect.h;
                m_Grounded = true;
                m_Jumping = false;
            } else {
                m_Hitbox.y += intersect.h;
            }
            m_YVelocity = 0.0f;
        }
    }
    m_Position.x = m_Hitbox.x - 3;
    m_Position.y = m_Hitbox.y - 3;
}

void Enemy::render(SDL_FPoint cam_pos) const {
    SDL_FRect dst = SDL_FRect {.x = m_Position.x - cam_pos.x, .y = m_Position.y - cam_pos.y, .w = m_SIZE, .h = m_SIZE};
    SDL_RenderCopyF(renderer, m_Texture, NULL, &dst);
}

SDL_FRect *Enemy::getHitbox() {
    return &m_Hitbox;
}
