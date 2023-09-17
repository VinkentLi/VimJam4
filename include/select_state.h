#pragma once
#include <vector>
#include <string>
#include <optional>
#include "game_state.h"

enum MoveDir
{
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT
};

struct FColor
{
    float r, g, b, a;
};

struct Node
{
    bool active;
    SDL_Point pos;
    std::optional<bool> bl, br;
};

class SelectState : public GameState
{
private:
    SelectState() {}

    static SelectState m_select_state;
    SDL_Texture *m_player_tex;
    SDL_Surface *m_node_surface;
    SDL_Texture *m_node_bond_on_tex;
    SDL_Texture *m_node_bond_off_tex;
    SDL_Texture *m_node_off_tex;
    SDL_Texture *m_node_on_tex;
    std::string m_node_data;
    std::vector<Node> m_nodes;
    std::vector<Node>::iterator m_cur_node;
    SDL_FPoint m_camera_pos;
    SDL_FPoint m_player_pos;
    static constexpr int m_NODE_COUNT = 16;
    static constexpr int m_NODE_SIZE = 24;
    static constexpr int m_PLAYER_SIZE = 16;
    static constexpr float m_PLAYER_SPEED = 0.02;
    static constexpr float m_CAM_W = 32.0f/3.0f;
    static constexpr float m_CAM_H = 6.0f;
    float m_move_timer;
    bool m_move_tl, m_move_tr, m_move_bl, m_move_br;
    bool m_moving;
    MoveDir m_move_dir;

    void parse_node_data();
    std::optional<int> node_search(int x, int y, int lo=0, int hi=m_NODE_COUNT-1);

public:
    void init() override;
    void destroy() override;
    void handle_events(SDL_Event *event) override;
    void update(float dt) override;
    void render() const override;
    int non_static_get_cur_lvl();
    static SelectState *get();
    static int get_cur_lvl();
};