#pragma once
#include <SDL.h>

class Button
{
private:
    bool m_pressed;
    SDL_Renderer *m_renderer;
    SDL_Rect m_button_box;
    SDL_Color m_color;

public:
    Button() = default;
    Button(SDL_Renderer *renderer, SDL_Rect button_box, SDL_Color color);
    ~Button();
    void render() const;
    void press();
    void release();
    void add_color(int add1, int add2, int add3);
    SDL_Rect *get_button_box();
    bool is_pressed();
};
