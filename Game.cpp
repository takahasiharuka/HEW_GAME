#include "Game.h"
#include <iostream>
#include <string> 
#include "direct3d.h"
//#include "Input.h"
#include "Keyboard.h"
#include "Audio.h"
int YokoX = YOKO_X; // 現在の地面の横幅
int YokoY = YOKO_Y; // 現在の地面の高さ
int TateX = TATE_X;
int TateY = TATE_Y;
void Game::Init(HWND hWnd) {
	D3D_Create(hWnd);
	srand(GetTickCount());

	// プレイヤーの初期化
	player.Init(L"asset/char01.png", 3, 4);
	player.SetPos(-100.0f, -154.0f, 0.0f);
	player.SetSize(45.0f, 60.0f, 0.0f);

	player2.Init(L"asset/char01.png", 3, 4);//右判定
	player2.SetPos(-90.0f, -174.0f, 0.0f);
	player2.SetSize(40.0f, 20.0f, 0.0f);
	player2.SetColor(1.0f, 1.0f, 1.0f, 0.0f);

	player3.Init(L"asset/char01.png", 3, 4);//左判定
	player3.SetPos(-110.0f, -164.0f, 0.0f);
	player3.SetSize(40.0f, 20.0f, 0.0f);
	player3.SetColor(1.0f, 1.0f, 1.0f, 0.0f);

	player4.Init(L"asset/char01.png", 3, 4);//上判定
	player4.SetPos(-100.0f, -134.0f, 0.0f);
	player4.SetSize(40.0f, 20.0f, 0.0f);
	player4.SetColor(1.0f, 1.0f, 1.0f, 0.5f);

	player5.Init(L"asset/char01.png", 3, 4);//下判定
	player5.SetPos(-100.0f, -194.0f, 0.0f);
	player5.SetSize(10.0f, 5.0f, 0.0f);
	player5.SetColor(1.0f, 1.0f, 0.0f, 1.0f);

	// 背景やその他オブジェクトの初期化
	background.Init(L"asset/stage.jpg");
	background.SetPos(0.0f, 0.0f, 0.0f);
	background.SetSize(2000.0f, 1300.0f, 0.0f);

	scoreboard.Init(L"asset/number.png", 10, 1);
	scoreboard.SetPos(-145.0f, 200.0f, 0.0f);
	scoreboard.SetSize(40.0f, 40.0f, 0.0f);

	title.Init(L"asset/Title.png");
	title.SetPos(0.0f, 0.0f, 0.0f);
	title.SetSize(1920.0f, 1080.0f, 0.0f);

	pause.Init(L"asset/haikei.png");
	pause.SetPos(0.0f, 0.0f, 0.0f);
	pause.SetSize(1920.0f, 1080.0f, 0.0f);

	result.Init(L"asset/result1.png");
	result.SetPos(0.0f, 0.0f, 0.0f);
	result.SetSize(1920.0f, 1080.0f, 0.0f);

	// ステージの床の初期化
	for (int x = 0; x < STAGE_X; x++) {
		for (int y = 0; y < STAGE_Y; y++) {
			blocks[x][y].Init(L"asset/block.png");
			blocks[x][y].SetPos(BLOCK_SIZE * (x - STAGE_X / 2) - 200, BLOCK_SIZE * y - 200, 0.0f);
			blocks[x][y].SetSize(BLOCK_SIZE, BLOCK_SIZE, 0.0f);
		}
	}
	//横ブロック
	for (int x = 0; x < YokoX; x++) {
		for (int y = 0; y < YokoY; y++) {
			blocks[x][y].Init(L"asset/block.png");
			blocks[x][y].SetPos(BLOCK_SIZE * (x - YokoX / 2) - 30, BLOCK_SIZE * (y - YokoY / 2) - 100, 0.0f);
			blocks[x][y].SetSize(BLOCK_SIZE, BLOCK_SIZE, 0.0f);
		}
	}

	// 縦ブロックの初期化
	for (int x = 0; x < TateX; x++) {
		for (int y = 0; y < TateY; y++) {
			blocks[x + YokoX][y].Init(L"asset/block.png");
			blocks[x + YokoX][y].SetPos(BLOCK_SIZE * (x - TateX / 2), BLOCK_SIZE * (y - TateY / 2) - 130, 0.0f);
			blocks[x + YokoX][y].SetSize(BLOCK_SIZE, BLOCK_SIZE, 0.0f);
		}
	}
	tyu.Init(L"asset/krober.jpg");
	tyu.SetPos(-5.0f, -135.0f, 0.0f); // 横ブロックと縦ブロックの交点に配置
	tyu.SetSize(10.0f, 10.0f, 0.0f);

}
// 下にブロックがないときに落下を開始する関数


void Game::Update(void) {
	input.Update();
	switch (scene) {
	case 0://タイトル画面
		if (input.GetKeyTrigger(VK_3)) {
			scene = 1;
		}
		break;
	case 1:
	{

		if (state == 3)
		{
			if (input.GetKeyTrigger(VK_2)) {
				scene = 0;
			}
			return;
		}
		if (pauseFg == true)
		{
			//ポーズ
			if (input.GetKeyTrigger(VK_1)) {
				pauseFg = false;
			}
			//タイトル
			if (input.GetKeyTrigger(VK_2)) {
				pauseFg = false;
				scene = 0;
			}
			return;
		}
		if (input.GetKeyTrigger(VK_1)) {
			pauseFg = true;
			return;
		}
		count++;
		//player.numU++;
		//if (player.numU >= 3) {
			//player.numU = 0;
		//}

		// プレイヤーの処理
		DirectX::XMFLOAT3 pos = player.GetPos();
		DirectX::XMFLOAT3 pos2 = player2.GetPos(); // player2 の座標を取得
		DirectX::XMFLOAT3 pos3 = player3.GetPos(); // player3 の座標を取得
		DirectX::XMFLOAT3 pos4 = player4.GetPos(); // player4 の座標を取得
		DirectX::XMFLOAT3 pos5 = player5.GetPos(); // player5 の座標を取得
		bool blockCollision = false; // player2 とブロックの接触フラグ
		bool blockCollision2 = false; // player3 とブロックの接触フラグ
		bool blockCollision3 = false; // player4 とブロックの接触フラグ
		bool blockCollision4 = false; // player5とブロックの接触フラグ
		if (pos.y <= -220.0f) {
			// 初期位置に戻す
			pos.x = -100.0f;
			pos.y = -154.0f;
			pos.z = 0.0f;
		}
		// player2 とブロックの衝突判定
		for (int x = 0; x < STAGE_X; x++) {
			for (int y = 0; y < STAGE_Y; y++) {
				if (pos2.x >= blocks[x][y].GetPos().x && pos2.x <= blocks[x][y].GetPos().x + BLOCK_SIZE &&
					pos2.y >= blocks[x][y].GetPos().y - BLOCK_SIZE && pos2.y - 10.0f <= blocks[x][y].GetPos().y) {
					blockCollision = true;
					break;
				}
			}
			if (blockCollision) break;
		}
		//player3とブロックの衝突判定
		for (int x = 0; x < STAGE_X; x++) {
			for (int y = 0; y < STAGE_Y; y++) {
				if (pos3.x >= blocks[x][y].GetPos().x && pos3.x <= blocks[x][y].GetPos().x + BLOCK_SIZE &&
					pos3.y >= blocks[x][y].GetPos().y - BLOCK_SIZE && pos3.y - 10.0f <= blocks[x][y].GetPos().y) {
					blockCollision2 = true;
					break;
				}
			}
			if (blockCollision2) break;
		}
		//player4とブロックの衝突判定
		for (int x = 0; x < STAGE_X; x++) {
			for (int y = 0; y < STAGE_Y; y++) {
				if (pos4.x >= blocks[x][y].GetPos().x && pos4.x <= blocks[x][y].GetPos().x + BLOCK_SIZE &&
					pos4.y >= blocks[x][y].GetPos().y - BLOCK_SIZE && pos4.y <= blocks[x][y].GetPos().y) {
					blockCollision3 = true;

					break;
				}
			}
			if (blockCollision3) break;
		}
		const int JUMP_COUNT = 30;  // ジャンプのカウント（最大カウント）
		if (!jumpFg && !rakka) {
			bool blockBelow = false; // 足元にブロックがあるかのフラグ
			for (int x = 0; x < STAGE_X; x++) {
				for (int y = 0; y < STAGE_Y; y++) {
					if (pos5.x >= blocks[x][y].GetPos().x && pos5.x <= blocks[x][y].GetPos().x + BLOCK_SIZE &&
						pos5.y <= blocks[x][y].GetPos().y && pos.y >= blocks[x][y].GetPos().y - BLOCK_SIZE) {
						blockBelow = true;
						break;
					}
				}
				if (blockBelow) break;
			}

			if (!blockBelow) {
				pos.y -= GRAVITY * 0.4f; // 重力を適用
			}
		}

		if (input.GetKeyTrigger(VK_SPACE) && rakka == false && !jumpFg) {  // 落下していない場合にジャンプ開始
			jumpFg = true;  // ジャンプ開始
			count = 0;      // カウントをリセット
			float right_x = blocks[YOKO_X - 1][0].GetPos().x; // 右端のx座標を基準
			float right_y = blocks[YOKO_X - 1][0].GetPos().y; // 右端のy座標を基準
			for (int x = 0; x < YOKO_X; x++) {
				for (int y = 0; y < YOKO_Y; y++) {
					float old_x = blocks[x][y].GetPos().x - right_x;
					float old_y = blocks[x][y].GetPos().y - right_y;
					float new_x = -old_y + right_x;
					float new_y = old_x + right_y;
					blocks[x][y].SetPos(new_x, new_y, 0.0f);
				}
			}

			// 縦ブロックの回転
			float top_x = blocks[YOKO_X - 1][0].GetPos().x; // 上端のx座標を基準
			float top_y = blocks[YOKO_X - 1][0].GetPos().y; // 上端のy座標を基準
			for (int x = 0; x < TATE_X; x++) {
				for (int y = 0; y < TATE_Y; y++) {
					float old_x = blocks[x + YOKO_X][y].GetPos().x - top_x;
					float old_y = blocks[x + YOKO_X][y].GetPos().y - top_y;
					float new_x = -old_y + top_x;
					float new_y = old_x + top_y;
					blocks[x + YOKO_X][y].SetPos(new_x, new_y, 0.0f);
				}
			}

		}

		if (jumpFg) {
			if (count < JUMP_COUNT) {
				pos.y += JUMP_HEIGHT; // player のジャンプ上昇
				count++;
			}
			if (count == JUMP_COUNT) {
				jumpFg = false; // ジャンプ終了
				rakka = true;   // 落下開始
			}
		}
		if (rakka || blockCollision3) {
			pos.y -= GRAVITY * 0.4f;  // player の重力による落下

			// player1 の着地判定
			bool blockBelow = false;
			for (int x = 0; x < STAGE_X; x++) {
				for (int y = 0; y < STAGE_Y; y++) {
					if (pos5.x >= blocks[x][y].GetPos().x && pos5.x <= blocks[x][y].GetPos().x + BLOCK_SIZE &&
						pos5.y >= blocks[x][y].GetPos().y - BLOCK_SIZE && pos5.y <= blocks[x][y].GetPos().y) {
						blockBelow = true;
						rakka = false;
						pos5.y = blocks[x][y].GetPos().y; // player の位置合わせ

						break;
					}
				}
				if (!rakka) break;
			}

			if (!blockBelow) {
				rakka = true; // 足元にブロックがなければ落下継続
			}
		}

		// 横方向の移動制限
		// A, D キーの処理制限
		if (!blockCollision && input.GetKeyPress(VK_D)) {
			pos.x += 1.f; // player を右に移動

		}

		// Aキーの動作制限（左移動）
		// Aキーの動作制限（左移動）
		if (!blockCollision2 && input.GetKeyPress(VK_A)) {
			pos.x -= 1.0f; // player を右に移動

		}
		// player2 と player3 の x 座標を更新
		pos2.x = pos.x + 10.0f; // player2 は player の x + 10
		pos2.y = pos.y - 2.0f; // player2 は player の y - 20

		pos3.x = pos.x - 10.0f; // player3 は player の x - 10
		pos3.y = pos.y - 2.0f; // player3 は player の y - 10
		pos4.x = pos.x - 0.0f; // player3 は player の x - 10
		pos4.y = pos.y + 30.0f; // player3 は player の y - 10
		pos5.x = pos.x - 0.0f; // player3 は player の x - 10
		pos5.y = pos.y - 30.0f; // player3 は player の y - 10
		// 画面外に行かないように位置を制限
		if (pos.x < SCREEN_LEFT) pos.x = SCREEN_LEFT;
		if (pos.x > SCREEN_RIGHT) pos.x = SCREEN_RIGHT;
		if (pos.y < SCREEN_BOTTOM) pos.y = SCREEN_BOTTOM;
		if (pos.y > SCREEN_TOP) pos.y = SCREEN_TOP;

		if (pos2.x < SCREEN_LEFT) pos2.x = SCREEN_LEFT;
		if (pos2.x > SCREEN_RIGHT) pos2.x = SCREEN_RIGHT;
		if (pos2.y < SCREEN_BOTTOM) pos2.y = SCREEN_BOTTOM;
		if (pos2.y > SCREEN_TOP) pos2.y = SCREEN_TOP;

		if (pos3.x < SCREEN_LEFT) pos3.x = SCREEN_LEFT;
		if (pos3.x > SCREEN_RIGHT) pos3.x = SCREEN_RIGHT;
		if (pos3.y < SCREEN_BOTTOM) pos3.y = SCREEN_BOTTOM;
		if (pos3.y > SCREEN_TOP) pos3.y = SCREEN_TOP;

		if (pos4.x < SCREEN_LEFT) pos4.x = SCREEN_LEFT;
		if (pos4.x > SCREEN_RIGHT) pos4.x = SCREEN_RIGHT;
		if (pos4.y < SCREEN_BOTTOM) pos4.y = SCREEN_BOTTOM;
		if (pos4.y > SCREEN_TOP) pos4.y = SCREEN_TOP;

		if (pos5.x < SCREEN_LEFT) pos5.x = SCREEN_LEFT;
		if (pos5.x > SCREEN_RIGHT) pos5.x = SCREEN_RIGHT;
		if (pos5.y < SCREEN_BOTTOM) pos5.y = SCREEN_BOTTOM;
		if (pos5.y > SCREEN_TOP) pos5.y = SCREEN_TOP;
		// 座標を設定
		player.SetPos(pos.x, pos.y, pos.z);
		player2.SetPos(pos2.x, pos2.y, pos2.z);
		player3.SetPos(pos3.x, pos3.y, pos3.z);
		player4.SetPos(pos4.x, pos4.y, pos4.z);
		player5.SetPos(pos5.x, pos5.y, pos5.z);
	}
	break;
	case 2:

		if (input.GetKeyTrigger(VK_3)) {
			scene = 0;
		}
		break;
	}
}
void Game::Draw(void) {
	D3D_StartRender();

	switch (scene) {
	case 0:
		title.Draw();
		break;
	case 1:
	{

		background.Draw();
		player.Draw();
		player2.Draw();
		player3.Draw();
		player4.Draw();
		player5.Draw();
		tyu.Draw();
		//ブロックを表示
		for (int x = 0; x < STAGE_X; x++) {
			for (int y = 0; y < STAGE_Y; y++) {
				blocks[x][y].Draw(); // 各ブロックの描画を呼び出す
			}
		}
		for (int x = 0; x < YokoX; x++) {
			for (int y = 0; y < YokoY; y++) {
				blocks[x][y].Draw();
			}
		}

		for (int x = 0; x < TateX; x++) {
			for (int y = 0; y < TateY; y++) {
				blocks[x + YokoX][y].Draw();
			}
		}
		DirectX::XMFLOAT3 pos = scoreboard.GetPos();
		DirectX::XMFLOAT3 size = scoreboard.GetSize();
		int keta = 0;
		do {
			scoreboard.numU = score % (int)pow(10, keta + 1) / (int)pow(10, keta);
			scoreboard.SetPos(pos.x - size.x * keta, pos.y, pos.z);
			scoreboard.Draw();
			keta++;
		} while (score >= (int)pow(10, keta));
		scoreboard.SetPos(pos.x, pos.y, pos.z);
		if (pauseFg == true) {
			pause.Draw();
		}
	}
	break;

	case 2:
		result.Draw();
		DirectX::XMFLOAT3 pos = { 0,0,0 };
		DirectX::XMFLOAT3 size = scoreboard.GetSize();
		int keta = 0;
		do {
			scoreboard.numU = score % (int)pow(10, keta + 1) / (int)pow(10, keta);
			scoreboard.SetPos(pos.x - size.x * keta, pos.y, pos.z);
			scoreboard.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
			scoreboard.Draw();
			keta++;
		} while (score >= (int)pow(10, keta));

		if (pauseFg == true) {
			pause.Draw();
		}

		break;

	}

	D3D_FinishRender();
}

void Game::Uninit(void) {
	player.Uninit();
	player2.Uninit();
	player3.Uninit();
	player4.Uninit();
	player5.Uninit();
	tyu.Uninit();
	background.Uninit();
	scoreboard.Uninit();

	//ブロックを終了
	for (int x = 0; x < STAGE_X; x++) {
		for (int y = 0; y < STAGE_Y; y++) {
			blocks[x][y].Uninit();
		}
	}
	for (int x = 0; x < YokoX; x++) {
		for (int y = 0; y < YokoY; y++) {
			blocks[x][y].Uninit();
		}
	}

	for (int x = 0; x < TateX; x++) {
		for (int y = 0; y < TateY; y++) {
			blocks[x + YokoX][y].Uninit();
		}
	}
	title.Uninit();
	result.Uninit();
	pause.Uninit();
	//sound.Uninit();
	D3D_Release();
}
