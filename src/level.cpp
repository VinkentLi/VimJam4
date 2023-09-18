#include "level.h"
#include "select_state.h"
#include "enemy.h"
#include "constants.h"
#include <string>
#include <fstream>
#include <algorithm>
#include <SDL_image.h>

Level::Level()
{
    m_lvl_complete_tex = IMG_LoadTexture(renderer, "res/gfx/flag.png");

    int level_n = SelectState::get()->get_cur_lvl();
    std::string level_file = "res/lvl/" + std::to_string(level_n) + ".lvl";

    std::ifstream in(level_file);

    int objs;
    in >> objs;

    SDL_Log("%d", objs);

    for (int o = 0; o < objs; o++)
    {
        Object obj;
        SDL_FRect obj_rect;
        in >> obj_rect.x >> obj_rect.y >> obj_rect.w >> obj_rect.h;
        obj.obj = obj_rect;

        int r, g, b;
        in >> r >> g >> b;

        obj.color = SDL_Color {.r = (uint8_t)r, .g = (uint8_t)g, .b = (uint8_t)b};

        int layer;
        in >> layer;
        obj.layer = layer;

        m_objs.push_back(obj);
    }

    std::sort(m_objs.begin(), m_objs.end(), [] (const Object &a, const Object &b) -> bool
    {
        return a.layer < b.layer;
    });

    int num_coins;

    in >> num_coins;

    for (int c = 0; c < num_coins; c++)
    {
        SDL_FPoint coin_pos;
        in >> coin_pos.x >> coin_pos.y;

        m_coins.push_back(SDL_FRect {.x = coin_pos.x, .y = coin_pos.y, .w = COIN_SIZE, .h = COIN_SIZE});
    }

    in >> m_lvl_complete_flag.x >> m_lvl_complete_flag.y;
    m_lvl_complete_flag.w = FLAG_SIZE;
    m_lvl_complete_flag.h = FLAG_SIZE;

    int num_enemies;

    in >> num_enemies;

    SDL_Log("num_enemies: %d", num_enemies);

    for (int e = 0; e < num_enemies; e++)
    {
        SDL_FPoint enemy_pos;
        in >> enemy_pos.x >> enemy_pos.y;

        SDL_Log("x: %f, y:%f", enemy_pos.x, enemy_pos.y);

        m_enemies.push_back(new Enemy(&m_objs, enemy_pos));
    }

    in.close();
}

Level::~Level()
{
    SDL_DestroyTexture(m_lvl_complete_tex);

    for (Enemy *e : m_enemies)
    {
        delete e;
    }

    m_enemies.clear();
}

void Level::update(float dt)
{
    for (Enemy *e : m_enemies)
    {
        e->update(dt);
    }
}

void Level::render(SDL_FPoint cam_pos) const
{
    SDL_SetRenderDrawColor(renderer, 0x87, 0xCE, 0xEB, 0xFF);
    SDL_RenderFillRect(renderer, NULL);

    for (Object object : m_objs)
    {
        SDL_FRect dst = object.obj;
        dst.x -= cam_pos.x;
        dst.y -= cam_pos.y;

        SDL_SetRenderDrawColor(renderer, object.color.r, object.color.g, object.color.b, 0xFF);
        SDL_RenderFillRectF(renderer, &dst);
    }

    for (SDL_FRect coin : m_coins)
    {
        SDL_FRect dst = SDL_FRect {.x = coin.x + 4, .y = coin.y + 4, .w = COIN_RENDER_SIZE, .h = COIN_RENDER_SIZE};
        dst.x -= cam_pos.x;
        dst.y -= cam_pos.y;

        SDL_SetRenderDrawColor(renderer, 0xFF, 0xD7, 0x00, 0xFF);
        SDL_RenderFillRectF(renderer, &dst);
    }

    for (Enemy *e : m_enemies)
    {
        e->render(cam_pos);
    }

    SDL_FRect flag_dst = m_lvl_complete_flag;
    flag_dst.x -= cam_pos.x;
    flag_dst.y -= cam_pos.y;

    SDL_RenderCopyF(renderer, m_lvl_complete_tex, NULL, &flag_dst);
}

std::vector<Enemy *> &Level::get_enemies()
{
    return m_enemies;
}

std::vector<Object> &Level::get_objs()
{
    return m_objs;
}

std::vector<SDL_FRect> &Level::get_coins()
{
    return m_coins;
}

SDL_FRect *Level::get_flag()
{
    return &m_lvl_complete_flag;
}
