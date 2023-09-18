#include "play_state.h"
#include "select_state.h"
#include "enemy.h"
#include "font.h"
#include <SDL_image.h>
#include "constants.h"

PlayState PlayState::m_play_state = PlayState();

void PlayState::init()
{
    m_level = new Level();
    m_cam_pos = SDL_FPoint {.x = -3 * m_PLAYER_SIZE, .y = -5 * m_PLAYER_SIZE};
    m_player_pos = SDL_FPoint {.x = 0.0f, .y = 0.0f};
    m_latest_checkpoint_pos = SDL_FPoint {m_player_pos.x, m_player_pos.y};
    m_player_hitbox = SDL_FRect {.x = 5.0f, .y = 2.0f, .w = 6.0f, .h = 14.0f};
    m_player_x_velo = 0.0f;
    m_player_y_velo = 0.0f;
    m_invinc_timer = 0.0f;
    m_player_tex = IMG_LoadTexture(renderer, "res/gfx/player.png");
    m_checkpoint_tex = IMG_LoadTexture(renderer, "res/gfx/checkpoint.png");
    m_checkpoint_sound = Mix_LoadWAV("res/sfx/checkpoint.wav");
    m_coin_sound = Mix_LoadWAV("res/sfx/coin.wav");
    m_death_sound = Mix_LoadWAV("res/sfx/death.wav");
    m_jump_sound = Mix_LoadWAV("res/sfx/jump.wav");
    m_left_pressed = false;
    m_right_pressed = false;
    m_jump_pressed = false;
    m_sprint_pressed = false;
    m_grounded = false;
    m_jumping = false;
    m_paused = false;
    m_dead = false;
}

void PlayState::destroy()
{
    SDL_DestroyTexture(m_player_tex);
    SDL_DestroyTexture(m_checkpoint_tex);
    Mix_FreeChunk(m_checkpoint_sound);
    Mix_FreeChunk(m_coin_sound);
    Mix_FreeChunk(m_death_sound);
    Mix_FreeChunk(m_jump_sound);
    m_checkpoint_rects.clear();
    delete m_level;
}

void PlayState::kill()
{
    Mix_PlayChannel(-1, m_death_sound, 0);
    m_dead = true;
    m_player_pos = m_latest_checkpoint_pos;
    m_player_hitbox.x = m_player_pos.x + 5.0f;
    m_player_hitbox.y = m_player_pos.y + 2.0f;
    add_coins(-get_coins());
    m_player_x_velo = 0.0f;
    m_player_y_velo = 0.0f;
    m_invinc_timer = 60.0f;
}

void PlayState::handle_events(SDL_Event *event)
{
    if (event->type == SDL_KEYDOWN)
    {
        switch (event->key.keysym.scancode)
        {
        case SDL_SCANCODE_LEFT:
        case SDL_SCANCODE_A:
            m_left_pressed = true;
            break;
        case SDL_SCANCODE_RIGHT:
        case SDL_SCANCODE_D:
            m_right_pressed = true;
            break;
        case SDL_SCANCODE_UP:
        case SDL_SCANCODE_SPACE:
            m_dead = false;
            m_jump_pressed = true;
            break;
        case SDL_SCANCODE_LSHIFT:
        case SDL_SCANCODE_RSHIFT:
            m_sprint_pressed = true;
            break;
        default:
            break;
        }
    }
    else if (event->type == SDL_KEYUP)
    {
        switch (event->key.keysym.scancode)
        {
        case SDL_SCANCODE_LEFT:
        case SDL_SCANCODE_A:
            m_left_pressed = false;
            break;
        case SDL_SCANCODE_RIGHT:
        case SDL_SCANCODE_D:
            m_right_pressed = false;
            break;
        case SDL_SCANCODE_UP:
        case SDL_SCANCODE_SPACE:
            m_jump_pressed = false;
            break;
        case SDL_SCANCODE_LSHIFT:
        case SDL_SCANCODE_RSHIFT:
            m_sprint_pressed = false;
            break;
        case SDL_SCANCODE_ESCAPE:
            m_paused = !m_paused;
            break;
        case SDL_SCANCODE_C:
        {
            if (m_checkpoints <= 0)
            {
                break;
            }

            Mix_PlayChannel(-1, m_checkpoint_sound, 0);
            change_used_checkpoint();
            add_checkpoints(-1);
            m_latest_checkpoint_pos = SDL_FPoint {m_player_pos.x, m_player_pos.y};
            SDL_FRect checkpoint_rect = {.x = m_player_pos.x, .y = m_player_pos.y, .w = 16, .h = 16};
            m_checkpoint_rects.push_back(checkpoint_rect);
            break;
        }
        default:
            break;
        }
    }
}

void PlayState::update(float dt)
{
    if (m_paused || m_dead)
    {
        return;
    }

    m_invinc_timer -= dt;

    m_level->update(dt);

    float x_accel = m_X_ACCEL;

    if (!m_grounded)
    {
        x_accel /= 3.0f;
    }

    // move left
    if (m_left_pressed)
    {
        m_player_x_velo -= x_accel * dt;

        if (m_sprint_pressed && m_player_x_velo < -m_MAX_SPRINT_VELO)
        {
            m_player_x_velo = -m_MAX_SPRINT_VELO;
        }
        else if (!m_sprint_pressed && m_player_x_velo < -m_MAX_VELO)
        {
            m_player_x_velo = -m_MAX_VELO;
        }
    }

    // move right
    if (m_right_pressed)
    {
        m_player_x_velo += x_accel * dt;

        if (m_sprint_pressed && m_player_x_velo > m_MAX_SPRINT_VELO)
        {
            m_player_x_velo = m_MAX_SPRINT_VELO;
        }
        else if (!m_sprint_pressed && m_player_x_velo > m_MAX_VELO)
        {
            m_player_x_velo = m_MAX_VELO;
        }
    }

    // slow down if not moving in either dir
    if (((!m_left_pressed && !m_right_pressed) || (m_left_pressed && m_right_pressed)) && m_player_x_velo != 0.0f)
    {
        if (m_player_x_velo > 0.0f)
        {
            m_player_x_velo -= x_accel * dt;

            if (m_player_x_velo < 0.0f)
            {
                m_player_x_velo = 0.0f;
            }
        }
        else
        {
            m_player_x_velo += x_accel * dt;

            if (m_player_x_velo > 0.0f)
            {
                m_player_x_velo = 0.0f;
            }
        }
    }

    if (m_jump_pressed)
    {
        if (m_grounded)
        {
            Mix_PlayChannel(-1, m_jump_sound, 0);
            m_player_y_velo = m_JUMP_STRENGTH;
            m_jumping = true;
        }
        else if (m_jumping)
        {
            m_player_y_velo += m_GRAVITY * dt / 4.0f;
        }
        else
        {
            m_player_y_velo += m_GRAVITY * dt;
        }
    }
    else
    {
        m_player_y_velo += m_GRAVITY * dt;
    }

    m_grounded = false;

    if (m_player_y_velo > m_TERMINAL_VELO)
    {
        m_player_y_velo = m_TERMINAL_VELO;
    }

    // check y collisions
    m_player_hitbox.y += m_player_y_velo * dt;

    for (Object &o : m_level->get_objs())
    {
        SDL_FRect intersect;

        if (SDL_IntersectFRect(&m_player_hitbox, &o.obj, &intersect))
        {
            if (m_player_y_velo >= 0.0f)
            {
                m_player_hitbox.y -= intersect.h;
                m_grounded = true; // fun fact: without vsync, i think the game runs so fast that the dt variable multiplies the y accel to be basically zero, causing no collision to be detected and leading the game to believe that the player isnt grounded for sometime lol
                m_jumping = false;
            }
            else
            {
                m_player_hitbox.y += intersect.h;
            }

            m_player_y_velo = 0.0f;
        }
    }

    m_player_pos.y = m_player_hitbox.y - 2.0f;

    // check x collisions
    m_player_hitbox.x += m_player_x_velo * dt;

    for (Object &o : m_level->get_objs())
    {
        SDL_FRect intersect;

        if (SDL_IntersectFRect(&m_player_hitbox, &o.obj, &intersect))
        {
            if (m_player_x_velo >= 0.0f)
            {
                m_player_hitbox.x -= intersect.w;
            }
            else
            {
                m_player_hitbox.x += intersect.w;
            }

            m_player_x_velo = 0.0f;
        }
    }

    m_player_pos.x = m_player_hitbox.x - 5.0f;

    // check coin collisions
    std::vector<SDL_FRect> &coins = m_level->get_coins();

    for (auto it = coins.end() - 1; it >= coins.begin(); it--)
    {
        if (SDL_HasIntersectionF(&m_player_hitbox, it.base()))
        {
            Mix_PlayChannel(-1, m_coin_sound, 0);
            PlayState::add_coins(1);
            coins.erase(it);
        }
    }

    // check flag collision
    if (SDL_HasIntersectionF(&m_player_hitbox, m_level->get_flag()))
    {
        SelectState::get()->go_back_to_state();
        GameStateManager::pop_state();
    }

    if (m_player_pos.x - m_cam_pos.x > WIDTH / 2)
    {
        m_cam_pos.x = m_player_pos.x - WIDTH / 2;
    }

    if (m_player_pos.x - m_cam_pos.x < WIDTH / 3)
    {
        m_cam_pos.x = m_player_pos.x - WIDTH / 3;
    }

    if (m_player_pos.y > 300) // kill if in void
    {
        kill();
    }

    if (m_invinc_timer >= 0.0f)
    {
        return;
    }

    // check enemy collisions
    for (Enemy *e : m_level->get_enemies())
    {
        if (SDL_HasIntersectionF(&m_player_hitbox, e->get_hitbox()))
        {
            kill();
        }
    }
}

void PlayState::render() const
{
    if (m_dead)
    {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderFillRect(renderer, NULL);

        std::string text1 = "YOU DIED!";
        std::string text2 = "and lost all your coins!";
        std::string text3 = "press SPACE to go to your";
        std::string text4 = "last checkpoint!";

        FontRender::render_text(text1, WIDTH / 2 - text1.size() * FONT_SIZE / 2, HEIGHT / 2 - FONT_SIZE * 2);
        FontRender::render_text(text2, WIDTH / 2 - text2.size() * FONT_SIZE / 2, HEIGHT / 2 - FONT_SIZE);
        FontRender::render_text(text3, WIDTH / 2 - text3.size() * FONT_SIZE / 2, HEIGHT / 2);
        FontRender::render_text(text4, WIDTH / 2 - text4.size() * FONT_SIZE / 2, HEIGHT / 2 + FONT_SIZE);

        return;
    }

    m_level->render(m_cam_pos);

    for (SDL_FRect checkpoint : m_checkpoint_rects)
    {
        SDL_FRect dst = checkpoint;
        dst.x -= m_cam_pos.x;
        dst.y -= m_cam_pos.y;

        SDL_RenderCopyF(renderer, m_checkpoint_tex, NULL, &dst);
    }

    SDL_Rect dst = SDL_Rect {
        .x = int(m_player_pos.x - m_cam_pos.x),
        .y = int(m_player_pos.y - m_cam_pos.y), 
        .w = m_PLAYER_SIZE, 
        .h = m_PLAYER_SIZE
    };

    SDL_RenderCopy(renderer, m_player_tex, NULL, &dst);

    int LEVEL_INDEX = SelectState::get()->get_cur_lvl();
    const std::string top_text = "LVL COIN CHKPNT";

    std::string bottom_text = " ";
    bottom_text += char(LEVEL_INDEX / 10 + '0');
    bottom_text += char(LEVEL_INDEX % 10 + '0');
    bottom_text += "  ";
    bottom_text += char(m_coins / 100 + '0');
    bottom_text += char((m_coins % 100) / 10 + '0');
    bottom_text += char(m_coins % 10 + '0');
    bottom_text += "    ";
    bottom_text += char(m_checkpoints / 100 + '0');
    bottom_text += char((m_checkpoints % 100) / 10 + '0');
    bottom_text += char(m_checkpoints % 10 + '0');

    FontRender::render_text(top_text, WIDTH / 2 - top_text.size() * FONT_SIZE / 2, 0);
    FontRender::render_text(bottom_text, WIDTH / 2 - top_text.size() * FONT_SIZE / 2, FONT_SIZE);

    if (!m_used_checkpoint)
    {
        std::string text = "PRESS \"C\" to place a checkpoint!";
        FontRender::render_text(text, 1, HEIGHT - FONT_SIZE - 1);
    }

    if (m_paused)
    {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x80);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderFillRect(renderer, NULL);

        const std::string text = "PAUSED";

        FontRender::render_text(text, WIDTH / 2 - text.size() * FONT_SIZE / 2, HEIGHT / 2 - FONT_SIZE / 2);
    }
}

void PlayState::change_used_checkpoint()
{
    m_used_checkpoint = true;
}

void PlayState::add_coins(int n)
{
    m_coins += n;
}

void PlayState::add_checkpoints(int n)
{
    m_checkpoints += n;
}

int PlayState::get_coins()
{
    return m_coins;
}

int PlayState::get_checkpoints()
{
    return m_checkpoints;
}

PlayState *PlayState::get()
{
    return &m_play_state;
}

SDL_Texture *PlayState::get_checkpoint_tex()
{
    return m_checkpoint_tex;
}
