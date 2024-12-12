#include "Game.h"
void Game::Init(HWND hWnd) {
	D3D_Create(hWnd);
	srand(GetTickCount());
	//sound.Init();
	//sound.Play(SOUND_LABEL_BGM000);

	player.Init(L"asset/image.png", 3, 4); //プレイヤーを初期化
	player.SetPos(-100.0f, -154.0f, 0.0f);     //位置を設定
	player.SetSize(60.0f, 90.0f, 0.0f);  //大きさを設定
	player.SetAngle(0.0f);                 //角度を設定
	player.SetColor(1.0f, 1.0f, 1.0f, 1.0f);

	title.Init(L"asset/Title.png");
	title.SetPos(0.0f, 0.0f, 0.0f);
	title.SetSize(640.0f, 480.0f, 0.0f);

	pause.Init(L"asset/haikei.png");
	pause.SetPos(0.0f, 0.0f, 0.0f);
	pause.SetSize(640.0f, 480.0f, 0.0f);
	pauseFg = false;

	background.Init(L"asset/background.png");
	background.SetPos(0.0f, 0.0f, 0.0f);
	background.SetSize(640.0f, 480.0f, 0.0f);
	background.SetAngle(0.0f);
	for (int x = 0; x < STAGE_X; x++) {
		for (int y = 0; y < STAGE_Y; y++) {
			blocks[x][y].Init(L"asset/block.png");//ブロックを初期化
			blocks[x][y].SetPos(BLOCK_SIZE * (x - STAGE_X / 2), y - STAGE_Y - 200, 00.0f);//位置を設定
			blocks[x][y].SetSize(BLOCK_SIZE, BLOCK_SIZE, 0.0f);//大きさを設定
		}
	}
	for (int x = 0; x < YUKA_X; x++) {
		for (int y = 0; y < YUKA_Y; y++) {
			blocks[x][y].Init(L"asset/block.png");//ブロックを初期化
			blocks[x][y].SetPos(BLOCK_SIZE * (x - YUKA_X / 2), y - YUKA_Y - 130, 00.0f);//位置を設定
			blocks[x][y].SetSize(BLOCK_SIZE, BLOCK_SIZE, 0.0f);//大きさを設定
		}
	}
	scoreboard.Init(L"asset/number.png", 10, 1);
	scoreboard.SetPos(-145.0f, 200.0f, 0.0f);
	scoreboard.SetSize(40.0f, 40.0f, 0.0f);
	score = 0;

	result.Init(L"asset/haikei.png");
	result.SetPos(0.0f, 0.0f, 0.0f);
	result.SetSize(640.0f, 480.0f, 0.0f);



}
// 下にブロックがないときに落下を開始する関数

void Game::Update(void) {
	input.Update();
	switch (scene) {
	case 0://タイトル画面
		if (input.GetKeyTrigger(VK_3)) {
			scene = 1;
			for (int x = 0; x < STAGE_X; x++) {
				for (int y = 0; y < STAGE_Y + 3; y++) {
					data[x][y] = 0;
				}
			}
			state = 0;
			score - 0;
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
		// 2秒ごとに新しいキャラクターを生成 

		DirectX::XMFLOAT3 pos = player.GetPos();

		if (count % 2 == 0) { // キャラクターがいる場合の移動処理（例として毎フレーム移動）
			float x = player.GetPos().x - 1.0f; // 1フレームごとに1単位左に移動 
			player.SetPos(x, player.GetPos().y, player.GetPos().z);
		}


		// 定数
		const float GRAVITY = 10.0f;  // 重力の設定
		const float JUMP_HEIGHT = 6.0f;  // ジャンプの高さ
		const int JUMP_COUNT = 30;  // ジャンプのカウント（最大カウント）

		// 常に重力を適用する（ただしブロックに接触していないときのみ）
		if (!jumpFg && !rakka) {
			bool blockBelow = false; // 足元にブロックがあるかのフラグ
			for (int x = 0; x < STAGE_X; x++) {
				for (int y = 0; y < STAGE_Y; y++) {
					// プレイヤーの位置がブロックの位置と接触したかを判定
					if (pos.x >= blocks[x][y].GetPos().x && pos.x <= blocks[x][y].GetPos().x + BLOCK_SIZE && // X軸の判定
						pos.y - 50.0f <= blocks[x][y].GetPos().y && pos.y - 50.0f >= blocks[x][y].GetPos().y - BLOCK_SIZE) { // Y軸の判定
						blockBelow = true; // 足元にブロックがある
						break;
					}
				}
				if (blockBelow) break; // 足元にブロックが見つかったらループを抜ける
			}

			if (!blockBelow) {
				pos.y -= GRAVITY * 0.1f;  // 重力の適用（毎フレーム少しずつ下に引っ張る）
			}
		}

		// ジャンプの判定 
		if (input.GetKeyTrigger(VK_SPACE) && rakka == false) {  // 落下していない場合にジャンプ開始
			jumpFg = true;  // ジャンプ開始
			count = 0;      // カウントをリセット
		}

		if (jumpFg) {
			if (count < JUMP_COUNT) {  // ジャンプ上昇処理（カウントが30未満の場合）
				pos.y += JUMP_HEIGHT;  // 上昇
				count++;        // カウントを増加
			}
			if (count == JUMP_COUNT) {
				jumpFg = false; // ジャンプ終了
				rakka = true;   // 落下開始
			}
		}

		// 落下処理 (重力適用)
		if (rakka) {
			pos.y -= GRAVITY * 0.1f;  // 重力による落下

			// プレイヤーがブロックと接触したかどうかをチェック
			bool blockBelow = false;  // 足元にブロックがあるかのフラグ
			for (int x = 0; x < STAGE_X; x++) {
				for (int y = 0; y < STAGE_Y; y++) {
					// プレイヤーの位置がブロックの位置と接触したかを判定
					if (pos.x >= blocks[x][y].GetPos().x && pos.x <= blocks[x][y].GetPos().x + BLOCK_SIZE && // X軸の判定
						pos.y >= blocks[x][y].GetPos().y - BLOCK_SIZE && pos.y - 50.0f <= blocks[x][y].GetPos().y) { // Y軸の判定（ブロックの上に触れている場合）

						blockBelow = true;  // 足元にブロックがある
						rakka = false;      // 落下停止
						pos.y = blocks[x][y].GetPos().y + 50.0f; // 接触したブロックの上に位置合わせ
						break; // 一度接触したら他のブロックとの判定は不要
					}
				}
				if (!rakka) break; // 落下を停止したら外側のループも抜ける
			}

			// 足元にブロックがない場合は落下を続ける
			if (!blockBelow) {
				rakka = true; // 落下継続
			}
		}

		// 左への移動
		if (!jumpFg && !rakka) {
			if (input.GetKeyTrigger(VK_A)) {
				// 足元にブロックがあるかチェック
				bool blockBelow = false;
				for (int x = 0; x < STAGE_X; x++) {
					for (int y = 0; y < STAGE_Y; y++) {
						if (pos.x >= blocks[x][y].GetPos().x && pos.x <= blocks[x][y].GetPos().x + BLOCK_SIZE && // X軸の判定
							pos.y - 2.0f <= blocks[x][y].GetPos().y && pos.y - 2.0f >= blocks[x][y].GetPos().y - BLOCK_SIZE) { // Y軸の判定
							blockBelow = true; // ブロックが足元にある
							break;
						}
					}
					if (blockBelow) break; // ブロックが見つかったらループを抜ける
				}

				if (!blockBelow) {
					rakka = true; // 足元にブロックがない場合、落下を開始
				}
			}
		}

		// 右への移動
		if (!jumpFg && !rakka) {
			if (input.GetKeyTrigger(VK_D)) {
				// 足元にブロックがあるかチェック
				bool blockBelow = false;
				for (int x = 0; x < STAGE_X; x++) {
					for (int y = 0; y < STAGE_Y; y++) {
						if (pos.x >= blocks[x][y].GetPos().x && pos.x <= blocks[x][y].GetPos().x + BLOCK_SIZE && // X軸の判定
							pos.y - 2.0f <= blocks[x][y].GetPos().y && pos.y - 2.0f >= blocks[x][y].GetPos().y - BLOCK_SIZE) { // Y軸の判定
							blockBelow = true; // ブロックが足元にある
							break;
						}
					}
					if (blockBelow) break; // ブロックが見つかったらループを抜ける
				}

				if (!blockBelow) {
					rakka = true; // 足元にブロックがない場合、落下を開始
				}
			}
		}
		// 左への移動（Aキー）
		if (input.GetKeyPress(VK_A)) {
			pos.x -= 1.0f; // 左に移動
		}

		// 右への移動（Dキー）
		if (input.GetKeyPress(VK_D)) {
			pos.x += 1.0f; // 右に移動
		}
		player.SetPos(pos.x, pos.y, pos.z);

		for (int x = 0; x < STAGE_X; x++) {
			for (int y = 0; y < STAGE_Y; y++) {
				switch (data[x][y]) {
				case 0:
					blocks[x][y].SetColor(1.0f, 1.0f, 1.0f, 1.0f);
					break;
				case 1:
					blocks[x][y].SetColor(1.0f, 1.0f, 1.0f, 1.0f);
					break;
				}
			}
		}
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
		//ブロックを表示
		for (int x = 0; x < STAGE_X; x++) {
			for (int y = 0; y < STAGE_Y; y++) {
				blocks[x][y].Draw();
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
	kyara.Uninit();
	love.Uninit();
	tarai.Uninit();
	background.Uninit();
	ef.Uninit();
	//ブロックを終了
	for (int x = 0; x < STAGE_X; x++) {
		for (int y = 0; y < STAGE_Y; y++) {
			blocks[x][y].Uninit();
		}
	}
	title.Uninit();
	result.Uninit();
	pause.Uninit();
	//sound.Uninit();
	D3D_Release();
}