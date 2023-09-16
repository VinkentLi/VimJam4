#include <stdio.h>
#include "button.h"
#include "constants.h"

Button::Button(SDL_Renderer *renderer, SDL_Rect button_box, SDL_Color color) 
    : m_renderer(renderer), m_button_box(button_box), m_color(color)
{
    m_pressed = false;
}

Button::~Button()
{
}

void Button::render() const
{
    if (!m_pressed)
    {
        SDL_SetRenderDrawColor(m_renderer, m_color.r, m_color.g, m_color.b, 0xFF);
    }
    else
    {
        SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    }

    SDL_RenderFillRect(m_renderer, &m_button_box);
}

void Button::press()
{
    m_pressed = true;
}

void Button::release()
{
    m_pressed = false;
}

void Button::add_color(int add1, int add2, int add3)
{
    m_color.r += add1;
    m_color.g += add2;
    m_color.b += add3;
}

SDL_Rect *Button::get_button_box()
{
    return &m_button_box;
}

bool Button::is_pressed()
{
    return m_pressed;
}
