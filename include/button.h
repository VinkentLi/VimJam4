#pragma once
#include <SDL.h>

class Button {
private:
    bool m_Pressed;
    SDL_Renderer *m_Renderer;
    SDL_Rect m_ButtonBox;
    SDL_Color m_Color;

public:
    Button() = default;
    Button(SDL_Renderer *renderer, SDL_Rect button_box, SDL_Color color);
    ~Button();
    void render() const;
    void press();
    void release();
    void addColor(int add1, int add2, int add3);
    SDL_Rect *getButtonBox();
    bool isPressed();
};
