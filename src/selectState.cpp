#include <algorithm>
#include <SDL_image.h>
#include "selectState.h"
#include "playState.h"
#include "constants.h"
#include "font.h"

SelectState SelectState::m_SelectState = SelectState();

void SelectState::init() {
    m_PlayerTexture = IMG_LoadTexture(renderer, "res/gfx/player.png");
    m_NodeSurface = IMG_Load("res/gfx/node.png");
    m_NodeSurface = SDL_ConvertSurfaceFormat(m_NodeSurface, SDL_PIXELFORMAT_ARGB8888, 0);
    m_NodeOnTexture = SDL_CreateTextureFromSurface(renderer, m_NodeSurface);

    //grayscale surface from https://gigi.nullneuron.net/gigilabs/converting-an-image-to-grayscale-using-sdl2/ 
    SDL_LockSurface(m_NodeSurface);
    uint32_t *nodePixels = (uint32_t *)m_NodeSurface->pixels;

    for (int i = 0; i < m_NodeSurface->w * m_NodeSurface->h; i++) {
        uint32_t pixel = nodePixels[i];
        if (!(pixel >> 24)) {
            continue;
        }
        uint8_t r = pixel >> 16 & 0xFF;
        uint8_t g = pixel >> 8 & 0xFF;
        uint8_t b = pixel & 0xFF;
        uint8_t v = 0.212671f * r + 0.715160f * g + 0.072169f * b;
        pixel = (0xFF << 24) | (v << 16) | (v << 8) | v;
        nodePixels[i] = pixel;
    }
    SDL_UnlockSurface(m_NodeSurface);
    m_NodeOffTexture = SDL_CreateTextureFromSurface(renderer, m_NodeSurface);
    SDL_Surface *nodeBondSurface = IMG_Load("res/gfx/node_bond.png");
    nodeBondSurface = SDL_ConvertSurfaceFormat(nodeBondSurface, SDL_PIXELFORMAT_ARGB8888, 0);
    m_NodeBondOnTexture = SDL_CreateTextureFromSurface(renderer, nodeBondSurface);

    SDL_LockSurface(nodeBondSurface);
    uint32_t *bondPixels = (uint32_t *)nodeBondSurface->pixels;

    for (int i = 0; i < nodeBondSurface->w * nodeBondSurface->h; i++) {
        uint32_t pixel = bondPixels[i];
        if (!(pixel >> 24)) {
            continue;
        }
        uint8_t r = pixel >> 16 & 0xFF;
        uint8_t g = pixel >> 8 & 0xFF;
        uint8_t b = pixel & 0xFF;
        uint8_t v = 0.212671f * r + 0.715160f * g + 0.072169f * b;
        pixel = (0xFF << 24) | (v << 16) | (v << 8) | v;
        bondPixels[i] = pixel;
    }
    SDL_UnlockSurface(nodeBondSurface);
    m_NodeBondOffTexture = SDL_CreateTextureFromSurface(renderer, nodeBondSurface);
    SDL_FreeSurface(nodeBondSurface);

    m_BuySound = Mix_LoadWAV("res/sfx/buy.wav");
    m_EnterSound = Mix_LoadWAV("res/sfx/enter.wav");
    m_UnlockBondSound = Mix_LoadWAV("res/sfx/unlock_bond.wav");
    m_LevelCompleteSound = Mix_LoadWAV("res/sfx/level_complete.wav");

    // SDL_Log("%d", SDL_MUSTLOCK(m_node_bond_on_surf));
    // SDL_LockSurface(m_node_bond_on_surf);

    m_NodeData = "06 00 1 1 "
                  "04 02 1 1 "
                  "08 02 1 1 "
                  "02 04 1 1 "
                  "06 04 1 1 "
                  "10 04 1 1 "
                  "00 06 0 1 "
                  "04 06 1 1 "
                  "08 06 1 1 "
                  "12 06 1 0 "
                  "02 08 0 1 "
                  "06 08 1 1 "
                  "10 08 1 0 "
                  "04 10 0 1 "
                  "08 10 1 0 "
                  "06 12 0 0 ";

    parseNodeData();

    // nodes are already sorted in m_NodeData but im still gonna sort it because why not
    std::sort(m_Nodes.begin(), m_Nodes.end(), [](const Node &a, const Node &b) -> bool {
        if (a.pos.y == b.pos.y) {
            return a.pos.x < b.pos.x;
        }
        return a.pos.y < b.pos.y; 
    });
    m_CurrentNode = m_Nodes.begin();
    m_LastPlayedNode = -1;

    m_CameraPosition = SDL_FPoint{.x = 13.0f / 2.0f - m_CAM_W / 2, .y = -m_CAM_H / 2 + 0.5};
    m_PlayerPosition = SDL_FPoint{.x = 6.0f, .y = 0.0f}; 
    m_MoveTimer = 0.0f;
    m_MoveTopRight = false;
    m_MoveTopLeft = false;
    m_MoveBottomLeft = false;
    m_MoveBottomRight = false;
    m_Moving = false;
    m_ShopMode = false;
    m_SelectBondMode = false;
    m_NoDoublePlayMode = false;
}

void SelectState::destroy()
{
    SDL_DestroyTexture(m_PlayerTexture);
    SDL_DestroyTexture(m_NodeOnTexture);
    SDL_DestroyTexture(m_NodeOffTexture);
    SDL_DestroyTexture(m_NodeBondOnTexture);
    SDL_DestroyTexture(m_NodeBondOffTexture);
    // SDL_UnlockSurface(m_node_bond_on_surf);
    SDL_FreeSurface(m_NodeSurface);
    Mix_FreeChunk(m_BuySound);
    Mix_FreeChunk(m_EnterSound);
    Mix_FreeChunk(m_UnlockBondSound);
    Mix_FreeChunk(m_LevelCompleteSound);
}

void SelectState::parseNodeData() {
    for (auto it = m_NodeData.begin(); it < m_NodeData.end(); it += 2) {
        Node n;
        n.active = false;
        n.pos.x = (*it - '0') * 10 + (*(++it)) - '0';
        // SDL_Log("x: %d", n.pos.x);
        it += 2;
        n.pos.y = (*it - '0') * 10 + (*(++it)) - '0';
        // SDL_Log("y: %d, ", n.pos.y);
        it += 2;
        // SDL_Log("bl: %d", *it - '0');
        if (*it - '0' == 1) {
            n.bl = false;
        } else {
            n.bl = std::nullopt;
        }
        // SDL_Log("optional1: %d", *it - '0');
        it += 2;
        // SDL_Log("br: %d\n", *it - '0');
        if (*it - '0' == 1) {
            n.br = false;
        } else {
            n.br = std::nullopt;
        }
        // SDL_Log("optional2: %d", *it - '0');
        m_Nodes.push_back(n);
    }
}

void SelectState::handleEvents(SDL_Event *event) {
    if (event->type == SDL_KEYDOWN) {
        // if (event->key.keysym.scancode == SDL_SCANCODE_ESCAPE) {    
        //     GameStateManager::popState();
        // }
        /*else*/ if (event->key.keysym.scancode == SDL_SCANCODE_SPACE) {
            if (m_LastPlayedNode != getCurrentLevel()) {
                Mix_PlayChannel(-1, m_EnterSound, 0);
                GameStateManager::addState(PlayState::get());
            } else if (!m_NoDoublePlayMode) {
                m_NoDoublePlayMode = true;
            } else {
                m_NoDoublePlayMode = false;
            }
        }
    } else if (event->type == SDL_KEYUP) {
        if (event->key.keysym.scancode == SDL_SCANCODE_S) {
            m_ShopMode = !m_ShopMode;
        } else if (event->key.keysym.scancode == SDL_SCANCODE_B) {
            if (m_ShopMode) {
                int coins = PlayState::getCoins();

                if (coins >= 3) {
                    Mix_PlayChannel(-1, m_BuySound, 0);
                    PlayState::addCheckpoints(1);
                    PlayState::addCoins(-3);
                }
            }
        }
    }

    if (!m_Moving && event->type == SDL_MOUSEBUTTONDOWN) {
        int x, y;

        if (SDL_GetMouseState(&x, &y) == 1) {
            // scale x and y
            constexpr float wScale = (float)WIDTH / (float)SCREEN_WIDTH;
            constexpr float hScale = (float)HEIGHT / (float)SCREEN_HEIGHT;

            x *= wScale;
            y *= hScale;

            if (x < WIDTH / 2 && y < HEIGHT / 2) {
                m_MoveTopLeft = true;
                return;
            }

            if (x >= WIDTH / 2 && y < HEIGHT / 2) {
                m_MoveTopRight = true;
                return;
            }

            if (x < WIDTH / 2 && y >= HEIGHT / 2) {
                m_MoveBottomLeft = true;
                return;
            }

            if (x >= WIDTH / 2 && y >= HEIGHT / 2) {
                m_MoveBottomRight = true;
                return;
            }
        }
    }
}

void SelectState::update(float dt) {
    if (m_MoveTopLeft) {
        m_MoveTopLeft = false;
        std::optional<int> topLeftNodePosition = nodeSeearch(m_CurrentNode->pos.x - 2, m_CurrentNode->pos.y - 2, 0, m_CurrentNode - m_Nodes.begin());
        if (topLeftNodePosition.has_value()) {
            if (m_Nodes.at(topLeftNodePosition.value()).br.has_value()) {
                if (m_Nodes.at(topLeftNodePosition.value()).br.value()) {   
                    m_CurrentNode = topLeftNodePosition.value() + m_Nodes.begin();
                    m_Moving = true;
                    m_MoveTimer = 100.0f;
                    m_MoveDirection = TOP_LEFT;
                } else if (m_SelectBondMode) {
                    m_Nodes.at(topLeftNodePosition.value()).br = true;
                    m_SelectBondMode = false;
                    Mix_PlayChannel(-1, m_UnlockBondSound, 0);
                }
            }
        }
    } else if (m_MoveTopRight) {
        m_MoveTopRight = false;
        std::optional<int> topRightNodePosition = nodeSeearch(m_CurrentNode->pos.x + 2, m_CurrentNode->pos.y - 2, 0, m_CurrentNode - m_Nodes.begin());
        if (topRightNodePosition.has_value()) {
            if (m_Nodes.at(topRightNodePosition.value()).bl.has_value()) {
                if (m_Nodes.at(topRightNodePosition.value()).bl.value()) {   
                    m_CurrentNode = topRightNodePosition.value() + m_Nodes.begin();
                    m_Moving = true;
                    m_MoveTimer = 100.0f;
                    m_MoveDirection = TOP_RIGHT;
                } else if (m_SelectBondMode) {
                    m_Nodes.at(topRightNodePosition.value()).bl = true;
                    m_SelectBondMode = false;
                    Mix_PlayChannel(-1, m_UnlockBondSound, 0);
                }
            }
        }
    } else if (m_MoveBottomLeft) {
        m_MoveBottomLeft = false;
        if (m_CurrentNode->bl.has_value()) {
            if (m_CurrentNode->bl.value()) {   
                std::optional<int> bl_node_pos = nodeSeearch(m_CurrentNode->pos.x - 2, m_CurrentNode->pos.y + 2, m_CurrentNode - m_Nodes.begin());
                if (bl_node_pos.has_value()) {
                    m_CurrentNode = bl_node_pos.value() + m_Nodes.begin();
                    m_Moving = true;
                    m_MoveTimer = 100.0f;
                    m_MoveDirection = BOTTOM_LEFT;
                }
            } else if (m_SelectBondMode) {
                m_CurrentNode->bl = true;
                m_SelectBondMode = false;
                Mix_PlayChannel(-1, m_UnlockBondSound, 0);
            }
        }
    } else if (m_MoveBottomRight) {
        m_MoveBottomRight = false;
        if (m_CurrentNode->br.has_value()) {
            if (m_CurrentNode->br.value()) {   
                std::optional<int> br_node_pos = nodeSeearch(m_CurrentNode->pos.x + 2, m_CurrentNode->pos.y + 2, m_CurrentNode - m_Nodes.begin());
                if (br_node_pos.has_value()) {
                    m_CurrentNode = br_node_pos.value() + m_Nodes.begin();
                    m_Moving = true;
                    m_MoveTimer = 100.0f;
                    m_MoveDirection = BOTTOM_RIGHT;
                }
            } else if (m_SelectBondMode) {
                m_CurrentNode->br = true;
                m_SelectBondMode = false;
                Mix_PlayChannel(-1, m_UnlockBondSound, 0);
            }
        }
    }

    if (m_Moving) {
        m_MoveTimer -= dt;
        if (m_MoveTimer <= 0.0f) {
            m_Moving = false;
            switch (m_MoveDirection) {
            case TOP_LEFT:
                m_CameraPosition.x -= 2;
                m_CameraPosition.y -= 2;
                break;
            case TOP_RIGHT:
                m_CameraPosition.x += 2;
                m_CameraPosition.y -= 2;
                break;
            case BOTTOM_LEFT:
                m_CameraPosition.x -= 2;
                m_CameraPosition.y += 2;
                break;
            case BOTTOM_RIGHT:
                m_CameraPosition.x += 2;
                m_CameraPosition.y += 2;
                break;
            }
        }
    }

    if (m_Moving) {
        switch (m_MoveDirection) {
        case TOP_LEFT:
            m_PlayerPosition.x -= m_PLAYER_SPEED * dt;
            m_PlayerPosition.y -= m_PLAYER_SPEED * dt;
            break;
        case TOP_RIGHT:
            m_PlayerPosition.x += m_PLAYER_SPEED * dt;
            m_PlayerPosition.y -= m_PLAYER_SPEED * dt;
            break;
        case BOTTOM_LEFT:
            m_PlayerPosition.x -= m_PLAYER_SPEED * dt;
            m_PlayerPosition.y += m_PLAYER_SPEED * dt;
            break;
        case BOTTOM_RIGHT:
            m_PlayerPosition.x += m_PLAYER_SPEED * dt;
            m_PlayerPosition.y += m_PLAYER_SPEED * dt;
            break;
        }
    }
}

void SelectState::render() const
{
    // for (int y = 0; y < HEIGHT; y++) {
    //     for (int x = 0; x < WIDTH; x++) {
    //         int i = y + x;
    //         int j = i * 500;

    //         int g = std::clamp((j & 0xFF) + 120, 0, 255);

    //         SDL_SetRenderDrawColor(renderer, 0, g, 0, 255);
    //         SDL_RenderDrawPoint(renderer, x, y);
    //     }
    // }

    if (m_ShopMode) {
        SDL_SetRenderDrawColor(renderer, 0x55, 0x34, 0x2B, 0xFF);
        SDL_RenderFillRect(renderer, NULL);

        std::string text1 = "PRESS \"B\" to buy a checkpoint";
        std::string text2 = "for 3 coins";
        std::string text3 = "PRESS \"S\" to leave shop!";

        FontRender::renderText(text1, WIDTH / 2 - text1.size() * FONT_SIZE / 2, 0);
        FontRender::renderText(text2, WIDTH / 2 - text2.size() * FONT_SIZE / 2, FONT_SIZE);
        FontRender::renderText(text3, WIDTH / 2 - text3.size() * FONT_SIZE / 2, HEIGHT - FONT_SIZE - 1);

        SDL_Texture *checkpointTexture = IMG_LoadTexture(renderer, "res/gfx/checkpoint.png"); // really bad code
        SDL_Rect dst = SDL_Rect {.x = WIDTH / 2 - 16, .y = HEIGHT / 2 - 16, .w = 32, .h = 32};

        SDL_RenderCopy(renderer, checkpointTexture, NULL, &dst);
        SDL_DestroyTexture(checkpointTexture); // no mem leak

        return;
    }

    if (m_NoDoublePlayMode) {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderFillRect(renderer, NULL);

        std::string text1 = "you can play the same level";
        std::string text2 = "multiple times. however";
        std::string text3 = "you may not play the same level";
        std::string text4 = "twice in a row";

        FontRender::renderText(text1, WIDTH / 2 - text1.size() * FONT_SIZE / 2, HEIGHT / 2 - FONT_SIZE * 2);
        FontRender::renderText(text2, WIDTH / 2 - text2.size() * FONT_SIZE / 2, HEIGHT / 2 - FONT_SIZE);
        FontRender::renderText(text3, WIDTH / 2 - text3.size() * FONT_SIZE / 2, HEIGHT / 2);
        FontRender::renderText(text4, WIDTH / 2 - text4.size() * FONT_SIZE / 2, HEIGHT / 2 + FONT_SIZE);

        return;
    }

    SDL_SetRenderDrawColor(renderer, 0x00, 0xB7, 0x0A, 0xFF);
    SDL_RenderFillRect(renderer, NULL);

    // render paths/nodes
    for (auto it = m_Nodes.begin(); it < m_Nodes.end(); it++) {
        float nodeX = (float)it->pos.x - m_CameraPosition.x;
        float nodeY = (float)it->pos.y - m_CameraPosition.y;

        // render bonds first
        if (it->bl.has_value()) {
            bool bondActive = it->bl.value();
            SDL_Texture *bondTexture = bondActive ? m_NodeBondOnTexture : m_NodeBondOffTexture;

            SDL_Rect bondDst = SDL_Rect {
                .x = int(nodeX * (float)m_NODE_SIZE) - 7 * m_NODE_SIZE / 2,
                .y = int(nodeY * (float)m_NODE_SIZE) + m_NODE_SIZE / 2,
                .w = m_NODE_SIZE * 4,
                .h = m_NODE_SIZE * 4
            };

            SDL_RenderCopyEx(renderer, bondTexture, NULL, &bondDst, 0.0, NULL, SDL_FLIP_HORIZONTAL);
        }

        if (it->br.has_value()) {
            bool bondActive = it->br.value();
            SDL_Texture *bondTexture = bondActive ? m_NodeBondOnTexture : m_NodeBondOffTexture;

            SDL_Rect bondDst = SDL_Rect {
                .x = int(nodeX * (float)m_NODE_SIZE) + m_NODE_SIZE / 2,
                .y = int(nodeY * (float)m_NODE_SIZE) + m_NODE_SIZE / 2,
                .w = m_NODE_SIZE * 4,
                .h = m_NODE_SIZE * 4
            };

            SDL_RenderCopyEx(renderer, bondTexture, NULL, &bondDst, 0.0, NULL, SDL_FLIP_NONE);
        }

        // render nodes
        SDL_Rect nodeDst = SDL_Rect {
            .x = int(nodeX * (float)m_NODE_SIZE),
            .y = int(nodeY * (float)m_NODE_SIZE),
            .w = m_NODE_SIZE,
            .h = m_NODE_SIZE
        };

        SDL_Texture *nodeTexture = it->active ? m_NodeOnTexture : m_NodeOffTexture;

        SDL_RenderCopy(renderer, nodeTexture, NULL, &nodeDst);
    }

    // render player
    constexpr int PLAYER_OFFSET = 4;

    float currentPlayerX = m_PlayerPosition.x - m_CameraPosition.x;
    float currentPlayerY = m_PlayerPosition.y - m_CameraPosition.y;

    SDL_Rect playerDst = SDL_Rect {
        .x = int(currentPlayerX * (float)m_NODE_SIZE) + PLAYER_OFFSET,
        .y = int(currentPlayerY * (float)m_NODE_SIZE) - PLAYER_OFFSET,
        .w = m_PLAYER_SIZE,
        .h = m_PLAYER_SIZE
    };

    SDL_RenderCopy(renderer, m_PlayerTexture, NULL, &playerDst);

    int LEVEL_INDEX = m_CurrentNode - m_Nodes.begin() + 1;
    const std::string topText = "LVL COIN CHKPNT";

    int coins = PlayState::getCoins();
    int checkpoints = PlayState::getCheckpoints();

    std::string bottomText = " ";
    bottomText += char(LEVEL_INDEX / 10 + '0');
    bottomText += char(LEVEL_INDEX % 10 + '0');
    bottomText += "  ";
    bottomText += char(coins / 100 + '0');
    bottomText += char((coins % 100) / 10 + '0');
    bottomText += char(coins % 10 + '0');
    bottomText += "    ";
    bottomText += char(checkpoints / 100 + '0');
    bottomText += char((checkpoints % 100) / 10 + '0');
    bottomText += char(checkpoints % 10 + '0');

    FontRender::renderText(topText, WIDTH / 2 - topText.size() * FONT_SIZE / 2, 0);
    FontRender::renderText(bottomText, WIDTH / 2 - topText.size() * FONT_SIZE / 2, FONT_SIZE);

    if (m_SelectBondMode) {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x50);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderFillRect(renderer, NULL);

        std::string text1 = "PLEASE SELECT AN";
        std::string text2 = "EMPTY PATH TO ACTIVATE";
        std::string text3 = "IT OR MOVE TO AN ACTIVE";
        std::string text4 = "PATH UNTIL YOU SELECT ONE";

        FontRender::renderText(text1, WIDTH / 2 - text1.size() * FONT_SIZE / 2, HEIGHT / 2 - FONT_SIZE * 2);
        FontRender::renderText(text2, WIDTH / 2 - text2.size() * FONT_SIZE / 2, HEIGHT / 2 - FONT_SIZE);
        FontRender::renderText(text3, WIDTH / 2 - text3.size() * FONT_SIZE / 2, HEIGHT / 2);
        FontRender::renderText(text4, WIDTH / 2 - text4.size() * FONT_SIZE / 2, HEIGHT / 2 + FONT_SIZE);
    }
    FontRender::renderText("PRESS \"S\" to shop!", 1, HEIGHT - FONT_SIZE - 1);
}

SelectState *SelectState::get() {
    return &m_SelectState;
}

std::optional<int> SelectState::nodeSeearch(int x, int y, int lo, int hi) {
    if (lo < 0 || lo >= m_NODE_COUNT || hi < 0 || hi >= m_NODE_COUNT) {
        return {};
    }

    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (m_Nodes.at(mid).pos.y == y) {
            if (m_Nodes.at(mid).pos.x == x) {
                return mid;
            }
            if (m_Nodes.at(mid).pos.x > x) {
                hi = mid - 1;
            } else {
                lo = mid + 1;
            }
            continue;
        }

        if (m_Nodes.at(mid).pos.y > y) {
            hi = mid - 1;
        } else {
            lo = mid + 1;
        }
    }
    return {};
}

int SelectState::getCurrentLevel() {
    return m_CurrentNode - m_Nodes.begin() + 1;
}

std::vector<Node>::iterator SelectState::getCurrentNode() {
    return m_CurrentNode;
}

void SelectState::setSelectBond(bool a) {
    m_SelectBondMode = a;
}

void SelectState::setLastPlayedNode(int i) {
    m_LastPlayedNode = i;
}

Mix_Chunk *SelectState::getLevelCompleteSound() {
    return m_LevelCompleteSound;
}

void SelectState::goBackToState() {
    SelectState *ss = SelectState::get();
    Mix_PlayChannel(-1, ss->getLevelCompleteSound(), 0);
    ss->setSelectBond(true);
    ss->getCurrentNode()->active = true;
    ss->setLastPlayedNode(ss->getCurrentLevel());
}
