#pragma once
#include"Object.h"
#include"input.h"
#include"Sound.h"
#define STAGE_X    (100)
#define STAGE_Y    (1)
#define BLOCK_SIZE (10)
#define YUKA_X    (10)
#define YUKA_Y    (1)

class Game {
private:
	Input input;
	Object player;
	Object kyara;
	Object love;
	Object tarai;
	Object background;
	Object scoreboard;
	Object blocks[STAGE_X][STAGE_Y];
	Object ef;
	bool isOnBlock = false;
	bool jumpFg = false;
	bool rakka = false;



	// 前のフレームの時間を保存 

	Sound sound;
	typedef struct
	{
		LPCSTR filename;
		bool bloop;
	}PARAM;

	PARAM m_param[SOUND_LABEL_MAX] =
	{
		{"asset/BGM/sample001.wav",true}
	};
	int data[STAGE_X][STAGE_Y + 3] = {};
	int count = -100;
	int state = 0;//ゲームの状態(0:落下するものがない、１:落下中)
	int score = 0;
	bool effect = false;
	Object pause;//ポーズ画面オブジェクト」
	bool pauseFg = false;//ポーズフラグ
	int scene = 0;

	Object title;
	Object result;
	bool blockBelow = false;
public:
	void Init(HWND hWnd);
	void Update(void);
	void Draw();
	void Uninit();

};

