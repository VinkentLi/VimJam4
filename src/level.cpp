#include "level.h"
#include "selectState.h"
#include "enemy.h"
#include "constants.h"
#include <string>
#include <fstream>
#include <algorithm>
#include <SDL_image.h>

Level::Level() {
    m_LevelCompleteTexture = IMG_LoadTexture(renderer, "res/gfx/flag.png");

    int levelN = SelectState::get()->getCurrentLevel();
    std::string levelFile = "res/lvl/" + std::to_string(levelN) + ".lvl";

    std::ifstream in(levelFile);

    int objs;
    in >> objs;

    SDL_Log("%d", objs);

    for (int o = 0; o < objs; o++) {
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

        m_Objects.push_back(obj);
    }

    std::sort(m_Objects.begin(), m_Objects.end(), [] (const Object &a, const Object &b) -> bool {
        return a.layer < b.layer;
    });

    int numCoins;
    in >> numCoins;

    for (int c = 0; c < numCoins; c++)
    {
        SDL_FPoint coinPosition;
        in >> coinPosition.x >> coinPosition.y;

        m_Coins.push_back(SDL_FRect {.x = coinPosition.x, .y = coinPosition.y, .w = COIN_SIZE, .h = COIN_SIZE});
    }

    in >> m_LevelCompleteFlag.x >> m_LevelCompleteFlag.y;
    m_LevelCompleteFlag.w = FLAG_SIZE;
    m_LevelCompleteFlag.h = FLAG_SIZE;

    int numEnemies;
    in >> numEnemies;
    SDL_Log("num_enemies: %d", numEnemies);

    for (int e = 0; e < numEnemies; e++) {
        SDL_FPoint enemyPosition;
        in >> enemyPosition.x >> enemyPosition.y;
        SDL_Log("x: %f, y:%f", enemyPosition.x, enemyPosition.y);
        m_Enemies.push_back(new Enemy(&m_Objects, enemyPosition));
    }

    in.close();
}

Level::~Level() {
    SDL_DestroyTexture(m_LevelCompleteTexture);
    for (Enemy *e : m_Enemies) {
        delete e;
    }
    m_Enemies.clear();
}

void Level::update(float dt) {
    for (Enemy *e : m_Enemies) {
        e->update(dt);
    }
}

void Level::render(SDL_FPoint cameraPosition) const {
    SDL_SetRenderDrawColor(renderer, 0x87, 0xCE, 0xEB, 0xFF);
    SDL_RenderFillRect(renderer, NULL);

    for (Object object : m_Objects) {
        SDL_FRect dst = object.obj;
        dst.x -= cameraPosition.x;
        dst.y -= cameraPosition.y;

        SDL_SetRenderDrawColor(renderer, object.color.r, object.color.g, object.color.b, 0xFF);
        SDL_RenderFillRectF(renderer, &dst);
    }

    for (SDL_FRect coin : m_Coins) {
        SDL_FRect dst = SDL_FRect {.x = coin.x + 4, .y = coin.y + 4, .w = COIN_RENDER_SIZE, .h = COIN_RENDER_SIZE};
        dst.x -= cameraPosition.x;
        dst.y -= cameraPosition.y;

        SDL_SetRenderDrawColor(renderer, 0xFF, 0xD7, 0x00, 0xFF);
        SDL_RenderFillRectF(renderer, &dst);
    }

    for (Enemy *e : m_Enemies) {
        e->render(cameraPosition);
    }

    SDL_FRect flagDst = m_LevelCompleteFlag;
    flagDst.x -= cameraPosition.x;
    flagDst.y -= cameraPosition.y;

    SDL_RenderCopyF(renderer, m_LevelCompleteTexture, NULL, &flagDst);
}

std::vector<Enemy *> &Level::getEnemies() {
    return m_Enemies;
}

std::vector<Object> &Level::getObjects() {
    return m_Objects;
}

std::vector<SDL_FRect> &Level::getCoins() {
    return m_Coins;
}

SDL_FRect *Level::getFlag() {
    return &m_LevelCompleteFlag;
}
