#include <algorithm>
#include <SDL_image.h>
#include "select_state.h"
#include "constants.h"

void SelectState::init()
{
    m_player_tex = IMG_LoadTexture(renderer, "res/gfx/player.png");

    m_node_data =   "06 00 1 1 "
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
    std::sort(m_nodes.begin(), m_nodes.end(), [] (const Node &a, const Node &b) -> bool 
    {
        if (a.pos.y == b.pos.y)
        {
            return a.pos.x < b.pos.x;
        }
        return a.pos.y < b.pos.y;
    }); // sorting to make a binary search to make searching barely faster

    m_cur_node = m_nodes.begin();
    m_camera_pos = SDL_FPoint {.x = 6 - m_cam_w / 2, .y = -m_cam_h/2};
    m_player_pos = SDL_FPoint {.x = 6.0f, .y = 0.0f};
    m_move_timer = 0.0f;
}

void SelectState::destroy()
{

}

void SelectState::parse_node_data()
{
    for (auto it = m_node_data.begin(); it < m_node_data.end(); it++)
    {
        Node n;
        n.active = false;
        n.pos.x = (*it - '0') * 10 + (*(++it)) - '0'; // not sure if this works lol
        it += 2;
        n.pos.y = (*it - '0') * 10 + (*(++it)) - '0';
        it += 2;

        if (*it - '0' != 0)
        {
            n.bl = false;
        }
        else
        {
            n.bl = std::nullopt;
        }

        it += 2;

        if (*it - '0' != 0)
        {
            n.br = false;
        }
        else
        {
            n.br = std::nullopt;
        }
    }
}

void SelectState::handle_events(SDL_Event *event)
{

}

void SelectState::update(float dt)
{

}

void SelectState::render() const
{
    for (int y = 0; y < HEIGHT; y++)
	{
		for (int x = 0; x < WIDTH; x++)
		{
			int i = y + x;
			int j = i * 500;

			int g = std::clamp((j & 0xFF) + 100, 0, 255);

			SDL_SetRenderDrawColor(renderer, 0, g, 0, 255);
			SDL_RenderDrawPoint(renderer, x, y);
		}
	}
}

SelectState *SelectState::get()
{
    static SelectState select_state;
    return &select_state;
}
