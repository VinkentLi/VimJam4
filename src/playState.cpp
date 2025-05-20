#include "playState.h"
#include "selectState.h"
#include "enemy.h"
#include "font.h"
#include <SDL_image.h>
#include "constants.h"

PlayState PlayState::m_PlayState = PlayState();

void PlayState::init() {
    m_Level = new Level();
    m_CameraPosition = SDL_FPoint {.x = -3 * m_PLAYER_SIZE, .y = -5 * m_PLAYER_SIZE};
    m_PlayerPosition = SDL_FPoint {.x = 0.0f, .y = 0.0f};
    m_LatestCheckpointPosition = SDL_FPoint {m_PlayerPosition.x, m_PlayerPosition.y};
    m_PlayerHitbox = SDL_FRect {.x = 5.0f, .y = 2.0f, .w = 6.0f, .h = 14.0f};
    m_PlayerXVelocity = 0.0f;
    m_PlayerYVelocity = 0.0f;
    m_InvincibilityTimer = 0.0f;
    m_PlayerTexture = IMG_LoadTexture(renderer, "res/gfx/player.png");
    m_CheckpointTexture = IMG_LoadTexture(renderer, "res/gfx/checkpoint.png");
    m_CheckpointSound = Mix_LoadWAV("res/sfx/checkpoint.wav");
    m_CoinSound = Mix_LoadWAV("res/sfx/coin.wav");
    m_DeathSound = Mix_LoadWAV("res/sfx/death.wav");
    m_JumpSound = Mix_LoadWAV("res/sfx/jump.wav");
    m_LeftPressed = false;
    m_RightPressed = false;
    m_JumpPressed = false;
    m_SprintPressed = false;
    m_Grounded = false;
    m_Jumping = false;
    m_Paused = false;
    m_Dead = false;
}

void PlayState::destroy() {
    SDL_DestroyTexture(m_PlayerTexture);
    SDL_DestroyTexture(m_CheckpointTexture);
    Mix_FreeChunk(m_CheckpointSound);
    Mix_FreeChunk(m_CoinSound);
    Mix_FreeChunk(m_DeathSound);
    Mix_FreeChunk(m_JumpSound);
    m_CheckpointRects.clear();
    delete m_Level;
}

void PlayState::kill() {
    Mix_PlayChannel(-1, m_DeathSound, 0);
    m_Dead = true;
    m_PlayerPosition = m_LatestCheckpointPosition;
    m_PlayerHitbox.x = m_PlayerPosition.x + 5.0f;
    m_PlayerHitbox.y = m_PlayerPosition.y + 2.0f;
    addCoins(-getCoins());
    m_PlayerXVelocity = 0.0f;
    m_PlayerYVelocity = 0.0f;
    m_InvincibilityTimer = 60.0f;
}

void PlayState::handleEvents(SDL_Event *event) {
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.scancode) {
        case SDL_SCANCODE_LEFT:
        case SDL_SCANCODE_A:
            m_LeftPressed = true;
            break;
        case SDL_SCANCODE_RIGHT:
        case SDL_SCANCODE_D:
            m_RightPressed = true;
            break;
        case SDL_SCANCODE_UP:
        case SDL_SCANCODE_SPACE:
            m_Dead = false;
            m_JumpPressed = true;
            break;
        case SDL_SCANCODE_LSHIFT:
        case SDL_SCANCODE_RSHIFT:
            m_SprintPressed = true;
            break;
        default:
            break;
        }
    } else if (event->type == SDL_KEYUP) {
        switch (event->key.keysym.scancode) {
        case SDL_SCANCODE_LEFT:
        case SDL_SCANCODE_A:
            m_LeftPressed = false;
            break;
        case SDL_SCANCODE_RIGHT:
        case SDL_SCANCODE_D:
            m_RightPressed = false;
            break;
        case SDL_SCANCODE_UP:
        case SDL_SCANCODE_SPACE:
            m_JumpPressed = false;
            break;
        case SDL_SCANCODE_LSHIFT:
        case SDL_SCANCODE_RSHIFT:
            m_SprintPressed = false;
            break;
        case SDL_SCANCODE_ESCAPE:
            m_Paused = !m_Paused;
            break;
        case SDL_SCANCODE_C: {
            if (m_Checkpoints <= 0) {
                break;
            }

            Mix_PlayChannel(-1, m_CheckpointSound, 0);
            changeUsedCheckpoint();
            addCheckpoints(-1);
            m_LatestCheckpointPosition = SDL_FPoint {m_PlayerPosition.x, m_PlayerPosition.y};
            SDL_FRect checkpointsRect = {.x = m_PlayerPosition.x, .y = m_PlayerPosition.y, .w = 16, .h = 16};
            m_CheckpointRects.push_back(checkpointsRect);
            break;
        }
        default:
            break;
        }
    }
}

void PlayState::update(float dt) {
    if (m_Paused || m_Dead) {
        return;
    }
    m_InvincibilityTimer -= dt;
    m_Level->update(dt);
    float x_accel = m_X_ACCEL;
    if (!m_Grounded) {
        x_accel /= 3.0f;
    }
    // move left
    if (m_LeftPressed) {
        m_PlayerXVelocity -= x_accel * dt;
        if (m_SprintPressed && m_PlayerXVelocity < -m_MAX_SPRINT_VELO) {
            m_PlayerXVelocity = -m_MAX_SPRINT_VELO;
        } else if (!m_SprintPressed && m_PlayerXVelocity < -m_MAX_VELO) {
            m_PlayerXVelocity = -m_MAX_VELO;
        }
    }
    // move right
    if (m_RightPressed) {
        m_PlayerXVelocity += x_accel * dt; 
        if (m_SprintPressed && m_PlayerXVelocity > m_MAX_SPRINT_VELO) {
            m_PlayerXVelocity = m_MAX_SPRINT_VELO;
        } else if (!m_SprintPressed && m_PlayerXVelocity > m_MAX_VELO) {
            m_PlayerXVelocity = m_MAX_VELO;
        }
    }
    // slow down if not moving in either dir
    if (((!m_LeftPressed && !m_RightPressed) || (m_LeftPressed && m_RightPressed)) && m_PlayerXVelocity != 0.0f) {
        if (m_PlayerXVelocity > 0.0f) {
            m_PlayerXVelocity -= x_accel * dt;
            if (m_PlayerXVelocity < 0.0f) {
                m_PlayerXVelocity = 0.0f;
            }
        } else {
            m_PlayerXVelocity += x_accel * dt;
            if (m_PlayerXVelocity > 0.0f) {
                m_PlayerXVelocity = 0.0f;
            }
        }
    }
    if (m_JumpPressed) {
        if (m_Grounded) {
            Mix_PlayChannel(-1, m_JumpSound, 0);
            m_PlayerYVelocity = m_JUMP_STRENGTH;
            m_Jumping = true;
        } else if (m_Jumping) {
            m_PlayerYVelocity += m_GRAVITY * dt / 4.0f;
        } else {
            m_PlayerYVelocity += m_GRAVITY * dt;
        }
    } else {
        m_PlayerYVelocity += m_GRAVITY * dt;
    }
    m_Grounded = false;
    if (m_PlayerYVelocity > m_TERMINAL_VELO) {
        m_PlayerYVelocity = m_TERMINAL_VELO;
    }
    // check y collisions
    m_PlayerHitbox.y += m_PlayerYVelocity * dt;
    for (Object &o : m_Level->getObjects()) {
        SDL_FRect intersect; 
        if (SDL_IntersectFRect(&m_PlayerHitbox, &o.obj, &intersect)) {
            if (m_PlayerYVelocity >= 0.0f) {
                m_PlayerHitbox.y -= intersect.h;
                m_Grounded = true; // fun fact: without vsync, i think the game runs so fast that the dt variable multiplies the y accel to be basically zero, causing no collision to be detected and leading the game to believe that the player isnt grounded for sometime lol
                m_Jumping = false;
            } else {
                m_PlayerHitbox.y += intersect.h;
            }
            m_PlayerYVelocity = 0.0f;
        }
    }
    m_PlayerPosition.y = m_PlayerHitbox.y - 2.0f;
    // check x collisions
    m_PlayerHitbox.x += m_PlayerXVelocity * dt;
    for (Object &o : m_Level->getObjects()) {
        SDL_FRect intersect;
        if (SDL_IntersectFRect(&m_PlayerHitbox, &o.obj, &intersect)) {
            if (m_PlayerXVelocity >= 0.0f) {
                m_PlayerHitbox.x -= intersect.w;
            } else {
                m_PlayerHitbox.x += intersect.w;
            }
            m_PlayerXVelocity = 0.0f;
        }
    }
    m_PlayerPosition.x = m_PlayerHitbox.x - 5.0f;
    // check coin collisions
    std::vector<SDL_FRect> &coins = m_Level->getCoins();
    for (auto it = coins.end() - 1; it >= coins.begin(); it--) {
        if (SDL_HasIntersectionF(&m_PlayerHitbox, it.base())) {
            Mix_PlayChannel(-1, m_CoinSound, 0);
            PlayState::addCoins(1);
            coins.erase(it);
        }
    }
    // check flag collision
    if (SDL_HasIntersectionF(&m_PlayerHitbox, m_Level->getFlag())) {
        SelectState::get()->goBackToState();
        GameStateManager::popState();
    }

    if (m_PlayerPosition.x - m_CameraPosition.x > WIDTH / 2) {
        m_CameraPosition.x = m_PlayerPosition.x - WIDTH / 2;
    }
    if (m_PlayerPosition.x - m_CameraPosition.x < WIDTH / 3) {
        m_CameraPosition.x = m_PlayerPosition.x - WIDTH / 3;
    }
    bool inVoid = m_PlayerPosition.y > 300;
    if (inVoid) {
        kill();
    }
    if (m_InvincibilityTimer >= 0.0f) {
        return;
    }
    // check enemy collisions
    for (Enemy *e : m_Level->getEnemies()) {
        if (SDL_HasIntersectionF(&m_PlayerHitbox, e->getHitbox())) {
            kill();
        }
    }
}

void PlayState::render() const {
    if (m_Dead) {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderFillRect(renderer, NULL);

        std::string text1 = "YOU DIED!";
        std::string text2 = "and lost all your coins!";
        std::string text3 = "press SPACE to go to your";
        std::string text4 = "last checkpoint!";

        FontRender::renderText(text1, WIDTH / 2 - text1.size() * FONT_SIZE / 2, HEIGHT / 2 - FONT_SIZE * 2);
        FontRender::renderText(text2, WIDTH / 2 - text2.size() * FONT_SIZE / 2, HEIGHT / 2 - FONT_SIZE);
        FontRender::renderText(text3, WIDTH / 2 - text3.size() * FONT_SIZE / 2, HEIGHT / 2);
        FontRender::renderText(text4, WIDTH / 2 - text4.size() * FONT_SIZE / 2, HEIGHT / 2 + FONT_SIZE);

        return;
    }
    m_Level->render(m_CameraPosition);
    for (SDL_FRect checkpoint : m_CheckpointRects) {
        SDL_FRect dst = checkpoint;
        dst.x -= m_CameraPosition.x;
        dst.y -= m_CameraPosition.y;

        SDL_RenderCopyF(renderer, m_CheckpointTexture, NULL, &dst);
    }
    SDL_Rect dst = SDL_Rect {
        .x = int(m_PlayerPosition.x - m_CameraPosition.x),
        .y = int(m_PlayerPosition.y - m_CameraPosition.y), 
        .w = m_PLAYER_SIZE, 
        .h = m_PLAYER_SIZE
    };

    SDL_RenderCopy(renderer, m_PlayerTexture, NULL, &dst);

    int LEVEL_INDEX = SelectState::get()->getCurrentLevel();
    const std::string topText = "LVL COIN CHKPNT";

    std::string bottomText = " ";
    bottomText += char(LEVEL_INDEX / 10 + '0');
    bottomText += char(LEVEL_INDEX % 10 + '0');
    bottomText += "  ";
    bottomText += char(m_Coins / 100 + '0');
    bottomText += char((m_Coins % 100) / 10 + '0');
    bottomText += char(m_Coins % 10 + '0');
    bottomText += "    ";
    bottomText += char(m_Checkpoints / 100 + '0');
    bottomText += char((m_Checkpoints % 100) / 10 + '0');
    bottomText += char(m_Checkpoints % 10 + '0');

    FontRender::renderText(topText, WIDTH / 2 - topText.size() * FONT_SIZE / 2, 0);
    FontRender::renderText(bottomText, WIDTH / 2 - topText.size() * FONT_SIZE / 2, FONT_SIZE);

    if (!m_UsedCheckpoint) {
        std::string text = "PRESS \"C\" to place a checkpoint!";
        FontRender::renderText(text, 1, HEIGHT - FONT_SIZE - 1);
    }

    if (m_Paused) {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x80);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderFillRect(renderer, NULL);
        const std::string text = "PAUSED";
        FontRender::renderText(text, WIDTH / 2 - text.size() * FONT_SIZE / 2, HEIGHT / 2 - FONT_SIZE / 2);
    }
}

void PlayState::changeUsedCheckpoint() {
    m_UsedCheckpoint = true;
}

void PlayState::addCoins(int n) {
    m_Coins += n;
}

void PlayState::addCheckpoints(int n) {
    m_Checkpoints += n;
}

int PlayState::getCoins() {
    return m_Coins;
}

int PlayState::getCheckpoints() {
    return m_Checkpoints;
}

PlayState *PlayState::get() {
    return &m_PlayState;
}

SDL_Texture *PlayState::getCheckpointTexture() {
    return m_CheckpointTexture;
}
