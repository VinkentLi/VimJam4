#pragma once
#include <SDL_mixer.h>
#include <vector>
#include <string>
#include <optional>
#include "gameState.h"

enum MoveDir {
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT
};

struct FColor {
    float r, g, b, a;
};

struct Node {
    bool active;
    SDL_Point pos;
    std::optional<bool> bl, br;
};

class SelectState : public GameState {
private:
    SelectState() {}
    static SelectState m_SelectState;
    SDL_Texture *m_PlayerTexture;
    SDL_Surface *m_NodeSurface;
    SDL_Texture *m_NodeBondOnTexture;
    SDL_Texture *m_NodeBondOffTexture;
    SDL_Texture *m_NodeOffTexture;
    SDL_Texture *m_NodeOnTexture;
    Mix_Chunk *m_BuySound;
    Mix_Chunk *m_EnterSound;
    Mix_Chunk *m_UnlockBondSound;
    Mix_Chunk *m_LevelCompleteSound;
    std::string m_NodeData;
    std::vector<Node> m_Nodes;
    std::vector<Node>::iterator m_CurrentNode;
    int m_LastPlayedNode;
    SDL_FPoint m_CameraPosition;
    SDL_FPoint m_PlayerPosition;
    static constexpr int m_NODE_COUNT = 16;
    static constexpr int m_NODE_SIZE = 24;
    static constexpr int m_PLAYER_SIZE = 16;
    static constexpr float m_PLAYER_SPEED = 0.02;
    static constexpr float m_CAM_W = 32.0f/3.0f;
    static constexpr float m_CAM_H = 6.0f;
    float m_MoveTimer;
    bool m_MoveTopLeft, m_MoveTopRight, m_MoveBottomLeft, m_MoveBottomRight;
    bool m_Moving;
    bool m_ShopMode;
    bool m_SelectBondMode;
    bool m_NoDoublePlayMode;
    MoveDir m_MoveDirection;

    void parseNodeData();
    std::optional<int> nodeSeearch(int x, int y, int lo=0, int hi=m_NODE_COUNT-1);

public:
    void init() override;
    void destroy() override;
    void handleEvents(SDL_Event *event) override;
    void update(float dt) override;
    void render() const override;
    static SelectState *get();
    int getCurrentLevel();
    std::vector<Node>::iterator getCurrentNode();
    void setSelectBond(bool a);
    void setLastPlayedNode(int i);
    Mix_Chunk *getLevelCompleteSound();
    static void goBackToState();
};