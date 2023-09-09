#pragma once
#include <SDL.h>

class Button
{
private:
    SDL_Renderer *m_renderer;
    SDL_Texture *m_button_texture;
    bool m_pressed;
    SDL_Rect m_button_box;

public:
    Button(SDL_Renderer *renderer, SDL_Rect button_box, const char *image_path);
    ~Button();
    void render() const;
    void press();
    void release();
};
