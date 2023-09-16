#pragma once
#include <vector>
#include <string>
#include <optional>
#include "game_state.h"

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

    SDL_Texture *m_player_tex;
    std::string m_node_data;
    std::vector<Node> m_nodes;
    std::vector<Node>::iterator m_cur_node;
    SDL_FPoint m_camera_pos;
    SDL_FPoint m_player_pos;
    static constexpr int m_node_count = 16;
    static constexpr float m_player_speed = 0.1;
    static constexpr float m_cam_w = 10.67f;
    static constexpr float m_cam_h = 6.0f;
    float m_move_timer;

    void parse_node_data();
    void node_search(int x, int y);

public:
    void init() override;
    void destroy() override;
    void handle_events(SDL_Event *event) override;
    void update(float dt) override;
    void render() const override;
    static SelectState *get();
};