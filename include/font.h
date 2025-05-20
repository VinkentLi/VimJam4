#pragma once
#include <SDL.h>
#include <string>

class FontRender
{
private:
    FontRender();
    inline static SDL_Texture *m_FontTexArray = nullptr;
    inline static SDL_Surface *m_FontShadowSurface = nullptr;
    inline static SDL_Renderer *m_Renderer = nullptr;

public:
    static void init(SDL_Renderer *renderer);
    static void destroy();
    static void renderText(const std::string &text, int x, int y, bool shadow=false, uint8_t alpha=0xFF);
};
