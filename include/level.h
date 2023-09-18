#pragma once
#include <SDL.h>
#include <vector>

class Enemy;

struct Object
{
    SDL_FRect obj;
    SDL_Color color;
    int layer;
};

class Level
{
private:
    std::vector<Object> m_objs;
    std::vector<SDL_FRect> m_coins;
    std::vector<Enemy *> m_enemies;
    SDL_FRect m_lvl_complete_flag;
    SDL_Texture *m_lvl_complete_tex;
    static constexpr int COIN_SIZE = 16;
    static constexpr int FLAG_SIZE = 16;
    static constexpr int COIN_RENDER_SIZE = 8;

public:
    Level();
    ~Level();
    void update(float dt);
    void render(SDL_FPoint cam_pos) const;
    std::vector<Enemy *> &get_enemies();
    std::vector<Object> &get_objs();
    std::vector<SDL_FRect> &get_coins();
    SDL_FRect *get_flag();
};
