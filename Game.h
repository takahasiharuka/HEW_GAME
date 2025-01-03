#pragma once
#include <vector>
#include <utility>
#include <Windows.h>
#include "Input.h"
#include"Object.h"

#include"Sound.h"
#include <utility>
#define STAGE_X    (30)
#define STAGE_Y    (1)
#define BLOCK_SIZE (20)
#define YOKO_X     (8)
#define YOKO_Y     (1)
#define TATE_X    (1)
#define TATE_Y    (8)

class Game {
private:
    Input input;
    
    Object blocks[STAGE_X][STAGE_Y]; // 床のブロックを保持する配列
    Object Blocks[STAGE_X][STAGE_Y];
    Object player, player2, player3, player4, player5;       // プレイヤーオブジェクト
    Object background, scoreboard, result;
    Object title, pause;                  // その他のオブジェクト
    Object tyu;

    bool pauseFg = false;                 // ポーズ状態のフラグ

    // ゲーム内のフラグと定数
    bool isOnBlock = false, jumpFg = false, rakka = false;
    const float GRAVITY = 10.0f;
    const float JUMP_HEIGHT = 8.0f;
    const float JUMP_SPEED_X = 2.0f;
    const int JUMP_COUNT = 30;

    // プレイヤーの当たり判定用サイズ
    const float PLAYER_WIDTH = 45.0f;
    const float PLAYER_HEIGHT = 60.0f;

    // 衝突判定用フラグ
    bool blockBelow = false, headHit = false;
    bool blockRight = false, blockLeft = false;

    // 画面端の座標制限
    const float SCREEN_LEFT = -300.0f;
    const float SCREEN_RIGHT = 300.0f;
    const float SCREEN_BOTTOM = -300.0f;
    const float SCREEN_TOP = 300.0f;

    int stageData[STAGE_X][STAGE_Y + 3] = {};
    int scene = 0, score = 0, state = 0, count = -100;

public:

    void Init(HWND hWnd);
    void Update();
    void Draw();
    void Uninit();
};