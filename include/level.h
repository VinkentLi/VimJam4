#pragma once
#include <SDL.h>
#include <vector>

class Enemy;

struct Object {
    SDL_FRect obj;
    SDL_Color color;
    int layer;
};

class Level {
private:
    std::vector<Object> m_Objects;
    std::vector<SDL_FRect> m_Coins;
    std::vector<Enemy *> m_Enemies;
    SDL_FRect m_LevelCompleteFlag;
    SDL_Texture *m_LevelCompleteTexture;
    static constexpr int COIN_SIZE = 16;
    static constexpr int FLAG_SIZE = 16;
    static constexpr int COIN_RENDER_SIZE = 8;

public:
    Level();
    ~Level();
    void update(float dt);
    void render(SDL_FPoint cameraPosition) const;
    std::vector<Enemy *> &getEnemies();
    std::vector<Object> &getObjects();
    std::vector<SDL_FRect> &getCoins();
    SDL_FRect *getFlag();
};
