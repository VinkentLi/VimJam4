#include <SDL_image.h>
#include <cstdio>
#include "font.h"
#include "constants.h"

void FontRender::init(SDL_Renderer *renderer)
{
    m_renderer = renderer;
    
    if (m_font_tex_array == nullptr)
    {
        m_font_tex_array = IMG_LoadTexture(m_renderer, "res/gfx/font.png");
    }
}

void FontRender::destroy()
{
    SDL_DestroyTexture(m_font_tex_array);
}

void FontRender::render_text(const std::string &text, int x, int y)
{
    const char OFFSET = ' ';
    int orig_x = x;

    for (char c : text)
    {
        // SDL_Log("%c, %d, %d\n", c, x, y);

        int index = c - OFFSET;

        int src_x = (index % FONT_TEX_W) * FONT_SIZE;
        int src_y = (index / FONT_TEX_W) * FONT_SIZE;

        // SDL_Log("%c, %d, %d, %d\n", c, index, src_x, src_y);

        SDL_Rect src = SDL_Rect {.x = src_x, .y = src_y, .w = FONT_SIZE, .h = FONT_SIZE};
        SDL_Rect dst = SDL_Rect {.x = x, .y = y, .w = FONT_SIZE, .h = FONT_SIZE};
        // SDL_Rect dst = {0, 0, 96, 64};

        SDL_RenderCopy(m_renderer, m_font_tex_array, &src, &dst);
        x += FONT_SIZE;

        if (x >= WIDTH)
        {
            x = orig_x;
            y += FONT_SIZE;
        }
    }
}
