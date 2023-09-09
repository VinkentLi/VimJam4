#include "button.h"
#include "SDL_image.h"

Button::Button(SDL_Renderer *renderer, SDL_Rect button_box, const char *image_path) 
    : m_renderer(renderer), m_button_box(button_box)
{
    m_pressed = false;
    m_button_texture = IMG_LoadTexture(m_renderer, image_path);
}

Button::~Button()
{
    SDL_DestroyTexture(m_button_texture);
}

void Button::render() const
{
    
}

void Button::press()
{
    m_pressed = true;
}

void Button::release()
{
    m_pressed = false;
}
