#pragma once
#include <SDL.h>
#include <string>

class FontRender
{
private:
    FontRender();
    inline static SDL_Texture *m_font_tex_array = nullptr;
    inline static SDL_Surface *m_font_shadow_surf = nullptr;
    inline static SDL_Renderer *m_renderer = nullptr;

public:
    static void init(SDL_Renderer *renderer);
    static void destroy();
    static void render_text(const std::string &text, int x, int y, bool shadow=false, uint8_t alpha=0xFF);
};
