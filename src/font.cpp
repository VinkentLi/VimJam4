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

    if (m_font_shadow_surf == nullptr)
    {
        m_font_shadow_surf = IMG_Load("res/gfx/font_shadow.png");
        m_font_shadow_surf = SDL_ConvertSurfaceFormat(m_font_shadow_surf, SDL_PIXELFORMAT_ARGB8888, 0);
    }
}

void FontRender::destroy()
{
    SDL_DestroyTexture(m_font_tex_array);
    SDL_FreeSurface(m_font_shadow_surf);
}

void FontRender::render_text(const std::string &text, int x, int y, bool shadow, uint8_t alpha)
{
    const char OFFSET = ' ';
    int orig_x = x;

    for (char c : text)
    {
        int index = c - OFFSET;

        int src_x = (index % FONT_TEX_W) * FONT_SIZE;
        int src_y = (index / FONT_TEX_W) * FONT_SIZE;

        SDL_Rect src = SDL_Rect {.x = src_x, .y = src_y, .w = FONT_SIZE, .h = FONT_SIZE};
        SDL_Rect dst = SDL_Rect {.x = x, .y = y, .w = FONT_SIZE, .h = FONT_SIZE};
        // SDL_Rect shadow_dst = dst;
        // shadow_dst.x++;
        // shadow_dst.y++;

        // if (shadow)
        // {
        //     if (alpha != 0xFF)
        //     {
        //         uint32_t *pixels = (uint32_t *)m_font_shadow_surf->pixels;

        //         for (int i = 0; i < m_font_shadow_surf->w * m_font_shadow_surf->h; i++)
        //         {
        //             uint32_t not_alpha = pixels[i] & 0x00FFFFFF;
        //             int a = alpha / 0xFF;
        //             pixels[i] = (a << 24) | not_alpha;
        //         }
        //     }

        //     SDL_Texture *font_shadow_tex = SDL_CreateTextureFromSurface(m_renderer, m_font_shadow_surf);
        //     SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
        //     SDL_RenderCopy(m_renderer, font_shadow_tex, &src, &shadow_dst);
        //     SDL_DestroyTexture(font_shadow_tex);
        // }

        SDL_RenderCopy(m_renderer, m_font_tex_array, &src, &dst);
        x += FONT_SIZE;

        if (x >= WIDTH)
        {
            x = orig_x;
            y += FONT_SIZE;
        }
    }
}
