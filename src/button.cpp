#include <stdio.h>
#include "button.h"
#include "constants.h"

Button::Button(SDL_Renderer *renderer, SDL_Rect buttonBox, SDL_Color color) 
    : m_Renderer(renderer), m_ButtonBox(buttonBox), m_Color(color) {
    m_Pressed = false;
}

Button::~Button() {
}

void Button::render() const {
    if (!m_Pressed) {
        SDL_SetRenderDrawColor(m_Renderer, m_Color.r, m_Color.g, m_Color.b, 0xFF);
    }
    else {
        SDL_SetRenderDrawColor(m_Renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    }
    SDL_RenderFillRect(m_Renderer, &m_ButtonBox);
}

void Button::press() {
    m_Pressed = true;
}

void Button::release() {
    m_Pressed = false;
}

void Button::addColor(int add1, int add2, int add3) {
    m_Color.r += add1;
    m_Color.g += add2;
    m_Color.b += add3;
}

SDL_Rect *Button::getButtonBox() {
    return &m_ButtonBox;
}

bool Button::isPressed() {
    return m_Pressed;
}
