#pragma once
/*
 @version	3.0
*/

#include	<locale.h>
#include	<stdio.h>
#include	<conio.h>
#include	<stdlib.h>
#include	<malloc.h>
#include	<string.h>
#include	<mbstring.h>
#include	<process.h>
#include	<windows.h>
#include	<tchar.h>
#include	<mmsystem.h>
#include	<digitalv.h>
#include	<xinput.h>

//#include	<thread>

#include	<CRTDBG.H>	//OutputDebugString();

#define _USE_MATH_DEFINES
#include	<cmath>
//#include <math.h>
#define	M_PI	3.1415926535897940041763831686694175004959106445312500

#define	 USED2D	//������Direct2D�̗��p������

#ifdef USED2D
#include	<d3d11_1.h>
#include	<d2d1_3.h>	//
#include	<dwrite.h>	//
#include	<wchar.h>	//
#include	<directxcolors.h>
#include	<wincodec.h>
#include	<wincodecsdk.h>
#include	<wrl/client.h>
//#include	<wincodec_proxy.h>

// lib
#pragma comment( lib, "d3d11.lib")
#pragma comment( lib, "d2d1.lib" )
#pragma comment( lib, "dwrite.lib" )
#pragma comment( lib, "windowscodecs.lib")

// palette
const int NUM_D2D_PAL = 256;
#endif // USED2D

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "xinput.lib")
#pragma warning(disable:4996)

#define NOCURSOR		0
#define SOLIDCURSOR		1
#define NORMALCURSOR	2

#define DEF_FONTSIZE_X		16
#define DEF_FONTSIZE_Y		16

// https://en.wikipedia.org/wiki/Color_Graphics_Adapter
enum COLORS {
	BLACK,			/* #000000	��				*/
	BLUE,			/* #0000AA	��				*/
	GREEN,			/* #00AA00	��				*/
	CYAN,			/* #00AAAA	�V�A��			*/
	RED,			/* #AA0000	��				*/
	MAGENTA,		/* #AA00AA	�}�[���^		*/
	BROWN,			/* #AA5500	��				*/
	LIGHTGRAY,		/* #AAAAAA	���邢�D�F		*/
	DARKGRAY,		/* #555555	�Â��D�F		*/
	LIGHTBLUE,		/* #5555FF	���邢��		*/
	LIGHTGREEN,		/* #55FF55	���邢��		*/
	LIGHTCYAN,		/* #55FFFF	���邢�V�A��	*/
	LIGHTRED,		/* #FF5555	���邢��		*/
	LIGHTMAGENTA,	/* #FF55FF	���邢�}�[���^	*/
	YELLOW,			/* #FFFF55	��				*/
	WHITE			/* #FFFFFF	��				*/
};

/* �L�[�R�[�h */
#define PK_ENTER				VK_RETURN
#define PK_ESC					VK_ESCAPE
#define PK_F1					VK_F1
#define PK_F2					VK_F2
#define PK_F3					VK_F3
#define PK_F4					VK_F4
#define PK_F5					VK_F5
#define PK_F6					VK_F6
#define PK_F7					VK_F7
#define PK_F8					VK_F8
#define PK_F9					VK_F9
#define PK_F10					VK_F10
#define PK_F11					VK_F11
#define PK_F12					VK_F12
#define PK_SP					VK_SPACE
#define PK_UP					VK_UP
#define PK_DOWN					VK_DOWN
#define PK_RIGHT				VK_RIGHT
#define PK_LEFT					VK_LEFT
#define PK_SHIFT				VK_SHIFT
#define PK_CTRL					VK_CONTROL
#define PK_ALT					VK_MENU
#define PK_BS					VK_BACK
#define PK_PAUSE				VK_PAUSE
#define PK_INS					VK_INSERT
#define PK_DEL					VK_DELETE
#define PK_TAB					VK_TAB
#define PK_NFER					VK_KANA		/* [���ϊ�]	*/
#define PK_XFER					VK_CONVERT	/* [�ϊ�]	*/
#define PK_0					0x30
#define PK_1					0x31
#define PK_2					0x32
#define PK_3					0x33
#define PK_4					0x34
#define PK_5					0x35
#define PK_6					0x36
#define PK_7					0x37
#define PK_8					0x38
#define PK_9					0x39
#define PK_NUMPAD0				VK_NUMPAD0	/* �e���L�[��[0]	*/
#define PK_NUMPAD1				VK_NUMPAD1	/* �e���L�[��[1]	*/
#define PK_NUMPAD2				VK_NUMPAD2	/* �e���L�[��[2]	*/
#define PK_NUMPAD3				VK_NUMPAD3	/* �e���L�[��[3]	*/
#define PK_NUMPAD4				VK_NUMPAD4	/* �e���L�[��[4]	*/
#define PK_NUMPAD5				VK_NUMPAD5	/* �e���L�[��[5]	*/
#define PK_NUMPAD6				VK_NUMPAD6	/* �e���L�[��[6]	*/
#define PK_NUMPAD7				VK_NUMPAD7	/* �e���L�[��[7]	*/
#define PK_NUMPAD8				VK_NUMPAD8	/* �e���L�[��[8]	*/
#define PK_NUMPAD9				VK_NUMPAD9	/* �e���L�[��[9]	*/
#define PK_A					0x41
#define PK_B					0x42
#define PK_C					0x43
#define PK_D					0x44
#define PK_E					0x45
#define PK_F					0x46
#define PK_G					0x47
#define PK_H					0x48
#define PK_I					0x49
#define PK_J					0x4A
#define PK_K					0x4B
#define PK_L					0x4C
#define PK_M					0x4D
#define PK_N					0x4E
#define PK_O					0x4F
#define PK_P					0x50
#define PK_Q					0x51
#define PK_R					0x52
#define PK_S					0x53
#define PK_T					0x54
#define PK_U					0x55
#define PK_V					0x56
#define PK_W					0x57
#define PK_X					0x58
#define PK_Y					0x59
#define PK_Z					0x5A

#define PK_LT					0x0BC		/* [,]	*/
#define PK_GT					0x0BE		/* [.]	*/
#define PK_SLUSH				0x0BF		/* [?]	*/
#define PK_DOT					VK_DECIMAL	/* �e���L�[��[.]	*/
#define PK_DIV					VK_DIVIDE	/* �e���L�[��[/]	*/
#define PK_BSLUSH				0x0E2		/* [_]	*/

#define PK_SEMICOLON			0x0BB		/* [;]	*/
#define PK_ADD					VK_ADD		/* �e���L�[��[+]	*/
#define PK_COLON				0x0BA		/* [:]	*/
#define PK_MUL					VK_MULTIPLY	/* �e���L�[��[*]	*/
#define PK_RBRACE				0x0DD		/* []]	*/

#define PK_ATMARK				0x0C0		/* [@]	*/
#define PK_LBRACE				0x0DB		/* [[]	*/

#define PK_MINUS				0x0BD		/* [-]	*/
#define PK_SUB					VK_SUBTRACT	/* �e���L�[��[-]	*/
#define PK_XOR					0x0DE		/* [^]	*/
#define PK_YEN					0x0DC		/* [\]	*/

#define PK_KANJI				0x0F3		/* [���p/�S�p]	*/
#define PK_CAPS					0x0F0		/* [�p��][�Ђ炪��]	*/

#define PM_LEFT					VK_LBUTTON
#define PM_MID					VK_MBUTTON
#define PM_RIGHT				VK_RBUTTON
#define PM_CURX					0x0101
#define PM_CURY					0x0102

#define PJ1_XPOS				0x0200
#define PJ1_YPOS				0x0201
#define PJ1_ZPOS				0x0202
#define PJ1_BTNS				0x0203
#define PJ2_XPOS				0x0210
#define PJ2_YPOS				0x0211
#define PJ2_ZPOS				0x0212
#define PJ2_BTNS				0x0213
#define PJ3_XPOS				0x0220
#define PJ3_YPOS				0x0221
#define PJ3_ZPOS				0x0222
#define PJ3_BTNS				0x0223
#define PJ4_XPOS				0x0230
#define PJ4_YPOS				0x0231
#define PJ4_ZPOS				0x0232
#define PJ4_BTNS				0x0233
#define PJ_XPOS					PJ1_XPOS
#define PJ_YPOS					PJ1_YPOS
#define PJ_ZPOS					PJ1_ZPOS
#define PJ_BTNS					PJ1_BTNS

#define PJX1_LXPOS				0x0200
#define PJX1_LYPOS				0x0201
#define PJX1_LTRG				0x0202
#define PJX1_RXPOS				0x0203
#define PJX1_RYPOS				0x0204
#define PJX1_RTRG				0x0205
#define PJX1_BTNS				0x0206
#define PJX2_LXPOS				0x0210
#define PJX2_LYPOS				0x0211
#define PJX2_LTRG				0x0212
#define PJX2_RXPOS				0x0213
#define PJX2_RYPOS				0x0214
#define PJX2_RTRG				0x0215
#define PJX2_BTNS				0x0216
#define PJX3_LXPOS				0x0220
#define PJX3_LYPOS				0x0221
#define PJX3_LTRG				0x0222
#define PJX3_RXPOS				0x0223
#define PJX3_RYPOS				0x0224
#define PJX3_RTRG				0x0225
#define PJX3_BTNS				0x0226
#define PJX4_LXPOS				0x0230
#define PJX4_LYPOS				0x0231
#define PJX4_LTRG				0x0232
#define PJX4_RXPOS				0x0233
#define PJX4_RYPOS				0x0234
#define PJX4_RTRG				0x0235
#define PJX4_BTNS				0x0236
#define PJX_LXPOS				PJX1_LXPOS
#define PJX_LYPOS				PJX1_LYPOS
#define PJX_LTRG				PJX1_LTRG
#define PJX_RXPOS				PJX1_RXPOS
#define PJX_RYPOS				PJX1_RYPOS
#define PJX_RTRG				PJX1_RTRG
#define PJX_BTNS				PJX1_BTNS

#define	NUM_KEYS		(256)	//�L�[�o�b�t�@�ɓǂݎ��L�[�̐�
#define	NUM_PALETTE		(16)	//�J���[�p���b�g���i��ʂ̐F���F�S�r�b�g�J���[�j
#define	NUM_ANSI_PAL	(256)	//ANIS 256 colors.

extern const COLORREF ANSI_PAL256_COLOR[NUM_ANSI_PAL];
extern const RGBQUAD ANSI_PAL256_RGB[NUM_ANSI_PAL];

#ifdef USED2D
/*
* LoadBmp�œǂݍ��񂾉摜�f�[�^���i�[����\���́B
*/
struct Bmp {
	int width;		//�摜�̉������s�N�Z����
	int height;		//�摜�̏c�����s�N�Z����
	int colbit;		//�e�s�N�Z���̐F�r�b�g��
	int numpal;		//�p���b�g�̐F��
	int numpix;		//�摜�f�[�^�̃o�C�g��
	bool swapRB;	//24�r�b�g�ȏ�̉摜�ŁAR��B������ւ����Ă���ꍇ��'TRUE'�ɂȂ�
	//�p���b�g�f�[�^�z��̌^��{COLORREF|RGBQUAD}��LoadBmp()���ɑI���B�f�t�H���gCOLORREF�^�i�j
	COLORREF* pal;		//COLORREF�p���b�g�i�Q�`�U�T�F�j�f�[�^�ւ̃|�C���^�������ꍇ��NULL
	RGBQUAD* pal_rgb;	//RGBQUAD�p���b�g�i�Q�`�U�T�F�j�f�[�^�ւ̃|�C���^�������ꍇ��NULL
	char* pixel;	//�摜�f�[�^�ւ̃|�C���^
	//�ȉ���Biitmap�����p
	int aaLv;		//�~�����i�A���`�G�C���A�X���x���jWin32API��GetGlyphOutline�֐��Q��
	//GGO_BITMAP�F�Q�F{0|1}
	//GGO_GRAY2_BITMAP�F�T�F{0~4}
	//GGO_GRAY4_BITMAP�F�P�V�F{0~16}
	//GGO_GRAY8_BITMAP�F�U�T�F{0~64}
	wchar_t	wch;	//�ϊ����̕���
	//Bmp* _next;	//���X�g�\���p���H������
};
#else
//LoadBmp�œǂݍ��񂾉摜�f�[�^���i�[����\���́B
struct Bmp {
	int width;		//�摜�̉������s�N�Z����
	int height;		//�摜�̏c�����s�N�Z����
	int colbit;		//�e�s�N�Z���̐F�r�b�g��
	int numpal;		//�p���b�g�̐F��
	int numpix;		//�摜�f�[�^�̃o�C�g��
	bool swapRB;	//24�r�b�g�ȏ�̉摜�ŁAR��B������ւ����Ă���ꍇ��'TRUE'�ɂȂ�
	COLORREF* pal;		//�p���b�g�i16�F�j�f�[�^�ւ̃|�C���^�������ꍇ��NULL
	char* pixel;	//�摜�f�[�^�ւ̃|�C���^
	//�ȉ���Biitmap�����p
	int aaLv;		//�~�����i�A���`�G�C���A�X���x���jWin32API��GetGlyphOutline�֐��Q��
	//GGO_BITMAP�F�Q�F�F�Q�l�̏ꍇ��{0x0|0xF}�˂O�������AF���s����
	//GGO_GRAY2_BITMAP�F�T�F
	//GGO_GRAY4_BITMAP�F�P�V�F
	//GGO_GRAY8_BITMAP�F�U�T�F
	wchar_t	wch;	//�ϊ����̕���
	//Bmp* _next;	//���X�g�\���p���H������
};
#endif // USED2D
//----------------------------------------------------------------
// ������
void	InitConio(int _width, int _height);	//�R���\�[�� I / O ������
void	InitConioEx(int _width, int _height, int _font_w, int _font_h);	//�R���\�[�� I / O �������i�g���Łj
void	InitConioEx(int _width, int _height, int _font_w, int _font_h, bool _init_wbuf = true);	//�R���\�[�� I / O �������i�g���Łj
void	InitConioEx(int _width, int _height, int _font_w, int _font_h, const wchar_t* _font_face_name, const COLORREF* _pal16, bool _init_wbuf = true);	//�R���\�[�� I / O �������i�g���Łj
void	EndConioEx(void);	//conioex�̏I������
int		InitDoubleBuffer(void);	//�_�u���o�b�t�@������
void	FlipScreen(void);		//�_�u���o�b�t�@���̕`��ʐؑ�
void	SetScreenFontSize(int width, int height);	//�t�H���g�T�C�Y�ύX
//----------------------------------------------------------------
//�E�B���h�E
#ifdef USED2D
LONG_PTR	FixWin(void);			//�E�B���h�E�T�C�Y�Œ�(�ύX�O�̏�Ԃ�Ԃ�)
LONG_PTR	FixWin(int _x, int _y);	//�E�B���h�E�ʒu���w�肵�āA�E�B���h�E�T�C�Y�Œ�(�ύX�O�̏�Ԃ�Ԃ�)
#else
LONG	FixWin(void);			//�E�B���h�E�T�C�Y�Œ�(�ύX�O�̏�Ԃ�Ԃ�)
LONG	FixWin(int _x, int _y);	//�E�B���h�E�ʒu���w�肵�āA�E�B���h�E�T�C�Y�Œ�(�ύX�O�̏�Ԃ�Ԃ�)
#endif // USED2D
HANDLE	GetCurrentHandle(void);	//���݃A�N�Z�X���̃o�b�t�@�̃n���h���B
void	SetCaption(const char* title);	//�R���\�[���E�B���h�E�̃^�C�g���o�[�Ƀe�L�X�g��ݒ�
void	SetCaptionF(const char* _format, ...);	//�R���\�[���E�B���h�E�̃^�C�g���o�[�ɏ����w�肵�ăe�L�X�g��ݒ�
int		GetCaption(char* title, int len);	//�R���\�[���E�B���h�E�̃^�C�g���o�[�ɕ\�������e�L�X�g���擾
RECT& GetConWinSize(RECT& _r);	//�N���C�A���g�̈�̃T�C�Y�ƃt�H���g�T�C�Y�̎擾
//----------------------------------------------------------------
//�J�[�\��
int		GetCursorX(void);				//���������̃J�[�\���ʒu���擾
int		GetCursorY(void);				//���������̃J�[�\���ʒu���擾
void	SetCursorPosition(int x, int y);	//�J�[�\���ʒu�̈ړ�
void	SetCursorType(int type);		//�J�[�\���^�C�v�ݒ�
POINT	GetCursorMousePos(POINT* _mp);	//�}�E�X���W(�����P��)�̎擾
//----------------------------------------------------------------
//������`��
void	PrintStringA(const char* _srcbuf, int _size);	//������̏o�́i�}���`�o�C�g�����p�j
void	PrintStringW(const wchar_t* _srcbuf, int _size);	//������̏o�́iUnicode�����p�j
#ifdef UNICODE
void	PrintString(const char* _srcbuf, int _size);	// ������̏o��(�}���`�o�C�g��Unicode�ϊ���)
#else
#define	PrintString	PrintStringA
#endif // UNICODE
//�����S��
void	SetHighVideoColor(void);		//�����F���P�x��
void	SetLowVideoColor(void);			//�����F��P�x��
void	SetNormalVideoColor(void);		//���蕶���F�ݒ�
//���������w��
void	SetTextBackColor(int color);	//�����w�i�F�ݒ�
void	SetConsoleTextColor(int color);	//�����F�ݒ�
void	SetTextAttribute(int attribute);	//�����F�w�i�F�����ݒ�
//�s����
void	ClearLine(void);				//�s���܂ŏ���
void	InsertLine(void);	//���ݍs�ɑ}��
void	DeleteLine(void);	//���ݍs�̍폜
//----------------------------------------------------------------
//�g��������`��
char* HanToZenA(const char* _src);	//���p������S�p�����ɕϊ�����i�}���`�o�C�g�Łj
wchar_t* HanToZenW(const wchar_t* _src);	//���p������S�p�����ɕϊ�����iUnicode�Łj
void	VPrintStringFA(bool _zenkaku, const char* _format, va_list _ap);	//�����w��t������`��i�������X�g�Łj�i�}���`�o�C�g�����p�j
void	VPrintStringFW(bool _zenkaku, const wchar_t* _format, va_list _ap);	//�����w��t������`��i�������X�g�Łj�iUnicode�����p�j
void	PrintStringFA(bool _zenkaku, const char* _format, ...);	//�����w��t������`��i�}���`�o�C�g�����p�j
void	PrintStringFW(bool _zenkaku, const wchar_t* _format, ...);	//�����w��t������`��iUnicode�����p�j
void	PosPrintStringFA(int _xp, int _yp, bool _zenkaku, const char* _format, ...);	//�ʒu�w�聕�����w��t������`��i�}���`�o�C�g�����p�j
void	PosPrintStringFW(int _xp, int _yp, bool _zenkaku, const wchar_t* _format, ...);	//�ʒu�w�聕�����w��t������`��iUnicode�����p�j
void	DrawStringFA(int _x, int _y, bool _zenkaku, const char* _format, ...);	//���W�w��i��ʍ������(0, 0)���W�j�{�����w��t������`��i�}���`�o�C�g�����p�j
void	DrawStringFW(int _x, int _y, bool _zenkaku, const wchar_t* _format, ...);	//���W�w��i��ʍ������(0, 0)���W�j�{�����w��t������`��iUnicode�����p�j
// �ȉ��́u#define�v�̓v���W�F�N�g��(�}���`�o�C�g�^Unicode)�Ɋւ�炸�A
// ��Ƀ}���`�o�C�g�Łi����'A'�j�̊֐������g���悤�ɂ���ׂ̒�`�B
// ��Unicode�ł��g���Ƃ��͖����I�ɖ���'W'�̊֐������Ăяo���B
#ifdef UNICODE
#define	HanToZen	HanToZenA
#define	PrintStringF	PrintStringFA
#define	PosPrintStringF	PosPrintStringFA
#define	DrawStringF		DrawStringFA
#else	// UNICODE
#define	HanToZen	HanToZenA
#define	PrintStringF	PrintStringFA
#define	PosPrintStringF	PosPrintStringFA
#define	DrawStringF		DrawStringFA
#endif // UNICODE
//----------------------------------------------------------------
// �p���b�g�֌W
// �p���b�g�Œ�f�[�^�iANSI 256�F�j
//	COLORREF��BMP�摜�̃p���b�g�`��
extern const COLORREF ANSI_PAL256_COLOR[NUM_ANSI_PAL];	//COLORREF
extern const RGBQUAD ANSI_PAL256_RGB[NUM_ANSI_PAL];	//RGBQUAD
COLORREF	RGBQtoCREF(RGBQUAD rgb);	//RGBQUAD: {Blue, Green, Red, 0}�^��COLORREF:0x00BBGGRR�^�ɕϊ�
RGBQUAD		CREFtoRGBQ(COLORREF ref);	//COLORREF:0x00BBGGRR�^��RGBQUAD : {Blue, Green, Red, 0}�^�ɕϊ�
COLORREF* ConvRGBQtoCREF(const RGBQUAD* _rgb, COLORREF* _cref);	//�p���b�g�ϊ��FRGBQ[16]->COLORREF[16]
RGBQUAD* ConvCREFtoRGBQ(const COLORREF* _cref, RGBQUAD* _rgb);	//�p���b�g�ϊ��FRGBQ[16]->COLORREF[16]
void	SetPalette(const COLORREF* _pal16, int _p1, int _p2);	//�P�U�F�p���b�g�ݒ�
void	SetPalette(const Bmp* _pBmp);	//Bmp����P�U�F�p���b�g�ݒ�
inline void ResetPalette(void) { SetPalette(ANSI_PAL256_COLOR, 0, 15); }

#ifdef USED2D
//----------------------------------------------------------------
//�t���[���o�b�t�@�摜�`��
void	ClearScreen(void);				//��ʁi�X�N���[���o�b�t�@�j����
void	ClearScreen(int _cc);			//��������p���b�g�ԍ����w�肷��
void	ClearScreen(int _red, int _green, int _blue);		//��������q�f�a�l���w�肷��
void	PrintFrameBuffer(void);
void	DrawPixel(int _x, int _y, unsigned char _c);	//�_��ł�
#ifdef UNICODE
#define	WriteText	WriteTextW
#else
#define	WriteText	WriteTextA
#endif // UNICODE
void	WriteTextA(int _xp, int _yp, const char* _text, double _scale, D2D1::ColorF _fgc, D2D1::ColorF _bgc, bool _new_line = false);
void	WriteTextW(int _xp, int _yp, const wchar_t* _text, double _scale, D2D1::ColorF _fgc, D2D1::ColorF _bgc, bool _new_line = false);
inline void	WriteTextA(int _xp, int _yp, const char* _text, double _scale) {
	WriteTextA(_xp, _yp, _text, _scale, D2D1::ColorF(1,1,1,1), D2D1::ColorF(0,0,0,0), false);
}
inline void	WriteTextW(int _xp, int _yp, const wchar_t* _text, double _scale) {
	WriteTextW(_xp, _yp, _text, _scale, D2D1::ColorF(1, 1, 1, 1), D2D1::ColorF(0, 0, 0, 0), false);
}

#else

//----------------------------------------------------------------
//�t���[���o�b�t�@�摜�`��
void	ClearScreen(void);				//��ʁi�X�N���[���o�b�t�@�j����
void	ClearScreenBuffer(const char _code = 0);	//�X�N���[���o�b�t�@�̏���
void	ClearFrameBuffer(char* buf);	//�u�t���[���o�b�t�@(16�F)�v�N���A
void	PrintFrameBuffer(const char* buf);	//�u�t���[���o�b�t�@(16�F)�v�̈ꊇ�]��
extern char* g_FrameBuffer4bit;
inline void	ClearFrameBuffer(void) { ClearFrameBuffer(g_FrameBuffer4bit); }	//�u�t���[���o�b�t�@(16�F)�v�N���A
inline void	PrintFrameBuffer(void) { PrintFrameBuffer(g_FrameBuffer4bit); }	//�u�t���[���o�b�t�@(16�F)�v�̈ꊇ�]��
void	DrawPixel(int _x, int _y, int _c);	//�_��ł�

#endif // !USED2D

void	DrawBmp(int _xp, int _yp, const Bmp* _bmp, bool _tr = true);	//Bmp�摜�̏o��
#define	BMP_HINV	(0b100)
#define	BMP_VINV	(0b010)
#define	BMP_HVINV	(BMP_HINV|BMP_VINV)
#define	BMP_ROT90	(0b001)
#define	BMP_ROT180	BMP_HVINV
#define	BMP_ROT270	(BMP_ROT90|BMP_HVINV)
void	DrawBmp(int _xp, int _yp, const Bmp* _bmp, int _hvinv, bool _tr = true);	//Bmp�摜�̏o��(���]�@�\)
//----------------------------------------------------------------
// �r�b�g�}�b�v(bmp)�t�@�C��
Bmp* LoadBmp(const char* _file_name, bool _palset_or_swap24RB = false);	//BMP�t�@�C���̓ǂݍ��݁B
void	DeleteBmp(Bmp** _p_bmp);	//�g���I�����BMP�摜�̍폜�B
void	Bmp24SwapRB(Bmp* _bmp);	//BRG�̃s�N�Z����RGB�ɕϊ�����B
//���Bmp�I�u�W�F�N�g�쐬�B
Bmp* CreateBmp(int _width, int _height, int _colbits, size_t _numpal, const COLORREF* const  _pal);
//�w�肵��Bmp�Ɠ����ݒ�ŋ��Bmp�����(�p���b�g������΃R�s�[�����)
inline Bmp* CreateBmp(const Bmp* _src) {
	return CreateBmp(_src->width, _src->height, _src->colbit, _src->numpal, _src->pal);
}
//�摜�̕����Ǎ��B
bool LoadDivBmp(const char* _path, int _x0, int _y0, size_t _xpix, size_t _ypix, size_t _xcount, size_t _ycount, Bmp** _pp_bmp);
//Bmp�̑S�s�N�Z��������������B�p���b�g�̂���摜�̏ꍇ�̓p���b�g�ԍ����w��A�t���J���[�̏ꍇ��0x00RRGGBB���w�肷��B
void ClearBmp(Bmp* _p, int _color = 0);
//����Bmp�̈ꕔ��ʂ�Bmp�ɃR�s�[����B
Bmp* CopyBmp(Bmp* _dest, int _dx, int _dy, const Bmp* _src, int _sx, int _sy, int _width, int _height, bool _tr);
//����Bmp�̈ꕔ��ʂ�Bmp�ɃR�s�[����B
inline Bmp* CopyBmp(Bmp* _dest, const Bmp* _src, int _sx, int _sy, int _width, int _height, bool _tr) {
	return CopyBmp(_dest, 0, 0, _src, _sx, _sy, _width, _height, _tr);
}
//��荞�ރT�C�Y��]����̃T�C�Y�ɍ��킹��
inline Bmp* CopyBmp(Bmp* _dest, const Bmp* _src, int _xp, int _yp, bool _tr) {
	return CopyBmp(_dest, 0, 0, _src, _xp, _yp, _dest->width, _dest->height, _tr);
}
//��荞�ރT�C�Y��]����̃T�C�Y�ɍ��킹��
inline Bmp* CopyBmp(Bmp* _dest, const Bmp* _src, bool _tr = false) {
	return CopyBmp(_dest, 0, 0, _src, 0, 0, _dest->width, _dest->height, _tr);
}
//Bmp�̈ꕔ��؂�o���A�V����Bmp�����
Bmp* CreateFromBmp(const Bmp* _src, int _xp, int _yp, int _width, int _height);
//�����T�C�Y��Bmp�����
inline Bmp* CreateFromBmp(const Bmp* _src) {
	return CreateFromBmp(_src, 0, 0, _src->width, _src->height);
}

//----------------------------------------------------------------
//�������Bmp�摜�̔z��Ƃ��Đ�������B
//	�r�b�g�}�b�v�����i������Bmp�摜�Ƃ��Đ����j
//	_ggo �ւ� {GGO_BITMAP,GGO_GRAY2_BITMAP,GGO_GRAY4_BITMAP,GGO_GRAY8_BITMAP}�̂ǂꂩ���w��
Bmp* CreateBmpChar(const TCHAR* _font_name, int _font_size, int _bold, int _ggo, const TCHAR* _text);
Bmp* CreateBmpString(const TCHAR* _font_name, int _font_size, int _bold, int _ggo, const TCHAR* _text);
inline Bmp* CreateBmpString(const TCHAR* _font_name, int _font_size, int _bold, const TCHAR* _text) {
	return CreateBmpString(_font_name, _font_size, _bold, GGO_BITMAP, _text);
}
Bmp* CreateBmpStringF(const TCHAR* _font_name, int _font_size, int _bold, int _ggo, bool _zenkaku, const TCHAR* _format, ...);
Bmp* CreateBmpStringF(const TCHAR* _font_name, int _font_size, int _bold, bool _zenkaku, const TCHAR* _format, ...);
//----------------------------------------------------------------
// �L�[���͊֌W
void	ResetKeyMap(void);	//�L�[��񃊃Z�b�g
int		InputKeyMouse(int port);	//�L�[�{�[�h�E�}�E�X����
//----------------------------------------------------------------
// �g���L�[����
SHORT	GetKey(int _vk);	//�P��L�[�̓��́B�����̃R���\�[���E�B���h�E�Ƀt�H�[�J�X���������Ă��鎞�������͂���B
SHORT	WaitKey(int _vk);	//�L�[���͂�҂B�����̃R���\�[���E�B���h�E�Ƀt�H�[�J�X���������Ă��鎞�������͂���B
int		GetKeyEx(int _vk, int _chktype);	//�S�ẴL�[�̓��͂Ɣ���
void	GetKeyAll(void);	//�S�ẴL�[�̓��́B�����̃R���\�[���E�B���h�E�Ƀt�H�[�J�X���������Ă��鎞�������͂���B
int		ChkKeyEdge(int _vk);	//Edge�L�[���͔���FGetKeyAll()�œ��͂����L�[���ɂ���Edge�L�[�i�g���K�[�L�[�j���͔��肷��
int		ChkKeyPress(int _vk);	//Press�L�[���͔���FGetKeyAll()�œ��͂����L�[���ɂ���Press�L�[�i�����L�[�j���͔��肷��
//----------------------------------------------------------------
// �W���C�p�b�h����
int		InputJoystick(int port);	//�W���C�p�b�h����
#if FALSE
int		InputJoystickX(int port);					// �Q�[���p�b�h����(XInput�Ή�)
#else	//FALSE
int		InputJoystickX(int id, int port);			// �Q�[���p�b�h����(XInput�Ή�)
#endif // FALSE
//----------------------------------------------------------------
// �T�E���h�Đ�
int* MciOpenSound(const char* path);	//�T�E���h �t�@�C�����J��
void	MciCloseSound(int* sound_id);	//�T�E���h �t�@�C�������
void	MciPlaySound(int* sound_id, int repeat);	//�T�E���h���Đ�����
void	MciStopSound(int* sound_id);	//�T�E���h�Đ����~����
int		MciCheckSound(int* sound_id);	//�T�E���h�Đ���Ԃ̎擾
void	MciUpdateSound(int* sound_id);	//���[�v�Đ��̋����X�V
void	MciSetVolume(int* sound_id, int percent);	//�Đ����ʂ�ݒ肷��

#ifdef USED2D
//----------------------------------------------------------------
// 24�r�b�g�F�摜
//void	PrintImage(const RGBQUAD* _buf);	//�t���J���[(24�r�b�g/Pixel)�摜�̕\���i��ʂ̃T�C�Y�Ɠ����摜���o�͂���j
//void	ClearFrameBufferFull(RGBQUAD* buf);	//�t���[���o�b�t�@(�t���J���[(24�r�b�g/Pixel))�N���A
//extern RGBQUAD* g_FrameBuffer32bitD2D;	//�t���J���[�p�t���[���o�b�t�@
//inline void	PrintImage(void) { PrintImage(g_FrameBuffer32bitD2D); };	//�t���J���[(24�r�b�g/Pixel)�摜�̕\���i��ʂ̃T�C�Y�Ɠ����摜���o�͂���j
//inline void	ClearFrameBufferFull(void) { ClearFrameBufferFull(g_FrameBuffer32bitD2D); }	//�t���[���o�b�t�@(�t���J���[(24�r�b�g/Pixel))�N���A
//void	PrintImage(void);
//void	ClearFrameBufferFull(void);

#else

//----------------------------------------------------------------
// 24�r�b�g�F�摜
extern char* g_FrameBufferFull;
void	PrintImage(const char* _buf);	//�t���J���[(24�r�b�g/Pixel)�摜�̕\���i��ʂ̃T�C�Y�Ɠ����摜���o�͂���j
void	ClearFrameBufferFull(char* buf);	//�t���[���o�b�t�@(�t���J���[(24�r�b�g/Pixel))�N���A
inline void	PrintImage(void) { PrintImage(g_FrameBufferFull); };	//�t���J���[(24�r�b�g/Pixel)�摜�̕\���i��ʂ̃T�C�Y�Ɠ����摜���o�͂���j
inline void	ClearFrameBufferFull(void) { ClearFrameBufferFull(g_FrameBufferFull); }	//�t���[���o�b�t�@(�t���J���[(24�r�b�g/Pixel))�N���A
#endif	//USED2D
#ifndef USED2D
////----------------------------------------------------------------
////�@256�F�摜�y�H�����z�d�l���ς��\�����傫��
//void	InitPaletteANSI256(void);	//�Q�T�U�p���b�g������
//void	SetPixel256(int _x, int _y, int _palidx);	//�P�s�N�Z��/�Q�T�U�F�`��
//void	SetPixelBuffer256(int _x, int _y, int _palidx);
//void	DrawPixelBuffer256(int _xp, int _yp, int _w, int _h, BYTE* _buf);
#endif // !USED2D

#define	CONIOEX_DDA_SHAPE
#ifdef CONIOEX_DDA_SHAPE
//================================================================
//	�}�`�̕`��iDDA�ŕ`��j
//================================================================
/*
* �~�̍\����
*/
struct CIRCLE {
	int cx, cy;	//���S�_
	size_t r;		//���a
};
/*
* �����̍\����
*/
struct LINE {
	int x1, y1;	//�n�_
	int x2, y2;	//�I�_
};
/*
* @brief	�����̈ړ�
* @param	LINE* _pl	�����f�[�^�̃|�C���^
* @param	int _dx		�w�����ړ���
* @param	int _dy		�x�����ړ���
*/
inline void MoveLine(LINE* _pl, int _dx, int _dy) {
	_pl->x1 += _dx;
	_pl->y1 += _dy;
	_pl->x2 += _dx;
	_pl->y2 += _dy;
}
/*
* @brief	�����̈ړ�
* @param	LINE* _pl	�����f�[�^�̃|�C���^
* @param	int _px		�ړ���w���W�ileft�j
* @param	int _py		�ړ���x���W�itop�j
*/
inline void SetLine(LINE* _pl, int _px, int _py) {
	_pl->x2 -= (_pl->x1 - 1);	//��
	_pl->y2 -= (_pl->y1 - 1);	//����
	_pl->x1 = _px;
	_pl->y1 = _py;
	_pl->x2 += _px;
	_pl->y2 += _py;
}
/*
* @brief	��`�̑��Έړ�
* @param	RECT* _pr	��`�f�[�^�̃|�C���^
* @param	int _dx		�w�����ړ���
* @param	int _dy		�x�����ړ���
*/
inline void MoveRect(RECT* _pr, int _dx, int _dy) {
	_pr->left += _dx;
	_pr->top += _dy;
	_pr->right += _dx;
	_pr->bottom += _dy;
}
/*
* @brief	��`�̈ړ�
* @param	RECT* pr	��`�f�[�^�̃|�C���^
* @param	int _px		�ړ���w���W�ileft�j
* @param	int _py		�ړ���x���W�itop�j
*/
inline void SetRect(RECT* _pr, int _px, int _py) {
	_pr->right -= _pr->left;	//��-1
	_pr->bottom -= _pr->top;	//����-1
	_pr->left = _px;
	_pr->top = _py;
	_pr->right += _px;
	_pr->bottom += _py;
}
//=== ������ ===
/*
* @brief	��������`�悷��
* @param	int _x1,_y1	�J�n���W
* @param	int _x2		�I���w���W�i�x���W��_y1�Ɠ����j
* @param	int _cc		�J���[�R�[�h�i�p���b�g�̔ԍ��j
*/
void DrawLineH(int _x1, int _y1, int _x2, int _cc);
/*
* @brief	��������`�悷��
* @param	LINE _ln	�����\���́ix1,y1,x2���g���ĕ`��j
* @param	int _cc		�J���[�R�[�h�i�p���b�g�̔ԍ��j
*/
inline void DrawLineH(const LINE* _ln, int _cc) {
	DrawLineH(_ln->x1, _ln->y1, _ln->x2, _cc);
}
/*
* @brief	��������`�悷��
* @param	int _x1,_y1	�J�n���W
* @param	int _x2		�I���w���W�i�x���W��_y1�Ɠ����j
* @param	RGBQUAD _rgb	�`��F��RGB�l
*/
void DrawLineH(int _x1, int _y1, int _x2, RGBQUAD _rgb);
/*
* @brief	��������`�悷��
* @param	LINE _ln	�����\���́ix1,y1,x2���g���ĕ`��j
* @param	RGBQUAD _rgb	�`��F��RGB�l
*/
inline void DrawLineH(const LINE* _ln, RGBQUAD _rgb) {
	DrawLineH(_ln->x1, _ln->y1, _ln->x2, _rgb);
}
//=== ������ ===
/*
* @brief	��������`�悷��
* @param	int _x1,_y1	�J�n���W
* @param	int _y2		�I���x���W�i�w���W��_x1�Ɠ����j
* @param	int _cc		�J���[�R�[�h�i�p���b�g�̔ԍ��j
*/
void DrawLineV(int _x1, int _y1, int _y2, int _cc);
/*
* @brief	��������`�悷��
* @param	LINE _ln	�����\���́ix1,y1,y2���g���ĕ`��j
* @param	int _cc		�J���[�R�[�h�i�p���b�g�̔ԍ��j
*/
inline void DrawLineV(const LINE* _ln, int _cc) {
	DrawLineV(_ln->x1, _ln->y1, _ln->y2, _cc);
}
/*
* @brief	��������`�悷��
* @param	int _x1,_y1	�J�n���W
* @param	int _y2		�I���x���W�i�w���W��_x1�Ɠ����j
* @param	RGBQUAD _rgb	�`��F��RGB�l
*/
void DrawLineV(int _x1, int _y1, int _y2, RGBQUAD _rgb);
/*
* @brief	��������`�悷��
* @param	LINE _ln	�����\���́ix1,y1,y2���g���ĕ`��j
* @param	RGBQUAD _rgb	�`��F��RGB�l
*/
inline void DrawLineV(const LINE* _ln, RGBQUAD _rgb) {
	DrawLineV(_ln->x1, _ln->y1, _ln->y2, _rgb);
}
//=== ���� ===
/*
* @brief	�����̕`��
* @param	int _x1,_y2	�J�n���W
* @param	int _x2,_y2	�I�����W
* @param	int _cc		�J���[�R�[�h�i�p���b�g�̔ԍ��j
*/
void DrawLine(int _x1, int _y1, int _x2, int _y2, int _cc);
/*
* @brief	�����̕`��
* @param	LINE _ln	�����\����
* @param	int _cc		�J���[�R�[�h�i�p���b�g�̔ԍ��j
*/
inline void DrawLine(const LINE* _ln, int _cc) {
	DrawLine(_ln->x1, _ln->y1, _ln->x2, _ln->y2, _cc);
}
/*
* @brief	�����̕`��
* @param	int _x1,_y2	�J�n���W
* @param	int _x2,_y2	�I�����W
* @param	RGBQUAD _rgb	�`��F��RGB�l
*/
void DrawLine(int _x1, int _y1, int _x2, int _y2, RGBQUAD _rgb);
/*
* @brief	�����̕`��
* @param	LINE _ln	�����\����
* @param	RGBQUAD _rgb	�`��F��RGB�l
*/
inline void DrawLine(const LINE* _ln, RGBQUAD _rgb) {
	DrawLine(_ln->x1, _ln->y1, _ln->x2, _ln->y2, _rgb);
}
//=== ��` ===
/*
* @brief	��`��`�悷��
* @param	int _x1,_y1	������W
* @param	int _x2,_y2	�E�����W
* @param	int _cc		�J���[�R�[�h�i�p���b�g�̔ԍ��j
* @param	bool _fill	�h��Ԃ�(true:����/false:���Ȃ�)
*/
void DrawRect(int _x1, int _y1, int _x2, int _y2, int _cc, bool _fill = false);
/*
* @brief	��`��`�悷��
* @param	RECT _rc	��`�̍��W
* @param	int _cc		�J���[�R�[�h�i�p���b�g�̔ԍ��j
* @param	bool _fill	�h��Ԃ�(true:����/false:���Ȃ�)
*/
inline void DrawRect(const RECT* _rc, int _cc, bool _fill = false) {
	DrawRect(_rc->left, _rc->top, _rc->right, _rc->bottom, _cc, _fill);
}
/*
* @brief	��`��`�悷��
* @param	POINT _rc	��`�̍�����W
* @param	SIZE _sz	��`�̕��ƍ���
* @param	int _cc		�J���[�R�[�h�i�p���b�g�̔ԍ��j
* @param	bool _fill	�h��Ԃ�(true:����/false:���Ȃ�)
*/
inline void DrawRect(const POINT* _p, const SIZE* _sz, int _cc, bool _fill = false) {
	DrawRect(_p->x, _p->y, _p->x + (_sz->cx - 1), _p->y + (_sz->cy - 1), _cc, _fill);
}
/*
* @brief	��`��`�悷��
* @param	int _x1,_y1	������W
* @param	int _x2,_y2	�E�����W
* @param	RGBQUAD _rgb	�`��F��RGB�l
* @param	bool _fill	�h��Ԃ�(true:����/false:���Ȃ�)
*/
void DrawRect(int _x1, int _y1, int _x2, int _y2, RGBQUAD _rgb, bool _fill = false);
/*
* @brief	��`��`�悷��
* @param	RECT _rc	��`�̍��W
* @param	RGBQUAD _rgb	�`��F��RGB�l
* @param	bool _fill	�h��Ԃ�(true:����/false:���Ȃ�)
*/
inline void DrawRect(const RECT* _rc, RGBQUAD _rgb, bool _fill = false) {
	DrawRect(_rc->left, _rc->top, _rc->right, _rc->bottom, _rgb, _fill);
}
/*
* @brief	��`��`�悷��
* @param	POINT _rc	��`�̍�����W
* @param	SIZE _sz	��`�̕��ƍ���
* @param	RGBQUAD _rgb	�`��F��RGB�l
* @param	bool _fill	�h��Ԃ�(true:����/false:���Ȃ�)
*/
inline void DrawRect(const POINT* _p, const SIZE* _sz, RGBQUAD _rgb, bool _fill = false) {
	DrawRect(_p->x, _p->y, _p->x + (_sz->cx - 1), _p->y + (_sz->cy - 1), _rgb, _fill);
}
//=== �~ ===
/*
* @brief	�~��`�悷��
* @param	int _cx,_cy	���S���W
* @param	int _r		���a
* @param	int _cc		�J���[�R�[�h�i�p���b�g�̔ԍ��j
* @param	bool _fill	�h��Ԃ�(true:����/false:���Ȃ�)
*/
void DrawCircle(int _cx, int _cy, int _r, int _cc, bool _fill = false);
/*
* @brief	�~��`�悷��
* @param	CIRCLE _pc	�~�̍��W�Ɣ��a
* @param	int _cc		�J���[�R�[�h�i�p���b�g�̔ԍ��j
* @param	bool _fill	�h��Ԃ�(true:����/false:���Ȃ�)
*/
inline void DrawCircle(const CIRCLE* _pr, int _cc, bool _fill = false) {
	DrawCircle(_pr->cx, _pr->cy, _pr->r, _cc, _fill);
}
/*
* @brief	�~��`�悷��
* @param	int _cx,_cy	���S���W
* @param	int _r		���a
* @param	RGBQUAD _rgb	�`��F��RGB�l
* @param	bool _fill	�h��Ԃ�(true:����/false:���Ȃ�)
*/
void DrawCircle(int _cx, int _cy, int _r, RGBQUAD _rgb, bool _fill = false);
/*
* @brief	�~��`�悷��
* @param	CIRCLE _pc	�~�̍��W�Ɣ��a
* @param	RGBQUAD _rgb	�`��F��RGB�l
* @param	bool _fill	�h��Ԃ�(true:����/false:���Ȃ�)
*/
inline void DrawCircle(const CIRCLE* _pr, RGBQUAD _rgb, bool _fill = false) {
	DrawCircle(_pr->cx, _pr->cy, _pr->r, _rgb, _fill);
}

#endif // !CONIOEX_DDA_SHAPE

//----------------------------------------------------------------
//	�t���[���������v���p�֐�
void InitFrameSync(double _FPS = 60.0);
void FrameSync(void);

/**
* @copyright (c) 2018-2019 HAL Osaka College of Technology & Design (Ihara, H.)
*/
