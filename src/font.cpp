#include <SDL_image.h>
#include <cstdio>
#include "font.h"
#include "constants.h"

void FontRender::init(SDL_Renderer *renderer) {
    m_Renderer = renderer;
    if (m_FontTexArray == nullptr) {
        m_FontTexArray = IMG_LoadTexture(m_Renderer, "res/gfx/font.png");
    }
    if (m_FontShadowSurface == nullptr) {
        m_FontShadowSurface = IMG_Load("res/gfx/font_shadow.png");
        m_FontShadowSurface = SDL_ConvertSurfaceFormat(m_FontShadowSurface, SDL_PIXELFORMAT_ARGB8888, 0);
    }
}

void FontRender::destroy() {
    SDL_DestroyTexture(m_FontTexArray);
    SDL_FreeSurface(m_FontShadowSurface);
}

void FontRender::renderText(const std::string &text, int x, int y, bool shadow, uint8_t alpha) {
    const char OFFSET = ' ';
    int origX = x;

    for (char c : text) {
        int index = c - OFFSET;
        int srcX = (index % FONT_TEX_W) * FONT_SIZE;
        int srcY = (index / FONT_TEX_W) * FONT_SIZE;
        SDL_Rect src = SDL_Rect {.x = srcX, .y = srcY, .w = FONT_SIZE, .h = FONT_SIZE};
        SDL_Rect dst = SDL_Rect {.x = x, .y = y, .w = FONT_SIZE, .h = FONT_SIZE};
        // SDL_Rect shadow_dst = dst;
        // shadow_dst.x++;
        // shadow_dst.y++;

        // if (shadow)
        // {
        //     if (alpha != 0xFF)
        //     {
        //         uint32_t *pixels = (uint32_t *)m_FontShadowSurface->pixels;

        //         for (int i = 0; i < m_FontShadowSurface->w * m_FontShadowSurface->h; i++)
        //         {
        //             uint32_t not_alpha = pixels[i] & 0x00FFFFFF;
        //             int a = alpha / 0xFF;
        //             pixels[i] = (a << 24) | not_alpha;
        //         }
        //     }

        //     SDL_Texture *font_shadow_tex = SDL_CreateTextureFromSurface(m_renderer, m_FontShadowSurface);
        //     SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
        //     SDL_RenderCopy(m_renderer, font_shadow_tex, &src, &shadow_dst);
        //     SDL_DestroyTexture(font_shadow_tex);
        // }

        SDL_RenderCopy(m_Renderer, m_FontTexArray, &src, &dst);
        x += FONT_SIZE;

        if (x >= WIDTH) {
            x = origX;
            y += FONT_SIZE;
        }
    }
}
