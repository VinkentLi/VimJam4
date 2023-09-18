#include <algorithm>
#include <SDL_image.h>
#include "select_state.h"
#include "play_state.h"
#include "constants.h"
#include "font.h"

SelectState SelectState::m_select_state = SelectState();

void SelectState::init()
{
    m_player_tex = IMG_LoadTexture(renderer, "res/gfx/player.png");
    m_node_surface = IMG_Load("res/gfx/node.png");
    m_node_surface = SDL_ConvertSurfaceFormat(m_node_surface, SDL_PIXELFORMAT_ARGB8888, 0);
    m_node_on_tex = SDL_CreateTextureFromSurface(renderer, m_node_surface);

    //grayscale surface from https://gigi.nullneuron.net/gigilabs/converting-an-image-to-grayscale-using-sdl2/ 
    SDL_LockSurface(m_node_surface);
    uint32_t *node_pixels = (uint32_t *)m_node_surface->pixels;

    for (int i = 0; i < m_node_surface->w * m_node_surface->h; i++)
    {
        uint32_t pixel = node_pixels[i];

        if (!(pixel >> 24))
        {
            continue;
        }

        uint8_t r = pixel >> 16 & 0xFF;
        uint8_t g = pixel >> 8 & 0xFF;
        uint8_t b = pixel & 0xFF;
        uint8_t v = 0.212671f * r + 0.715160f * g + 0.072169f * b;
        pixel = (0xFF << 24) | (v << 16) | (v << 8) | v;
        node_pixels[i] = pixel;
    }
    SDL_UnlockSurface(m_node_surface);

    m_node_off_tex = SDL_CreateTextureFromSurface(renderer, m_node_surface);

    SDL_Surface *node_bond_surf = IMG_Load("res/gfx/node_bond.png");
    node_bond_surf = SDL_ConvertSurfaceFormat(node_bond_surf, SDL_PIXELFORMAT_ARGB8888, 0);

    m_node_bond_on_tex = SDL_CreateTextureFromSurface(renderer, node_bond_surf);

    SDL_LockSurface(node_bond_surf);
    uint32_t *bond_pixels = (uint32_t *)node_bond_surf->pixels;

    for (int i = 0; i < node_bond_surf->w * node_bond_surf->h; i++)
    {
        uint32_t pixel = bond_pixels[i];

        if (!(pixel >> 24))
        {
            continue;
        }

        uint8_t r = pixel >> 16 & 0xFF;
        uint8_t g = pixel >> 8 & 0xFF;
        uint8_t b = pixel & 0xFF;
        uint8_t v = 0.212671f * r + 0.715160f * g + 0.072169f * b;
        pixel = (0xFF << 24) | (v << 16) | (v << 8) | v;
        bond_pixels[i] = pixel;
    }
    SDL_UnlockSurface(node_bond_surf);
    m_node_bond_off_tex = SDL_CreateTextureFromSurface(renderer, node_bond_surf);
    SDL_FreeSurface(node_bond_surf);

    // SDL_Log("%d", SDL_MUSTLOCK(m_node_bond_on_surf));
    // SDL_LockSurface(m_node_bond_on_surf);

    m_node_data = "06 00 1 1 "
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

    parse_node_data();

    // nodes are already sorted in m_node_data but im still gonna sort it because why not
    std::sort(m_nodes.begin(), m_nodes.end(), [](const Node &a, const Node &b) -> bool
    {
        if (a.pos.y == b.pos.y)
        {
            return a.pos.x < b.pos.x;
        }
        return a.pos.y < b.pos.y; 
    });

    m_cur_node = m_nodes.begin();
    m_last_played_node = -1;

    m_camera_pos = SDL_FPoint{.x = 13.0f / 2.0f - m_CAM_W / 2, .y = -m_CAM_H / 2 + 0.5};
    m_player_pos = SDL_FPoint{.x = 6.0f, .y = 0.0f}; 
    m_move_timer = 0.0f;
    m_move_tr = false;
    m_move_tl = false;
    m_move_bl = false;
    m_move_br = false;
    m_moving = false;
    m_shop_mode = false;
    m_select_bond_mode = false;
    m_no_double_play_mode = false;
}

void SelectState::destroy()
{
    SDL_DestroyTexture(m_player_tex);
    SDL_DestroyTexture(m_node_on_tex);
    SDL_DestroyTexture(m_node_off_tex);
    SDL_DestroyTexture(m_node_bond_on_tex);
    SDL_DestroyTexture(m_node_bond_off_tex);
    // SDL_UnlockSurface(m_node_bond_on_surf);
    SDL_FreeSurface(m_node_surface);
}

void SelectState::parse_node_data()
{
    for (auto it = m_node_data.begin(); it < m_node_data.end(); it += 2)
    {
        Node n;
        n.active = false;
        n.pos.x = (*it - '0') * 10 + (*(++it)) - '0';
        // SDL_Log("x: %d", n.pos.x);

        it += 2;

        n.pos.y = (*it - '0') * 10 + (*(++it)) - '0';
        // SDL_Log("y: %d, ", n.pos.y);

        it += 2;

        // SDL_Log("bl: %d", *it - '0');

        if (*it - '0' == 1)
        {
            n.bl = false;
        }
        else
        {
            n.bl = std::nullopt;
        }

        // SDL_Log("optional1: %d", *it - '0');

        it += 2;

        // SDL_Log("br: %d\n", *it - '0');

        if (*it - '0' == 1)
        {
            n.br = false;
        }
        else
        {
            n.br = std::nullopt;
        }
        // SDL_Log("optional2: %d", *it - '0');

        m_nodes.push_back(n);
    }
}

void SelectState::handle_events(SDL_Event *event)
{
    if (event->type == SDL_KEYDOWN)
    {
        // if (event->key.keysym.scancode == SDL_SCANCODE_ESCAPE)
        // {    
        //     GameStateManager::pop_state();
        // }
        /*else*/ if (event->key.keysym.scancode == SDL_SCANCODE_SPACE)
        {
            if (m_last_played_node != get_cur_lvl())
            {
                GameStateManager::add_state(PlayState::get());
            }
            else if (!m_no_double_play_mode)
            {
                m_no_double_play_mode = true;
            }
            else
            {
                m_no_double_play_mode = false;
            }
        }
    }
    else if (event->type == SDL_KEYUP)
    {
        if (event->key.keysym.scancode == SDL_SCANCODE_S)
            m_shop_mode = !m_shop_mode;
        else if (event->key.keysym.scancode == SDL_SCANCODE_B)
        {
            if (m_shop_mode)
            {
                int coins = PlayState::get_coins();

                if (coins >= 3)
                {
                    PlayState::add_checkpoints(1);
                    PlayState::add_coins(-3);
                }
            }
        }
    }

    if (!m_moving && event->type == SDL_MOUSEBUTTONDOWN)
    {
        int x, y;

        if (SDL_GetMouseState(&x, &y) == 1)
        {
            // scale x and y
            constexpr float w_scale = (float)WIDTH / (float)SCREEN_WIDTH;
            constexpr float h_scale = (float)HEIGHT / (float)SCREEN_HEIGHT;

            x *= w_scale;
            y *= h_scale;

            if (x < WIDTH / 2 && y < HEIGHT / 2)
            {
                m_move_tl = true;
                return;
            }

            if (x >= WIDTH / 2 && y < HEIGHT / 2)
            {
                m_move_tr = true;
                return;
            }

            if (x < WIDTH / 2 && y >= HEIGHT / 2)
            {
                m_move_bl = true;
                return;
            }

            if (x >= WIDTH / 2 && y >= HEIGHT / 2)
            {
                m_move_br = true;
                return;
            }
        }
    }
}

void SelectState::update(float dt)
{
    if (m_move_tl)
    {
        m_move_tl = false;
        std::optional<int> tl_node_pos = node_search(m_cur_node->pos.x - 2, m_cur_node->pos.y - 2, 0, m_cur_node - m_nodes.begin());

        if (tl_node_pos.has_value())
        {
            if (m_nodes.at(tl_node_pos.value()).br.has_value())
            {
                if (m_nodes.at(tl_node_pos.value()).br.value())
                {   
                    m_cur_node = tl_node_pos.value() + m_nodes.begin();
                    m_moving = true;
                    m_move_timer = 100.0f;
                    m_move_dir = TOP_LEFT;
                }
                else if (m_select_bond_mode)
                {
                    m_nodes.at(tl_node_pos.value()).br = true;
                    m_select_bond_mode = false;
                }
            }
        }
    }
    else if (m_move_tr)
    {
        m_move_tr = false;
        std::optional<int> tr_node_pos = node_search(m_cur_node->pos.x + 2, m_cur_node->pos.y - 2, 0, m_cur_node - m_nodes.begin());

        if (tr_node_pos.has_value())
        {
            if (m_nodes.at(tr_node_pos.value()).bl.has_value())
            {
                if (m_nodes.at(tr_node_pos.value()).bl.value())
                {   
                    m_cur_node = tr_node_pos.value() + m_nodes.begin();
                    m_moving = true;
                    m_move_timer = 100.0f;
                    m_move_dir = TOP_RIGHT;
                }
                else if (m_select_bond_mode)
                {
                    m_nodes.at(tr_node_pos.value()).bl = true;
                    m_select_bond_mode = false;
                }
            }
        }
    }
    else if (m_move_bl)
    {
        m_move_bl = false;

        if (m_cur_node->bl.has_value())
        {
            if (m_cur_node->bl.value())
            {   
                std::optional<int> bl_node_pos = node_search(m_cur_node->pos.x - 2, m_cur_node->pos.y + 2, m_cur_node - m_nodes.begin());

                if (bl_node_pos.has_value())
                {
                    m_cur_node = bl_node_pos.value() + m_nodes.begin();
                    m_moving = true;
                    m_move_timer = 100.0f;
                    m_move_dir = BOTTOM_LEFT;
                }
            }
            else if (m_select_bond_mode)
            {
                m_cur_node->bl = true;
                m_select_bond_mode = false;
            }
        }
    }
    else if (m_move_br)
    {
        m_move_br = false;

        if (m_cur_node->br.has_value())
        {
            if (m_cur_node->br.value())
            {   
                std::optional<int> br_node_pos = node_search(m_cur_node->pos.x + 2, m_cur_node->pos.y + 2, m_cur_node - m_nodes.begin());

                if (br_node_pos.has_value())
                {
                    m_cur_node = br_node_pos.value() + m_nodes.begin();
                    m_moving = true;
                    m_move_timer = 100.0f;
                    m_move_dir = BOTTOM_RIGHT;
                }
            }
            else if (m_select_bond_mode)
            {
                m_cur_node->br = true;
                m_select_bond_mode = false;
            }
        }
    }

    if (m_moving)
    {
        m_move_timer -= dt;

        if (m_move_timer <= 0.0f)
        {
            m_moving = false;

            switch (m_move_dir)
            {
            case TOP_LEFT:
                m_camera_pos.x -= 2;
                m_camera_pos.y -= 2;
                break;
            case TOP_RIGHT:
                m_camera_pos.x += 2;
                m_camera_pos.y -= 2;
                break;
            case BOTTOM_LEFT:
                m_camera_pos.x -= 2;
                m_camera_pos.y += 2;
                break;
            case BOTTOM_RIGHT:
                m_camera_pos.x += 2;
                m_camera_pos.y += 2;
                break;
            }
        }
    }

    if (m_moving)
    {
        switch (m_move_dir)
        {
        case TOP_LEFT:
            m_player_pos.x -= m_PLAYER_SPEED * dt;
            m_player_pos.y -= m_PLAYER_SPEED * dt;
            break;
        case TOP_RIGHT:
            m_player_pos.x += m_PLAYER_SPEED * dt;
            m_player_pos.y -= m_PLAYER_SPEED * dt;
            break;
        case BOTTOM_LEFT:
            m_player_pos.x -= m_PLAYER_SPEED * dt;
            m_player_pos.y += m_PLAYER_SPEED * dt;
            break;
        case BOTTOM_RIGHT:
            m_player_pos.x += m_PLAYER_SPEED * dt;
            m_player_pos.y += m_PLAYER_SPEED * dt;
            break;
        }
    }
}

void SelectState::render() const
{
    // for (int y = 0; y < HEIGHT; y++)
    // {
    //     for (int x = 0; x < WIDTH; x++)
    //     {
    //         int i = y + x;
    //         int j = i * 500;

    //         int g = std::clamp((j & 0xFF) + 120, 0, 255);

    //         SDL_SetRenderDrawColor(renderer, 0, g, 0, 255);
    //         SDL_RenderDrawPoint(renderer, x, y);
    //     }
    // }

    if (m_shop_mode)
    {
        SDL_SetRenderDrawColor(renderer, 0x55, 0x34, 0x2B, 0xFF);
        SDL_RenderFillRect(renderer, NULL);

        std::string text1 = "PRESS \"B\" to buy a checkpoint";
        std::string text2 = "for 3 coins";
        std::string text3 = "PRESS \"S\" to leave shop!";

        FontRender::render_text(text1, WIDTH / 2 - text1.size() * FONT_SIZE / 2, 0);
        FontRender::render_text(text2, WIDTH / 2 - text2.size() * FONT_SIZE / 2, FONT_SIZE);
        FontRender::render_text(text3, WIDTH / 2 - text3.size() * FONT_SIZE / 2, HEIGHT - FONT_SIZE - 1);

        SDL_Texture *checkpoint_tex = IMG_LoadTexture(renderer, "res/gfx/checkpoint.png"); // really bad code
        SDL_Rect dst = SDL_Rect {.x = WIDTH / 2 - 16, .y = HEIGHT / 2 - 16, .w = 32, .h = 32};

        SDL_RenderCopy(renderer, checkpoint_tex, NULL, &dst);
        SDL_DestroyTexture(checkpoint_tex); // no mem leak

        return;
    }

    if (m_no_double_play_mode)
    {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderFillRect(renderer, NULL);

        std::string text1 = "you can play the same level";
        std::string text2 = "multiple times. however";
        std::string text3 = "you may not play the same level";
        std::string text4 = "twice in a row";

        FontRender::render_text(text1, WIDTH / 2 - text1.size() * FONT_SIZE / 2, HEIGHT / 2 - FONT_SIZE * 2);
        FontRender::render_text(text2, WIDTH / 2 - text2.size() * FONT_SIZE / 2, HEIGHT / 2 - FONT_SIZE);
        FontRender::render_text(text3, WIDTH / 2 - text3.size() * FONT_SIZE / 2, HEIGHT / 2);
        FontRender::render_text(text4, WIDTH / 2 - text4.size() * FONT_SIZE / 2, HEIGHT / 2 + FONT_SIZE);

        return;
    }

    SDL_SetRenderDrawColor(renderer, 0x00, 0xB7, 0x0A, 0xFF);
    SDL_RenderFillRect(renderer, NULL);

    // render paths/nodes
    for (auto it = m_nodes.begin(); it < m_nodes.end(); it++)
    {
        float node_x = (float)it->pos.x - m_camera_pos.x;
        float node_y = (float)it->pos.y - m_camera_pos.y;

        // render bonds first
        if (it->bl.has_value())
        {
            bool bond_active = it->bl.value();
            SDL_Texture *bond_tex = bond_active ? m_node_bond_on_tex : m_node_bond_off_tex;

            SDL_Rect bond_dst = SDL_Rect {
                .x = int(node_x * (float)m_NODE_SIZE) - 7 * m_NODE_SIZE / 2,
                .y = int(node_y * (float)m_NODE_SIZE) + m_NODE_SIZE / 2,
                .w = m_NODE_SIZE * 4,
                .h = m_NODE_SIZE * 4
            };

            SDL_RenderCopyEx(renderer, bond_tex, NULL, &bond_dst, 0.0, NULL, SDL_FLIP_HORIZONTAL);
        }

        if (it->br.has_value())
        {
            bool bond_active = it->br.value();
            SDL_Texture *bond_tex = bond_active ? m_node_bond_on_tex : m_node_bond_off_tex;

            SDL_Rect bond_dst = SDL_Rect {
                .x = int(node_x * (float)m_NODE_SIZE) + m_NODE_SIZE / 2,
                .y = int(node_y * (float)m_NODE_SIZE) + m_NODE_SIZE / 2,
                .w = m_NODE_SIZE * 4,
                .h = m_NODE_SIZE * 4
            };

            SDL_RenderCopyEx(renderer, bond_tex, NULL, &bond_dst, 0.0, NULL, SDL_FLIP_NONE);
        }

        // render nodes
        SDL_Rect node_dst = SDL_Rect {
            .x = int(node_x * (float)m_NODE_SIZE),
            .y = int(node_y * (float)m_NODE_SIZE),
            .w = m_NODE_SIZE,
            .h = m_NODE_SIZE
        };

        SDL_Texture *node_tex = it->active ? m_node_on_tex : m_node_off_tex;

        SDL_RenderCopy(renderer, node_tex, NULL, &node_dst);
    }

    // render player
    constexpr int PLAYER_OFFSET = 4;

    float cur_player_x = m_player_pos.x - m_camera_pos.x;
    float cur_player_y = m_player_pos.y - m_camera_pos.y;

    SDL_Rect player_dst = SDL_Rect {
        .x = int(cur_player_x * (float)m_NODE_SIZE) + PLAYER_OFFSET,
        .y = int(cur_player_y * (float)m_NODE_SIZE) - PLAYER_OFFSET,
        .w = m_PLAYER_SIZE,
        .h = m_PLAYER_SIZE
    };

    SDL_RenderCopy(renderer, m_player_tex, NULL, &player_dst);

    int LEVEL_INDEX = m_cur_node - m_nodes.begin() + 1;
    const std::string top_text = "LVL COIN CHKPNT";

    int coins = PlayState::get_coins();
    int checkpoints = PlayState::get_checkpoints();

    std::string bottom_text = " ";
    bottom_text += char(LEVEL_INDEX / 10 + '0');
    bottom_text += char(LEVEL_INDEX % 10 + '0');
    bottom_text += "  ";
    bottom_text += char(coins / 100 + '0');
    bottom_text += char((coins % 100) / 10 + '0');
    bottom_text += char(coins % 10 + '0');
    bottom_text += "    ";
    bottom_text += char(checkpoints / 100 + '0');
    bottom_text += char((checkpoints % 100) / 10 + '0');
    bottom_text += char(checkpoints % 10 + '0');

    FontRender::render_text(top_text, WIDTH / 2 - top_text.size() * FONT_SIZE / 2, 0);
    FontRender::render_text(bottom_text, WIDTH / 2 - top_text.size() * FONT_SIZE / 2, FONT_SIZE);

    if (m_select_bond_mode)
    {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x50);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderFillRect(renderer, NULL);

        std::string text1 = "PLEASE SELECT AN";
        std::string text2 = "EMPTY PATH TO ACTIVATE";
        std::string text3 = "IT OR MOVE TO AN ACTIVE";
        std::string text4 = "PATH UNTIL YOU SELECT ONE";

        FontRender::render_text(text1, WIDTH / 2 - text1.size() * FONT_SIZE / 2, HEIGHT / 2 - FONT_SIZE * 2);
        FontRender::render_text(text2, WIDTH / 2 - text2.size() * FONT_SIZE / 2, HEIGHT / 2 - FONT_SIZE);
        FontRender::render_text(text3, WIDTH / 2 - text3.size() * FONT_SIZE / 2, HEIGHT / 2);
        FontRender::render_text(text4, WIDTH / 2 - text4.size() * FONT_SIZE / 2, HEIGHT / 2 + FONT_SIZE);
    }

    FontRender::render_text("PRESS \"S\" to shop!", 1, HEIGHT - FONT_SIZE - 1);
}

SelectState *SelectState::get()
{
    return &m_select_state;
}

std::optional<int> SelectState::node_search(int x, int y, int lo, int hi)
{
    if (lo < 0 || lo >= m_NODE_COUNT || hi < 0 || hi >= m_NODE_COUNT)
    {
        return {};
    }

    while (lo <= hi)
    {
        int mid = (lo + hi) / 2;

        if (m_nodes.at(mid).pos.y == y)
        {
            if (m_nodes.at(mid).pos.x == x)
            {
                return mid;
            }

            if (m_nodes.at(mid).pos.x > x)
            {
                hi = mid - 1;
            }
            else
            {
                lo = mid + 1;
            }

            continue;
        }

        if (m_nodes.at(mid).pos.y > y)
        {
            hi = mid - 1;
        }
        else
        {
            lo = mid + 1;
        }
    }

    return {};
}

int SelectState::get_cur_lvl()
{
    return m_cur_node - m_nodes.begin() + 1;
}

std::vector<Node>::iterator SelectState::get_cur_node()
{
    return m_cur_node;
}

void SelectState::set_select_bond(bool a)
{
    m_select_bond_mode = a;
}

void SelectState::set_last_played_node(int i)
{
    m_last_played_node = i;
}

void SelectState::go_back_to_state()
{
    SelectState *ss = SelectState::get();

    ss->set_select_bond(true);
    ss->get_cur_node()->active = true;
    ss->set_last_played_node(ss->get_cur_lvl());
}
