/*
 @version	3.0
*/
#include "conioex.h"
#include <vector>

#define CONSOLE_INPUT_MODE	(ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT | ENABLE_PROCESSED_INPUT)
//�R���\�[���o�͂�ANSI-256-color��t���J���[���������߂�'ENABLE_VIRTUAL_TERMINAL_PROCESSING'�ǉ�
#define CONSOLE_OUTPUT_MODE	(ENABLE_PROCESSED_OUTPUT | ENABLE_LVB_GRID_WORLDWIDE | ENABLE_VIRTUAL_TERMINAL_PROCESSING)

//----------------------------------------------------------------
// �r�b�g�}�b�v�t�@�C������p
#define	NUM_BMP1_PALETTE	(2)		//1�r�b�g�J���[�摜�̃p���b�g�͂Q�F
#define	NUM_BMP4_PALETTE	(16)	//4�r�b�g�J���[�摜�̃p���b�g�͂P�U�F
#define	NUM_BMP8_PALETTE	(256)	//8�r�b�g�J���[�摜�̃p���b�g�͂Q�T�U�F

//MCI�T�E���h�p�\����
typedef struct {
	int				device_type;
	MCIDEVICEID		device_id;
	char			path[MAX_PATH];
	int				repeat;
} MciSoundInfo;

static HANDLE	g_OrgOutputHandle = NULL;	//InitConio���O�̃n���h��
static DWORD	g_OrgOutputHandleMode = 0;
static HANDLE	g_OrgInputHandle;
static DWORD	g_OrgInputHandleMode = 0;
static CONSOLE_CURSOR_INFO	g_OrgCursorInfo = { 0 };	//�I���W�i���̃J�[�\�����
#ifdef USED2D
static LONG_PTR	g_OrgWindowStylePtr = 0;
static HANDLE	g_DisplayHandleD2D = NULL;	//�f�B�X�v���C�n���h���i�_�u���o�b�t�@�j
static HANDLE	g_InputHandleD2D = NULL;	//���͗p�n���h��
#else
static LONG		g_OrgWindowStyle = 0;
static HANDLE	g_DisplayHandle[2] = { NULL,NULL };	//�f�B�X�v���C�n���h���i�_�u���o�b�t�@�j
static HANDLE	g_InputHandle = NULL;	//���͗p�n���h��
static int		g_SwapFlg = 0;	//�_�u���o�b�t�@�L�����̃f�B�X�v���C�n���h�����֗p�t���O
#endif // USED2D

static DWORD	g_ConioKeyMap[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
static COORD	g_ConioMousePosition = { 0, 0 };
static COORD	g_ScreenBufferSize = { 0,0 };
static WORD		g_ScreenBufferAttribute = LIGHTGRAY;

static CONSOLE_SCREEN_BUFFER_INFOEX g_ScreenBufferInfoEx = { sizeof(CONSOLE_SCREEN_BUFFER_INFOEX) };
static CONSOLE_SCREEN_BUFFER_INFOEX g_OrgScreenBufferInfoEx = { sizeof(CONSOLE_SCREEN_BUFFER_INFOEX) };
static CONSOLE_FONT_INFOEX g_FontSizeEx = { sizeof(CONSOLE_FONT_INFOEX) };
static CONSOLE_FONT_INFOEX g_OrgFontSizeEx = { sizeof(CONSOLE_FONT_INFOEX) };

static SMALL_RECT g_WindowSize;

//�R���\�[���E�B���h�E�̃n���h���ۑ��p�ϐ�
static HWND g_hConWnd = NULL;

//�L�[���̓o�b�t�@
static int g_KeyPress[NUM_KEYS] = {};	//�v���X
static int g_KeyEdge[NUM_KEYS] = {};	//�G�b�W
static int g_KeyLast[NUM_KEYS] = {};		//TEMP

#ifndef USED2D
//�p���b�g�e�[�u��
static COLORREF	g_ConsoleColorTable[NUM_PALETTE] = { 0 };	//�R���\�[���̃p���b�g
static COLORREF	g_OrgColorTable[NUM_PALETTE] = { 0 };	//Conioex���N�������Ƃ��̃p���b�g

//�X�N���[���o�b�t�@
//static CHAR_INFO* g_ScreenBuffer4bit = NULL;	//�X�N���[���o�b�t�@
static WORD* g_ScreenBuffer4bit = NULL;	//�X�N���[���o�b�t�@
//�t���[���o�b�t�@
char* g_FrameBuffer4bit = NULL;	//�P�U�F�摜�p�t���[���o�b�t�@
#endif // !USED2D
//�X�N���[���o�b�t�@(16�F)�̐���
static HANDLE create_screen_buffer(CONSOLE_SCREEN_BUFFER_INFOEX* pCsbix, CONSOLE_FONT_INFOEX* pCfix);
#ifndef USED2D
//RGB�l�i�����R�����j"�O�O�O"�`"�Q�T�T"�̃e�[�u��(�Q�Sbit�J���[�摜�p)
static char CharRGBconvTBL[3][256] = {
	{'0','0','0'},
};

//�Q�S�r�b�g�t���J���[�摜�p
#define	PIXEL24ESCSIZE	(sizeof(pixel24bitEsc))	//�t���J���[�`�掞�̂Q����������ɕK�v�ȃT�C�Y�i�F�w��p�G�X�P�[�v�V�[�P���X�̃o�C�g���j
static const char pixel24bitEsc[] = "\033[48;2;000;000;000m ";
char* g_FrameBufferFull = NULL;	//�Q�Sbit�J���[�摜�p
static char* g_ScreenBufferFull = NULL;	//�Q�Sbit�J���[�摜�p
static int g_ScreenBufferLineStride = 0;	//�Q�S�r�b�g�摜�p�o�b�t�@�̂P�s���̃o�C�g���B
static void init_24bit_color_image(void);	//�Q�S�r�b�g�t���J���[�摜�p�̏�����
//�Q�T�U�F�p���b�g�摜�p
static const char pixel256Esc[] = "\x1b[48;5;000m ";	//�Q�T�U�p���b�g�ԍ��w��p�G�X�P�[�v�V�[�P���X�FESC[48;5;<�p���b�g�ԍ��F10�i��3�P�^>m
static const char palette256Esc[] = "\x1b]4;000;rgb:00/00/00\x1b\\";	//�Q�T�U�p���b�g�pRGB�ݒ�G�X�P�[�v�V�[�P���X�FESC[4;<�p���b�g�ԍ��F10�i��3�P�^;rgb:<RR>/<GG>/<BB>ESC\> ��RGB�l�͊e16�i2�P�^
static BYTE* g_FrameBuffer256 = NULL;	//�W�r�b�g/�s�N�Z���̃o�b�t�@
static BYTE* g_ScreenBuffer256 = NULL;	//�Q�T�U�p���b�g�ԍ��w��G�X�P�[�v�V�[�P���X�����񂾃o�b�t�@
static void init_256color_image(void);	//�Q�T�U�F�p���b�g�摜�p�̏�����
static void set_palette256(HANDLE _hCon, const COLORREF* _p256, int _num_pal);	//�Q�T�U�F�p���b�g�ݒ�
#endif // !USED2D


static RECT g_ConWinSize = {};	//�R���\�[���̃N���C�A���g�̈�̃T�C�Y�ƁA�t�H���g�T�C�Y�B{left=w,top=h,right=fw,bottom=fh}

#ifdef USED2D
//================================================================
//	Direct2D/DirectWrite
//================================================================
ID2D1HwndRenderTarget* g_pRenderTarget = NULL;	//�E�B���h�E�ɕ`�悷��ׂ̃����_�[�^�[�Q�b�g
ID2D1Factory* g_pD2DFactory = NULL;		//D2D�t�@�N�g���[
IDWriteFactory* g_pDWFactory = NULL;	//�e�L�X�g�o�͗pDirectWrite�t�@�N�g���[

std::vector<ID2D1Bitmap*>	g_pBmpList;		//�摜�p
std::vector<ID2D1Bitmap*>	g_pTextBmpList;	//�e�L�X�g�p

//char* g_FrameBuffer4bitD2D = NULL;		//�P�U�F(4bit)�摜�p�t���[���o�b�t�@
RGBQUAD* g_FrameBuffer32bitD2D = NULL;	//�t���J���[�p�t���[���o�b�t�@
//RGBQUAD* g_pMask = NULL;
COORD	g_CursorPosD2D = { 0,0 };		//�����\���J�n�ʒu�i�J�[�\���ʒu�j

//�p���b�g�e�[�u��
//static COLORREF	g_ConsoleColorTableD2D[NUM_D2D_PAL] = { 0 };	//�R���\�[���̃p���b�g
static COLORREF	g_OrgColorTableD2D[NUM_D2D_PAL] = { 0 };	//Conioex���N�������Ƃ��̃p���b�g
RGBQUAD	g_PaletteD2D[NUM_ANSI_PAL] = {};	//256�F�p���b�g�FCOLORREF:0x00BBGGRR->{R8,G8,B8,X}/RGBQUAD:{B8,G8,R8,A8}->0xAARRGGBB

bool g_PrintStringCompatibleMode = false;	//true=ConsoleAPI�̎��̂P�F�Q�̕����T�C�Y

//prototype
bool InitD2D(int _w, int _h);
void EndD2D(void);
//================================================================
//	D2D������
//================================================================
bool InitD2D(int _window_width, int _window_height)
{
	HRESULT hr;	//
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pD2DFactory);
	_ASSERT(hr == S_OK);
#if false
	RECT cr;
	GetClientRect(g_hConWnd, &cr);	//�N���C�A���g�̈��{0,0,��,����}�擾
	_window_width = cr.right;
	_window_height = cr.bottom;
	hr = g_pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(g_hConWnd, D2D1_SIZE_U{ (UINT32)_window_width, (UINT32)_window_height }), &g_pRenderTarget);
	_ASSERT(hr == S_OK);
#else
	/*
	* �����_�[�^�[�Q�b�g�̎��
	* ID2D1BitmapRenderTarget ------> CreateCompatibleRenderTarget ���\�b�h�ɂ���č쐬���ꂽ���ԃe�N�X�`���Ƀ����_�����O���܂��B
	* ID2D1DCRenderTarget ----------> GDI �f�o�C�X �R���e�L�X�g�ɑ΂��ĕ`��R�}���h�𔭍s���܂��B
	* ID2D1GdiInteropRenderTarget --> GDI �`��R�}���h���󂯓���邱�Ƃ��ł���f�o�C�X�R���e�L�X�g�ւ̃A�N�Z�X��񋟂��܂��B
	* ID2D1HwndRenderTarget --------> �`�施�߂��E�B���h�E�Ƀ����_�����O���܂��B
	*/
	//�E�B���h�E�ւ̕`��Ȃ̂� ID2D1HwndRenderTarget ���g�p����B
	//�T�C�Y�̓E�B���h�E�Ɠ����T�C�Y�ɂ��邱�ƁB�T�C�Y���Ⴄ�ƁA�E�B���h�E�ɍ��킹�Ċg��E�k����������B
	/////D2D1_SIZE_U renderTargetSize = { (UINT32)g_WindowSize.Right, (UINT32)g_WindowSize.Bottom };
	D2D1_SIZE_U renderTargetSize = { _window_width, _window_height };
	D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties;
	D2D1_HWND_RENDER_TARGET_PROPERTIES hwndRenderTargetProperties;
	D2D1_PIXEL_FORMAT pixelFormat;
	pixelFormat.format = DXGI_FORMAT_UNKNOWN;	//����̌`��
	pixelFormat.alphaMode = D2D1_ALPHA_MODE_UNKNOWN;
	//pixelFormat.format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;

	renderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;	//�n�[�h�E�F�A�ɂ��`��ƃ\�t�g�E�F�A�ɂ��`��̂ǂ���𗘗p���邩
	renderTargetProperties.pixelFormat = pixelFormat;				//�s�N�Z���`���ƃA���t�@���[�h
	renderTargetProperties.dpiX = 0;								//���ꂼ�ꐅ�������Ɛ��������� DPI �iDonts per Inch�A�s�N�Z�����x�j���w�肵�܂��B
	renderTargetProperties.dpiY = 0;								//����� DPI ���g�p����ɂ� 0 ���w�肵�܂��B
	renderTargetProperties.usage = D2D1_RENDER_TARGET_USAGE_NONE;	//�����_�[�^�[�Q�b�g�̃����[�g������ GDI �Ƃ̌݊���
	renderTargetProperties.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;	//�n�[�h�E�F�A�ɂ��`��ɕK�v�� Direct3D �̍ŏ����̋@�\���x��

	RECT cr;
	GetClientRect(g_hConWnd, &cr);	//�N���C�A���g�̈��{0,0,��,����}�擾
	renderTargetSize.width = cr.right;
	renderTargetSize.height = cr.bottom;
	hwndRenderTargetProperties.hwnd = g_hConWnd;	//�^�[�Q�b�g�ƂȂ�E�B���h�E�̃n���h��
	hwndRenderTargetProperties.pixelSize = renderTargetSize;	//�E�B���h�E�̃N���C�A���g�̈�̃T�C�Y
	hwndRenderTargetProperties.presentOptions = D2D1_PRESENT_OPTIONS_IMMEDIATELY;	//�񓯊��i�t���[�����Ԃ܂ő҂��Ȃ��j
	//hwndRenderTargetProperties.presentOptions = D2D1_PRESENT_OPTIONS_NONE;	//�t���[������
	//�����_�[�^�[�Q�b�g�擾
	hr = g_pD2DFactory->CreateHwndRenderTarget(renderTargetProperties, hwndRenderTargetProperties, &g_pRenderTarget);
#endif // false

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&g_pDWFactory));
	_ASSERT(hr == S_OK);
	//g_FrameBuffer4bitD2D = (char*)calloc(g_ScreenBufferSize.X * g_ScreenBufferSize.Y, sizeof(char));
	g_FrameBuffer32bitD2D = (RGBQUAD*)calloc(g_ScreenBufferSize.X * g_ScreenBufferSize.Y, sizeof(RGBQUAD));
	//g_pMask = (RGBQUAD*)calloc(g_ScreenBufferSize.X * g_ScreenBufferSize.Y, sizeof(RGBQUAD));
	//�f�t�H���g�Q�T�U�p���b�g��ANSI256�F�ŏ�����
	//memcpy_s(g_PaletteD2D, sizeof(g_PaletteD2D), ANSI_PAL256_RGB, sizeof(ANSI_PAL256_RGB));
	//�ŏ��̂P�U�F�͋N�����̃p���b�g����荞��ł���̂ŁA�P�V�F�ȍ~�̐F����荞�ށB
	for (int n = 16; n < NUM_ANSI_PAL; n++) {
		g_PaletteD2D[n] = ANSI_PAL256_RGB[n];
	}
	return true;
}	//InitD2D
//==================================================================
// D2D�I��
//==================================================================
void EndD2D(void)
{
	//free(g_pMask);
	if (g_FrameBuffer32bitD2D != NULL) {
		free(g_FrameBuffer32bitD2D);
		g_FrameBuffer32bitD2D = NULL;
	}

	//if (g_FrameBuffer4bitD2D != NULL) {
	//	free(g_FrameBuffer4bitD2D);
	//	g_FrameBuffer4bitD2D = NULL;
	//}

	for (ID2D1Bitmap* pbmp : g_pBmpList) {
		pbmp->Release();
	}
	g_pBmpList.clear();

	for (ID2D1Bitmap* pbmp : g_pTextBmpList) {
		pbmp->Release();
	}
	g_pTextBmpList.clear();

	// IDWriteFactory�̔j��
	if (NULL != g_pDWFactory) {
		g_pDWFactory->Release();
		g_pDWFactory = NULL;
	}

	// ID2D1HwndRenderTarget�̔j��
	if (NULL != g_pRenderTarget) {
		g_pRenderTarget->Release();
		g_pRenderTarget = NULL;
	}

	// ID2D1Factory�̔j��
	if (NULL != g_pD2DFactory) {
		g_pD2DFactory->Release();
		g_pD2DFactory = NULL;
	}
}	//EndD2D

//================================================================
/*
* ���݂�32�r�b�g�t���[���o�b�t�@(g_FrameBuffer32bitD2D)���r�b�g�}�b�v(ID2D1Bitmap)�ɓ]�����āA�r�b�g�}�b�v���X�g�� g_BmpList �ǉ�����B
* DXGI_FORMAT_B8G8R8A8_UNORM
* DXGI_FORMAT_R8G8B8A8_UNORM
* DXGI_FORMAT_A8_UNORM
*/
static void push_screen_buffer(void)
{
	HRESULT hr = ~S_OK;
	D2D1_SIZE_U siz = { g_ScreenBufferSize.X, g_ScreenBufferSize.Y };
	ID2D1Bitmap* pD2D1_Bmp;
	hr = g_pRenderTarget->CreateBitmap(
		siz,
		//D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),	//�s����
		//D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),	//�s����
		//D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_STRAIGHT)),	//�s����
		//D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_STRAIGHT)),	//�s����
		D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)),	//�s����
		//D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_IGNORE)),	//NG
		&pD2D1_Bmp
	);
	_ASSERT(hr == S_OK);
	if (pD2D1_Bmp == NULL) {
		return;	//�r�b�g�}�b�v�쐬���s
	}
	D2D1_RECT_U	ru = { 0,0,siz.width,siz.height };
	hr = pD2D1_Bmp->CopyFromMemory(&ru, reinterpret_cast<const void*>(g_FrameBuffer32bitD2D), g_ScreenBufferSize.X * 4);
	_ASSERT(hr == S_OK);
	//�������� �`�悵���r�b�g�}�b�v�����X�g�ɒǉ� ������
	g_pBmpList.push_back(pD2D1_Bmp);
	//�������� pD2D1_Bmp �� Release �s�v�I���������@�����@�`�掞�� Release ���Ă��� ��������
	return;
}	//push_screen_buffer

//================================================================
//	�C���f�b�N�X�J���[�摜�����ݐݒ肳��Ă���Q�T�U�F�p���b�g��32�r�b�g�o�b�t�@�ɕ`�悷��
/*
* ������
*	�`�掞�ɂP�s�N�Z���P�ʂőS�ĂR�Q�r�b�g�o�b�t�@�ɕ`������ł���̂ł��̏����͕s�v�B
*	�Ƃ������A���̂Ƃ���s�N�Z���P�ʂŏ������Ă��d���͖����̂ŁA
* 	�S�r�b�g�o�b�t�@���s�v�����m��Ȃ��E�E�E
*	����ɁAPrintImag()�ƕ��p����ƁA32�r�b�g�o�b�t�@���㏑�����Ă��܂��̂ł���₱�����Ȃ邵�E�E�E
*	�܂��A4�r�b�g�摜����̕ϊ��p�ɐ�p��32�r�b�g�o�b�t�@�����Ƃ������ǁA����ł̓p���b�g����������Ȃ��̂ŁA�������₱�����E�E�E
*	�������A�p���b�g�C���f�b�N�X�摜�̎d�g�݂�׋�����ׂɂ͗ǂ��̂�������Ȃ��̂Ō����̗]�n�͑傫�����E�E�E
* ������
*	int src_pix_s = (g_ScreenBufferSize.X * g_ScreenBufferSize.Y * sizeof(char));
*	RGBQUAD* tmp = g_FrameBuffer32bitD2D;
*	for (int n = 0; n < src_pix_s; n++) {
*		//8�r�b�g�̃p���b�g�C���f�b�N�X�o�b�t�@�Ńp���b�g�o�b�t�@�Q�Ƃ���32�r�b�gRGBQUAD�o�b�t�@�֏�������
*		tmp[n] = g_PaletteD2D[_buf_8bit[n] % NUM_ANSI_PAL];
*	}
* ������
*/
/*
* �t���[���o�b�t�@�������_�[�p���X�g�ɒǉ�����
*/
void PrintFrameBuffer(void)
{
	if (g_pRenderTarget == NULL) {
		return;
	}
	push_screen_buffer();
}	//PrintFrameBuffer

//================================================================
//	�������ID2D1Bitmap�ɕ`�悷��
void WriteTextA(int _xp, int _yp, const char* _text, double _scale, D2D1::ColorF _fgc, D2D1::ColorF _bgc, bool _new_line)
{
	//---- �}���`�o�C�g�������Unicode������ɕϊ�����
	int wc_count = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, _text, -1, NULL, 0);	//'\0'���܂ޕ��������Ԃ�
	size_t wc_src_bytes = (wc_count * sizeof(wchar_t));
	wchar_t* src_txt = (wchar_t*)_malloca(wc_src_bytes);	//�X�^�b�N��Ɋm�ہifree�s�v�j
	memset(src_txt, 0, wc_src_bytes);
	int disp_siz = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, _text, -1, src_txt, wc_count);
	WriteTextW(_xp, _yp, src_txt, _scale, _fgc, _bgc, _new_line);
}
void WriteTextW(int _xp, int _yp, const wchar_t* _text, double _scale, D2D1::ColorF _fgc, D2D1::ColorF _bgc, bool _new_line)
{
	if (_scale < 1.0) {
		_scale = 1.0;
	}
	//Windows�֐��̖߂�l
	HRESULT hr;
	double f_w = (double)(g_FontSizeEx.dwFontSize.X);	//�t�H���g�i�P�h�b�g�j�̕�
	double f_h = (double)(g_FontSizeEx.dwFontSize.Y);	//�t�H���g�i�P�h�b�g�j�̍���

	//�e�L�X�g�t�H�[�}�b�g�̐���
	//CreateTextFormat( L"�t�H���g��", �R���N�V����, ����, �X�^�C��, �g�k, �T�C�Y, ���[�J���l�[���H, �󂯎��|�C���^ );
	IDWriteTextFormat* pTextFormat = NULL;
	g_pDWFactory->CreateTextFormat(L"�l�r ����", NULL,
		DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, f_h * _scale, L"", &pTextFormat);

	// �u���V�̍쐬
	ID2D1SolidColorBrush* pBrushFG = NULL;
	g_pRenderTarget->CreateSolidColorBrush(_fgc, &pBrushFG);	//�t�H���g�̐F
	ID2D1SolidColorBrush* pBrushBG = NULL;
	g_pRenderTarget->CreateSolidColorBrush(_bgc, &pBrushBG);	//�w�i�̐F

	D2D1_RECT_F trf;
	{
		//�����_�[�^�[�Q�b�g�̃T�C�Y�擾
		D2D1_SIZE_F szf = g_pRenderTarget->GetSize();
		//�e�L�X�g�̃��C�A�E�g�i�ʒu�A���A�����j�̐���
		IDWriteTextLayout* pTextLayout = NULL;
		// IDWriteTextLayout �擾
		hr = g_pDWFactory->CreateTextLayout(
			_text					// ������
			, (UINT32)wcslen(_text)	// ������̒���
			, pTextFormat           // DWriteTextFormat
			, szf.width     // �g�̕�
			, szf.height    // �g�̍���
			, &pTextLayout
		);
		_ASSERT(hr == S_OK);
		DWRITE_TEXT_METRICS mtx;	//�e�L�X�g���͂ދ�`�̌v���l
		/*
		* DWRITE_TEXT_METRICS
		* FLOAT left;			���C�A�E�g �{�b�N�X����Ƃ��������ݒ肳�ꂽ�e�L�X�g�̍��[�̃|�C���g (�O���t�̃I�[�o�[�n���O������)�B
		* FLOAT top;			���C�A�E�g �{�b�N�X�ɑ΂��鏑���ݒ肳�ꂽ�e�L�X�g�̍ŏ�_ (�O���t�̃I�[�o�[�n���O������)�B
		* FLOAT width;			�e�s���̖����̋󔒂𖳎������A�����ݒ肳�ꂽ�e�L�X�g�̕��B
		* "FLOAT widthIncludeTrailingWhitespace;"	�e�s���̖����̋󔒂��l�������A�����ݒ肳�ꂽ�e�L�X�g�̕��B
		* FLOAT height;			�����ݒ肳�ꂽ�e�L�X�g�̍����B	��̕�����̍����́A����̃t�H���g�̍s�̍����̃T�C�Y�ɂ���Č��܂�܂��B
		* FLOAT layoutWidth;	���C�A�E�g�ɗ^�����鏉�����B	�e�L�X�g���܂�Ԃ��ꂽ���ǂ����ɉ����āA�e�L�X�g �R���e���c�̕������傫���Ȃ����菬�����Ȃ����肵�܂��B
		* FLOAT layoutHeight;	���C�A�E�g�ɗ^�����鏉���̍����B�����̒����ɂ���ẮA�����R���e���c�̍��������傫���Ȃ����菬�����Ȃ����肵�܂��B
		* UINT32 maxBidiReorderingDepth;	�K�v�ȃq�b�g �e�X�g �{�b�N�X�̍ő吔���v�Z���邽�߂Ɏg�p�����A�C�ӂ̃e�L�X�g�s�̍ő���בւ����B���C�A�E�g�ɑo�����e�L�X�g���Ȃ��ꍇ�A�܂��̓e�L�X�g���܂������Ȃ��ꍇ�A�ŏ����x���� 1 �ł��B
		* UINT32 lineCount;		�s�̑����B
		*/
		// �v��
		hr = pTextLayout->GetMetrics(&mtx);
		_ASSERT(hr == S_OK);
		//_RPTN(_CRT_WARN, "%f,%f,%f,%f,\n%f,%f,\n%f,\n%d,\n%d\n",mtx.left, mtx.top, mtx.width, mtx.height,mtx.layoutWidth, mtx.layoutHeight,mtx.widthIncludingTrailingWhitespace,mtx.maxBidiReorderingDepth,mtx.lineCount);
		float left = (mtx.left + _xp) * f_w;
		float top = (mtx.top + _yp) * f_h;
		float right = left + mtx.width;
		float bottom = top + mtx.height;
		if (!_new_line) {
			//��ʒ[�ŉ��s�����Ȃ��ꍇ
			//right = left + (mtx.width * mtx.lineCount);	//�s�������R�Ɋg��
			//bottom = top + (mtx.height / mtx.lineCount);	//�P�s���ɏk��
			//�Čv�Z
			hr = g_pDWFactory->CreateTextLayout(
				_text					// ������
				, (UINT32)wcslen(_text)	// ������̒���
				, pTextFormat           // DWriteTextFormat
				, (mtx.width * mtx.lineCount)     // �g�̕�
				, (mtx.height / mtx.lineCount)    // �g�̍���
				, &pTextLayout
			);
			_ASSERT(hr == S_OK);
			hr = pTextLayout->GetMetrics(&mtx);
			_ASSERT(hr == S_OK);
			left = (mtx.left + _xp) * f_w;
			top = (mtx.top + _yp) * f_h;
			right = left + mtx.width;
			bottom = top + mtx.height;
		}
		//��������͂ދ�`���쐬
		if( g_PrintStringCompatibleMode ){
			trf = D2D1::RectF(left / ((f_w*2)/f_h), top, right, bottom);	//�݊����[�h�F�����̃s�N�Z���̔䗦�͂P�F�Q
		}
		else{
			trf = D2D1::RectF(left, top, right, bottom);
		}
		// IDWriteTextLayout�̔j��
		pTextLayout->Release();
	}

#if true

	//�݊��r�b�g�}�b�v�̃����_�[�^�[�Q�b�g���쐬
	ID2D1BitmapRenderTarget* p_bitmap_render_target = NULL;
	hr = g_pRenderTarget->CreateCompatibleRenderTarget(&p_bitmap_render_target);
	_ASSERT(hr == S_OK);
	//��������������������������������
	//�e�L�X�g���C���[�`��
	p_bitmap_render_target->BeginDraw();
	// �l�p�`�̕`��
	p_bitmap_render_target->FillRectangle(&trf, pBrushBG);
	//p_bitmap_render_target->Clear();
	p_bitmap_render_target->DrawText(
		_text   // ������
		, (UINT32)wcslen(_text)    // ������
		, pTextFormat
		, &trf//&D2D1::RectF(0, 0, oTargetSize.width, oTargetSize.height)
		, pBrushFG
		, D2D1_DRAW_TEXT_OPTIONS_NONE
		//, D2D1_DRAW_TEXT_OPTIONS_CLIP
	);
	//p_bitmap_render_target->DrawRectangle(&trf, pBrushFG, 1.0f);	// �f�o�b�O�p�g(�l�p�`)�̕`��
	//
	hr = p_bitmap_render_target->EndDraw();
	//��������������������������������
	_ASSERT(hr == S_OK);
	//�������� �`�悵���r�b�g�}�b�v�����X�g�ɒǉ� ������
	ID2D1Bitmap* pD2D1_Bmp;
	p_bitmap_render_target->GetBitmap(&pD2D1_Bmp);	//�`�悵���r�b�g�}�b�v���擾
	g_pTextBmpList.push_back(pD2D1_Bmp);	//�r�b�g�}�b�v���e�L�X�g���C���[�ɒǉ�
	p_bitmap_render_target->Release();

#endif // false

	// �e�L�X�g�t�H�[�}�b�g�̔j��
	if (pTextFormat != NULL) {
		pTextFormat->Release();
	}
	// �u���V�̔j��
	if (pBrushBG != NULL) {
		pBrushBG->Release();
	}
	if (pBrushFG != NULL) {
		pBrushFG->Release();
	}

	return;
}	//WriteTextW

//================================================================
// �_�u���o�b�t�@�֘A
//================================================================

/**
 * @brief	�_�u���o�b�t�@������
 *
 * @return	�o�b�t�@�n���h���擾���s
 */
int InitDoubleBuffer(void)
{
	return 0;
}

/**
 * @brief	�_�u���o�b�t�@���̕`��ʐؑ�
 */
void FlipScreen(void)
{
	//Windows�֐��̖߂�l
	HRESULT hr;
	/*
	* �����_�[�^�[�Q�b�g�ւ̕`��
	* �r�b�g�}�b�v�̏������o�����烌���_�[�^�[�Q�b�g�ɓn���ĕ`����s���B
	* �f�t�H���g�̕�ԃ��[�h�� D2D1_BITMAP_INTERPOLATION_MODE_LINEAR �ɂȂ��Ă���̂ŁA
	* �����w�肵�Ȃ��Ɗg��k�����ɂڂ₯�Ă��܂��B
	*/
	//left,top,right,bottom --- x,y,x,y
	const FLOAT	scale_x = g_FontSizeEx.dwFontSize.X;	//�`��{��
	const FLOAT	scale_y = g_FontSizeEx.dwFontSize.Y;	//�`��{��
	const FLOAT	opacity = 1.0f;	//�����x�F�s����(1.0f)�`(0.0f)����
	//�`�悷���`���쐬
	//D2D1_RECT_F rf = { 0,0,((FLOAT)g_ScreenBufferSize.X - 1) * scale_x,((FLOAT)g_ScreenBufferSize.Y - 1) * scale_y };
	D2D1_RECT_F rf = { 0,0,(FLOAT)g_ScreenBufferSize.X * scale_x,(FLOAT)g_ScreenBufferSize.Y * scale_y };
	//D2D1_RECT_F drf = { 0,0,(FLOAT)g_ScreenBufferSize.X - 1,(FLOAT)g_ScreenBufferSize.Y - 1 };
	//D2D1_RECT_F srf = { 0,0,(FLOAT)g_ScreenBufferSize.X - 1,(FLOAT)g_ScreenBufferSize.Y - 1 };
	//��������������������������������
	g_pRenderTarget->BeginDraw();	//�`��J�n

	g_pRenderTarget->Clear();		//��ʏ���
	//DrawBitmap(bitmap,�`�悳���̈�̃T�C�Y�ƈʒu,�s�����x,��ԃ��[�h)
	for (ID2D1Bitmap* pbmp : g_pBmpList) {
		//���X�g�ɗ��܂��Ă���r�b�g�}�b�v�I�u�W�F�N�g��S�ĕ\������B
		//�h�b�g�g�債�Ă��ڂ₯�Ȃ��l��"_NEAREST_NEIGHBOR"���w�肵�Ă���B
		g_pRenderTarget->DrawBitmap(pbmp, &rf, opacity, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);	//�⊮����
		pbmp->Release();
	}
	g_pBmpList.clear();	//���X�g������

#if false
	ID2D1BitmapRenderTarget* p_bitmap_render_target = NULL;
	hr = g_pRenderTarget->CreateCompatibleRenderTarget(&p_bitmap_render_target);
	_ASSERT(hr == S_OK);
	ID2D1Bitmap* pD2D1_Bmp;
	p_bitmap_render_target->GetBitmap(&pD2D1_Bmp);	//�`�悵���r�b�g�}�b�v���擾
	g_pRenderTarget->DrawBitmap(pD2D1_Bmp, &rf, opacity, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);	//�⊮����
	pD2D1_Bmp->Release();
#else
	if (g_PrintStringCompatibleMode) {
		FLOAT bairitu = (scale_x*2) / scale_y;	//�݊����[�h�F�����̃s�N�Z���͂P���Q�����P���P�̃h�b�g�ɂ���ƃ��R�Ɋg�傳���
		rf = { 0,0,(FLOAT)g_ScreenBufferSize.X * scale_x * bairitu,(FLOAT)g_ScreenBufferSize.Y * scale_y};
	}
	//else{
	//	FLOAT bairitu = (scale_x*1) / scale_y;	//�m�[�}�����[�h�F�����̃s�N�Z���͂P���P�����P���Q�̃h�b�g�ɂ���ƃ��R�������ɂȂ�
	//	rf = { 0,0,(FLOAT)g_ScreenBufferSize.X * scale_x * bairitu,(FLOAT)g_ScreenBufferSize.Y * scale_y };
	//}
	for (ID2D1Bitmap* pbmp : g_pTextBmpList) {
		//���X�g�ɗ��܂��Ă���e�L�X�g�p�r�b�g�}�b�v�I�u�W�F�N�g��S�ĕ\������B
		//�h�b�g�g�債�Ă��ڂ₯�Ȃ��l��"_NEAREST_NEIGHBOR"���w�肵�Ă���B
		g_pRenderTarget->DrawBitmap(pbmp, &rf, opacity, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);	//�⊮����
		pbmp->Release();
	}
	g_pTextBmpList.clear();	//���X�g������
#endif // false

	hr = g_pRenderTarget->EndDraw();	//�`��I��
	//��������������������������������
	_ASSERT(hr == S_OK);
	g_PrintStringCompatibleMode = false;
	return;
}	//FlipScreen

/**
 * @brief	�t�H���g�T�C�Y�ύX
 *
 * @param	width [����] �t�H���g�̉��T�C�Y(1�`)
 * @param	height [����] �t�H���g�̏c�T�C�Y(1�`)
 */
void SetScreenFontSize(int _width, int _height)
{
	//g_FontSizeEx.dwFontSize.X = _width;
	//g_FontSizeEx.dwFontSize.Y = _height;
	//g_FontSizeEx.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	////g_FontSizeEx.FaceName = L"";
	////g_FontSizeEx.FontFamily = 
	//g_FontSizeEx.FontWeight;
	//g_FontSizeEx.nFont;

	// �t�H���g�T�C�Y�ύX
	g_FontSizeEx.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	g_FontSizeEx.dwFontSize.X = _width;
	g_FontSizeEx.dwFontSize.Y = _height;
	if (g_DisplayHandleD2D != NULL) {
		SetCurrentConsoleFontEx(g_DisplayHandleD2D, FALSE, &g_FontSizeEx);
	}

}	//SetScreenFontSize

//================================================================
// �t���[���o�b�t�@�摜�`��
//================================================================
/**
 * @brief	��ʁi�X�N���[���o�b�t�@�j����
 */
void ClearScreen(int _cc)
{
	//�摜�p�o�b�t�@����
	//memset(g_FrameBuffer32bitD2D, *((DWORD*)(&g_PaletteD2D[_cc % NUM_D2D_PAL])), sizeof(RGBQUAD) * g_ScreenBufferSize.X * g_ScreenBufferSize.Y);
	for (int i = 0; i < NUM_D2D_PAL; i++) {
		g_FrameBuffer32bitD2D[i] = g_PaletteD2D[_cc % NUM_D2D_PAL];
	}
}	//ClearScreen
void ClearScreen(int _red, int _green, int _blue)
{
	RGBQUAD rgb{ _blue,_green,_red,0};
	//rgb.rgbBlue = _blue;
	//rgb.rgbGreen = _green;
	//rgb.rgbRed = _red;
	//rgb.rgbReserved = _alpha;

	//�摜�p�o�b�t�@����
	//memset(g_FrameBuffer32bitD2D, ((DWORD*)&_rgb)[0], sizeof(RGBQUAD) * g_ScreenBufferSize.X * g_ScreenBufferSize.Y);
	for (int i = 0; i < (g_ScreenBufferSize.X * g_ScreenBufferSize.Y); i++) {
		g_FrameBuffer32bitD2D[i] = rgb;
	}
}	//ClearScreen
void ClearScreen(void)
{
	//�摜�p�o�b�t�@����
	ZeroMemory(g_FrameBuffer32bitD2D, sizeof(RGBQUAD) * g_ScreenBufferSize.X * g_ScreenBufferSize.Y);
}	//ClearScreen

/**
* @brief	�_��ł�
*
* @param	int _x,_y�F���W
* @param	RGBQUAD _rgb�F�F�iRGBQUAD:0x00RRGGBB�j
*/
void DrawPixel(int _x, int _y, RGBQUAD _rgb)
{
	if ((_x >= 0) && (_x < g_ScreenBufferSize.X) && (_y >= 0) && (_y < g_ScreenBufferSize.Y)) {
		g_FrameBuffer32bitD2D[_y * g_ScreenBufferSize.X + _x] = _rgb;
	}
}	//DrawPixel
/**
* @brief	�_��ł�
*
* @param	int _x,_y�F���W
* @param	int _c�F�F�i�p���b�g�ԍ��O�`�P�T�j
*/
void DrawPixel(int _x, int _y, unsigned char _c)
{
	DrawPixel(_x, _y, g_PaletteD2D[_c & 0xFF]);
}

#endif	//USED2D

//################################################################################################################################
//################################################################################################################################
//################################################################################################################################
//================================================================
// ������
//================================================================
/**
 * @brief	�R���\�[�� I/O ������
 *
 * @param	_width [����] �R���\�[���E�B���h�E�̉��T�C�Y(1�`)
 * @param	_height [����] �R���\�[���E�B���h�E�̏c�T�C�Y(1�`)
 */
void InitConio(int _width, int _height) {
	InitConioEx(_width, _height, DEF_FONTSIZE_X, DEF_FONTSIZE_Y, NULL, NULL);
}
/**
 * @brief	�R���\�[�� I/O �������i�g���Łj
 *
 * @param	int _width �F�R���\�[���E�B���h�E�̉��T�C�Y(1�`)
 * @param	int _height�F�R���\�[���E�B���h�E�̏c�T�C�Y(1�`)
 * @param	int _font_w�F�t�H���g�̉��T�C�Y(1�`)
 * @param	int _font_h�F�t�H���g�̏c�T�C�Y(1�`)
 * @param	bool _init_wbuf�F�_�u���o�b�t�@�̏�����(true=����/false=���Ȃ�)
 * @param	const wchar_t* _font_face_name�F�ݒ肷��t�H���g�̖��O(Unicode������)
 * @param	const COLORREF* _pal16�F�ݒ肷��16�F�p���b�g
 *
 * @return	����
 */
void InitConioEx(int _width, int _height, int _font_w, int _font_h)
{
	InitConioEx(_width, _height, _font_w, _font_h, NULL, NULL, false);
}
void InitConioEx(int _width, int _height, int _font_w, int _font_h, bool _init_wbuf)
{
	InitConioEx(_width, _height, _font_w, _font_h, NULL, NULL, _init_wbuf);
}
void InitConioEx(int _width, int _height, int _font_w, int _font_h, const wchar_t* _font_face_name, const COLORREF* _pal16, bool _init_wbuf)
{
	const char* str_locale = setlocale(LC_CTYPE, "Japanese_Japan");

	//�L�[�o�b�t�@�N���A
	memset(g_KeyPress, 0, NUM_KEYS);
	memset(g_KeyEdge, 0, NUM_KEYS);
	memset(g_KeyLast, 0, NUM_KEYS);

	//----------------------------------------------------------------
	//�R���\�[���E�B���h�E�̃E�B���h�E�n���h��(HWND)�擾���ۑ�
	g_hConWnd = GetConsoleWindow();
#ifdef USED2D
	//----------------------------------------------------------------
	// �R�}���h������ۑ����Ȃ�
	CONSOLE_HISTORY_INFO history_info;
	history_info.cbSize = sizeof(CONSOLE_HISTORY_INFO);
	history_info.HistoryBufferSize = 0;
	history_info.NumberOfHistoryBuffers = 0;
	history_info.dwFlags = 0;
	SetConsoleHistoryInfo(&history_info);

	//----------------------------------------------------------------
	// ���O�̃f�B�X�v���C���擾
	g_OrgOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);	//�o�̓n���h��
	GetConsoleMode(g_OrgOutputHandle, &g_OrgOutputHandleMode);	//�o�̓R���\�[�����
	g_OrgInputHandle = GetStdHandle(STD_INPUT_HANDLE);	//���̓n���h��
	GetConsoleMode(g_OrgInputHandle, &g_OrgInputHandleMode);	//���̓R���\�[�����
	g_InputHandleD2D = g_OrgInputHandle;	//���͂͋N�����Ɠ����n���h��
	//----------------------------------------------------------------
	//��ʏ���ۑ����Ă����i16�F�p���b�g�܂ށj
	g_OrgScreenBufferInfoEx.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
	GetConsoleScreenBufferInfoEx(g_OrgOutputHandle, &g_OrgScreenBufferInfoEx);
	// ���݂̃J���[�p���b�g��ۑ�
	for (int n = 0; n < NUM_PALETTE; n++) {
		g_OrgColorTableD2D[n] = g_OrgScreenBufferInfoEx.ColorTable[n];
		//�f�t�H���g�p���b�g�̍ŏ��̂P�U�F�̈ʒu�Ɏ�荞��
		g_PaletteD2D[n].rgbBlue = (g_OrgColorTableD2D[n] & 0x00FF0000) >> 16;
		g_PaletteD2D[n].rgbGreen = (g_OrgColorTableD2D[n] & 0x0000FF00) >> 8;
		g_PaletteD2D[n].rgbRed = (g_OrgColorTableD2D[n] & 0x000000FF);
		g_PaletteD2D[n].rgbReserved = (g_OrgColorTableD2D[n] & 0x0FF00000) >> 24;
	}
	//----------------------------------------------------------------
	//�t�H���g�T�C�Y�ۑ��F�I���W�i���ۑ�
	g_OrgFontSizeEx.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	GetCurrentConsoleFontEx(g_OrgOutputHandle, FALSE, &g_OrgFontSizeEx);	//��2�p�����[�^��TRUE���Ɖ�ʃo�b�t�@�Ɠ����T�C�Y���Ԃ�݂������E�E�E
	//----------------------------------------------------------------
	//���݂̃J�[�\����ԕۑ�
	GetConsoleCursorInfo(g_OrgOutputHandle, &g_OrgCursorInfo);
	//�J�[�\���\��OFF
	CONSOLE_CURSOR_INFO cci = { sizeof(CONSOLE_CURSOR_INFO) };
	cci.bVisible = FALSE;
	SetConsoleCursorInfo(g_OrgOutputHandle, &cci);
	//----------------------------------------------------------------
	//�E�B���h�E�̏�ԕۑ�
	g_OrgWindowStylePtr = GetWindowLongPtr(g_hConWnd, GWL_STYLE);

	//----------------------------------------------------------------
	// GetSystemMetrics
	// https://learn.microsoft.com/ja-JP/windows/win32/api/winuser/nf-winuser-getsystemmetrics
	//int cx_border = GetSystemMetrics(SM_CXBORDER);
	//int cy_border = GetSystemMetrics(SM_CYBORDER);	//�E�B���h�E�̋��E���̍��� (�s�N�Z���P��)�B 
	//int cx_size = GetSystemMetrics(SM_CXSIZE);				//�E�B���h�E �L���v�V�����܂��̓^�C�g�� �o�[�̃{�^���̕�(�s�N�Z���P��)�B
	//int cy_size = GetSystemMetrics(SM_CYSIZE);				//�E�B���h�E �L���v�V�����܂��̓^�C�g�� �o�[�̃{�^���̍���(�s�N�Z���P��)�B
	//int cx_size_frame = GetSystemMetrics(SM_CXSIZEFRAME);	//�T�C�Y��ύX�ł���E�B���h�E�̎��͂̃T�C�Y�ύX���E���̑��� (�s�N�Z���P��)�B 
	//int cy_size_frame = GetSystemMetrics(SM_CYSIZEFRAME);	//�T�C�Y��ύX�ł���E�B���h�E�̎��͂̃T�C�Y�ύX���E���̑��� (�s�N�Z���P��)�B 
	//				//SM_CXSIZEFRAME�͐������E���̕��ASM_CYSIZEFRAME�͐������E���̍����ł��B
	//int cx_v_scroll = GetSystemMetrics(SM_CXVSCROLL);	//�����X�N���[�� �o�[�̕�(�s�N�Z���P��)�B
	//int cy_h_scroll = GetSystemMetrics(SM_CYHSCROLL);	//�����X�N���[�� �o�[�̍���(�s�N�Z���P��)�B
	////int cx_caption = GetSystemMetrics(SM_CXCAPTION);	//�L���v�V�����̈�̍���(�s�N�Z���P��)�B
	//int cy_caption = GetSystemMetrics(SM_CYCAPTION);	//�L���v�V�����̈�̍���(�s�N�Z���P��)�B
	//int cx_min = GetSystemMetrics(SM_CXMIN);	//�E�B���h�E�̍ŏ���(�s�N�Z���P��)�B
	//int cy_min = GetSystemMetrics(SM_CYMIN);	//�E�B���h�E�̍ŏ�����(�s�N�Z���P��)�B

	int cx_size_frame = GetSystemMetrics(SM_CXSIZEFRAME); // ���E����X����
	int cy_size_frame = GetSystemMetrics(SM_CYSIZEFRAME); // ���E����Y����
	int cy_caption = GetSystemMetrics(SM_CYCAPTION);     // �^�C�g���o�[�̍���
	RECT rct_1;
	{
		//�N���C�A���g�̈悪�w�肳�ꂽ�傫���ɂȂ�悤�Ɍv�Z����
		GetClientRect(g_hConWnd, &rct_1);	//���݂̃N���C�A���g�̈�
		int w1 = rct_1.right - rct_1.left + 0;//1;
		int h1 = rct_1.bottom - rct_1.top + 0;//1;
		RECT rct_2;
		GetWindowRect(g_hConWnd, &rct_2);	//���݂̃E�B���h�E�̈�
		int w2 = rct_2.right - rct_2.left + 0;//1;
		int h2 = rct_2.bottom - rct_2.top + 0;//1;
		//�E�B���h�E�̈�ƃN���C�A���g�̈�̍������v�Z����
		int w3 = w2 - w1;
		int h3 = h2 - h1;
		//int w = _width * _font_w + w3;
		//int h = _height * _font_h + h3;
		int w = (_width * _font_w) + (cx_size_frame * 2);
		int h = (_height * _font_h) + (cy_size_frame * 2) + cy_caption;
		//�w�肳�ꂽ�T�C�Y�����E�B���h�E�̈�ɂȂ�悤�ɃE�B���h�E�T�C�Y��ݒ肷��
		//SetWindowPos(g_hConWnd, HWND_TOP, rct_2.left, rct_2.top, w, h, SWP_SHOWWINDOW /*| SWP_NOMOVE*/ );
		//SetWindowPos(g_hConWnd, HWND_TOP, rct_2.left, rct_2.top, _width * 2, _height * 2, SWP_SHOWWINDOW /*| SWP_NOMOVE*/);
		SetWindowPos(g_hConWnd, HWND_TOP, rct_2.left, rct_2.top, w, h, SWP_SHOWWINDOW /*| SWP_NOMOVE*/);
	}
	//WINDOWINFO	winfo;
	//winfo.cbSize = sizeof(WINDOWINFO);
	//GetWindowInfo(g_hConWnd, &winfo);



	//----------------------------------------------------------------
	//�o�b�t�@�T�C�Y�i�X�N���[���o�b�t�@�b�t���[���o�b�t�@�j
	g_ScreenBufferSize.X = _width;
	g_ScreenBufferSize.Y = _height;
	//�t�H���g�T�C�Y
	g_FontSizeEx.dwFontSize.X = _font_w;
	g_FontSizeEx.dwFontSize.Y = _font_h;
	//----------------------------------------------------------------
	// �t�H���g���̐ݒ�i�w�肪����Ώ��̂��ݒ肷��j
	g_FontSizeEx.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	g_FontSizeEx.FontFamily = (FF_DONTCARE | 0x00);	//���̕s�������m�X�y�[�X	//�����l�H54�i0x36:0b0011_0110�j
	g_FontSizeEx.FontWeight = 100;	//�׎�	//�����l�H400;
	//�t�H���g���w�肪����΂��̖��O���Z�b�g����B
	//�t�H���g���w�肪������Ό��݂̒l���g����B
	//�t�H���g����Unicode�w��iFaceName��WCHAR�Ȃ̂Łj
	if (_font_face_name != nullptr) {
		//PCONSOLE_FONT_INFOEX inf;
		memset(g_FontSizeEx.FaceName, 0, sizeof(g_FontSizeEx.FaceName));
		CopyMemory(g_FontSizeEx.FaceName, _font_face_name, LF_FACESIZE);
	}
	//----------------------------------------------------------------
	// �X�N���[���o�b�t�@�̏���ݒ�i�p���b�g�܂ށj
	g_ScreenBufferInfoEx.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
	if (_pal16 != NULL) {
		//�p���b�g�w�肪����Γ]������B
		for (int n = 0; n < NUM_PALETTE; n++) {
			g_ScreenBufferInfoEx.ColorTable[n] = _pal16[n];
		}
	}
	else {
		//�p���b�g�w�肪������΂��̒��O�̃f�B�X�v���C���̃p���b�g��ݒ肷��B
		for (int n = 0; n < NUM_PALETTE; n++) {
			g_ScreenBufferInfoEx.ColorTable[n] = g_OrgColorTableD2D[n];
		}
	}
	// �o�b�t�@�T�C�Y�ύX
	g_ScreenBufferSize.X = _width;
	g_ScreenBufferSize.Y = _height;
	// �E�B���h�E�T�C�Y�ύX
	g_WindowSize.Left = 0;
	g_WindowSize.Top = 0;
	g_WindowSize.Right = _width;// - 1;
	g_WindowSize.Bottom = _height;// - 1;
	//�R���\�[���X�N���[���o�b�t�@���̐ݒ�
	g_ScreenBufferInfoEx.dwSize = g_ScreenBufferSize;	//�����̗�ƍs�̃R���\�[����ʃo�b�t�@�[�̃T�C�Y
	g_ScreenBufferInfoEx.dwCursorPosition = { 0,0 };	//COORD{x,y}:�R���\�[����ʃo�b�t�@�[���̃J�[�\���̗���W�ƍs���W
	g_ScreenBufferInfoEx.wAttributes = (FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);	//��ʃo�b�t�@�[�̕�������:����=�p���b�g#15�^�w�i=�p���b�g#0
	g_ScreenBufferInfoEx.srWindow = g_WindowSize;		//�\���E�B���h�E�̍�����ƉE�����̃R���\�[����ʂ̃o�b�t�@�[���W
	g_ScreenBufferInfoEx.dwMaximumWindowSize = g_ScreenBufferInfoEx.dwSize;	//�R���\�[���E�B���h�E�̍ő�T�C�Y
	g_ScreenBufferInfoEx.bFullscreenSupported = FALSE;			//�S��ʕ\�����[�h�̃T�|�[�g
	g_ScreenBufferInfoEx.ColorTable;	//�R���\�[���̐F�ݒ�:COLORREF[16]{0x00bbggrr,,,}
	//----------------------------------------------------------------
	//�R���\�[���p�o�b�t�@�쐬
	//�y���z�����܂ł�"g_ScreenBufferInfoEx"��"g_FontSizeEx"���ݒ�ς݂ł��鎖�B
	//�o�͗p�����B
	g_DisplayHandleD2D = create_screen_buffer(&g_ScreenBufferInfoEx, &g_FontSizeEx);
	//���͗p�͋N�����̂Ɠ������̂��g���B
	SetConsoleMode(g_InputHandleD2D, CONSOLE_INPUT_MODE);	//���̓n���h������̓��[�h�ɐݒ�
	//���݂̐ݒ�l�Ńt�H���g�ύX
	//�y���z�R���\�[���o�b�t�@g_DisplayHandle[]�̂ǂꂩ����o���Ă���SetScreenFontSize()���Ăяo�����B
	SetScreenFontSize(_font_w, _font_h);

	//�N���C�A���g�̈悪�w�肳�ꂽ�傫���ɂȂ�悤�Ɍv�Z���Đݒ�
	{
		GetClientRect(g_hConWnd, &rct_1);	//���݂̃N���C�A���g�̈�
		int w1 = rct_1.right - rct_1.left + 1;
		int h1 = rct_1.bottom - rct_1.top + 1;
		RECT rct_2;
		GetWindowRect(g_hConWnd, &rct_2);	//���݂̃E�B���h�E�̈�
		int w2 = rct_2.right - rct_2.left + 1;
		int h2 = rct_2.bottom - rct_2.top + 1;
		//�E�B���h�E�̈�ƃN���C�A���g�̈�̍������v�Z����
		int w3 = w2 - w1;
		int h3 = h2 - h1;
		int w = _width * _font_w + w3;
		int h = _height * _font_h + h3;
		//�w�肳�ꂽ�T�C�Y�����E�B���h�E�̈�ɂȂ�悤�ɃE�B���h�E�T�C�Y��ݒ肷��
		SetWindowPos(g_hConWnd, HWND_TOP, rct_2.left, rct_2.top, w, h, SWP_SHOWWINDOW /*| SWP_NOMOVE*/);
	}

	//----------------------------------------------------------------
	//�������@Direct2D ������ ������
	InitD2D(_width, _height);

	//�E�B���h�E�T�C�Y�̌Œ�
	FixWin();

	GetClientRect(g_hConWnd, &rct_1);
	//�����܂łɐݒ�ς݂̉�ʂƃt�H���g�̃T�C�Y���擾
	GetConWinSize(g_ConWinSize);

	//�������ׂ̈ɃL�[���͌Ăяo��
	GetKeyAll();
	//�t���[�������̏�����
	InitFrameSync(60.0);
	return;

#else
	//----------------------------------------------------------------
	//�E�B���h�E�T�C�Y�ύX�̂n�e�e
	g_OrgWindowStyle = FixWin();	//���̃X�^�C����ۑ�
	//----------------------------------------------------------------
	// �R�}���h������ۑ����Ȃ�
	CONSOLE_HISTORY_INFO history_info;
	history_info.cbSize = sizeof(CONSOLE_HISTORY_INFO);
	history_info.HistoryBufferSize = 0;
	history_info.NumberOfHistoryBuffers = 0;
	history_info.dwFlags = 0;
	SetConsoleHistoryInfo(&history_info);

	//----------------------------------------------------------------
	// ���O�̃f�B�X�v���C���擾
	g_OrgOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);	//�o�̓n���h��
	GetConsoleMode(g_OrgOutputHandle, &g_OrgOutputHandleMode);	//�o�̓R���\�[�����
	//----------------------------------------------------------------
	//�A�N�e�B�u�ȉ�ʃo�b�t�@��ݒ肷��B
	SetConsoleActiveScreenBuffer(g_OrgOutputHandle);

	g_OrgInputHandle = GetStdHandle(STD_INPUT_HANDLE);

	GetConsoleMode(g_OrgInputHandle, &g_OrgInputHandleMode);
	//��ʏ���ۑ����Ă����i16�F�p���b�g�܂ށj
	g_OrgScreenBufferInfoEx.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
	GetConsoleScreenBufferInfoEx(g_OrgOutputHandle, &g_OrgScreenBufferInfoEx);
	// ���݂̃J���[�p���b�g��ۑ�
	for (int n = 0; n < NUM_PALETTE; n++) {
		g_OrgColorTable[n] = g_OrgScreenBufferInfoEx.ColorTable[n];
	}
	//�t�H���g�T�C�Y�ۑ��F�I���W�i���ۑ�
	g_OrgFontSizeEx.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	GetCurrentConsoleFontEx(g_OrgOutputHandle, FALSE, &g_OrgFontSizeEx);	//��2�p�����[�^��TRUE���Ɖ�ʃo�b�t�@�Ɠ����T�C�Y���Ԃ�݂������E�E�E
	//���݂̃J�[�\����ԕۑ�
	GetConsoleCursorInfo(g_OrgOutputHandle, &g_OrgCursorInfo);

	//----------------------------------------------------------------
	// �t�H���g���̐ݒ�i�w�肪����Ώ��̂��ݒ肷��j
	g_FontSizeEx.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	g_FontSizeEx.FontFamily = (FF_DONTCARE | 0x00);	//���̕s�������m�X�y�[�X	//�����l�H54�i0x36:0b0011_0110�j
	g_FontSizeEx.FontWeight = 100;	//�׎�	//�����l�H400;
	//�t�H���g���w�肪����΂��̖��O���Z�b�g����B
	//�t�H���g���w�肪������Ό��݂̒l���g����B
	//�t�H���g����Unicode�w��iFaceName��WCHAR�Ȃ̂Łj
	if (_font_face_name != nullptr) {
		//PCONSOLE_FONT_INFOEX inf;
		memset(g_FontSizeEx.FaceName, 0, sizeof(g_FontSizeEx.FaceName));
		CopyMemory(g_FontSizeEx.FaceName, _font_face_name, LF_FACESIZE);
	}

	//----------------------------------------------------------------
	// �X�N���[���o�b�t�@�̏���ݒ�i�p���b�g�܂ށj
	g_ScreenBufferInfoEx.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
	if (_pal16 != NULL) {
		//�p���b�g�w�肪����Γ]������B
		for (int n = 0; n < NUM_PALETTE; n++) {
			g_ScreenBufferInfoEx.ColorTable[n] = _pal16[n];
		}
	}
	else {
		//�p���b�g�w�肪������΂��̒��O�̃f�B�X�v���C���̃p���b�g��ݒ肷��B
		for (int n = 0; n < NUM_PALETTE; n++) {
			g_ScreenBufferInfoEx.ColorTable[n] = g_OrgColorTable[n];
		}
	}
	// �o�b�t�@�T�C�Y�ύX
	g_ScreenBufferSize.X = _width;
	g_ScreenBufferSize.Y = _height;
	// �E�B���h�E�T�C�Y�ύX
	g_WindowSize.Left = 0;
	g_WindowSize.Top = 0;
	g_WindowSize.Right = _width;// - 1;
	g_WindowSize.Bottom = _height;// - 1;
	//�R���\�[���X�N���[���o�b�t�@���̐ݒ�
	g_ScreenBufferInfoEx.dwSize = g_ScreenBufferSize;	//�����̗�ƍs�̃R���\�[����ʃo�b�t�@�[�̃T�C�Y
	g_ScreenBufferInfoEx.dwCursorPosition = { 0,0 };	//COORD{x,y}:�R���\�[����ʃo�b�t�@�[���̃J�[�\���̗���W�ƍs���W
	g_ScreenBufferInfoEx.wAttributes = (FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);	//��ʃo�b�t�@�[�̕�������:����=�p���b�g#15�^�w�i=�p���b�g#0
	g_ScreenBufferInfoEx.srWindow = g_WindowSize;		//�\���E�B���h�E�̍�����ƉE�����̃R���\�[����ʂ̃o�b�t�@�[���W
	g_ScreenBufferInfoEx.dwMaximumWindowSize = g_ScreenBufferInfoEx.dwSize;	//�R���\�[���E�B���h�E�̍ő�T�C�Y
	g_ScreenBufferInfoEx.bFullscreenSupported = FALSE;			//�S��ʕ\�����[�h�̃T�|�[�g
	g_ScreenBufferInfoEx.ColorTable;	//�R���\�[���̐F�ݒ�:COLORREF[16]{0x00bbggrr,,,}

	//----------------------------------------------------------------
	//�R���\�[���p�o�b�t�@�쐬
	//�y���z�����܂ł�"g_ScreenBufferInfoEx"��"g_FontSizeEx"���ݒ�ς݂ł��鎖�B
	//�o�͗p�����B
	g_DisplayHandle[0] = create_screen_buffer(&g_ScreenBufferInfoEx, &g_FontSizeEx);
	//g_DisplayHandle[0] = g_OrgOutputHandle;
	g_DisplayHandle[1] = NULL;
	//���͗p�͋N�����̂Ɠ������̂��g���B
	g_InputHandle = g_OrgInputHandle;
	SetConsoleMode(g_InputHandle, CONSOLE_INPUT_MODE);	//���̓n���h������̓��[�h�ɐݒ�

	//----------------------------------------------------------------
	//���݂̐ݒ�l�Ńt�H���g�ύX
	//�y���z�R���\�[���o�b�t�@g_DisplayHandle[]�̂ǂꂩ����o���Ă���SetScreenFontSize()���Ăяo�����B
	SetScreenFontSize(_font_w, _font_h);

	//----------------------------------------------------------------
	//�A�N�e�B�u�ȉ�ʃo�b�t�@��ݒ肷��B
	SetConsoleActiveScreenBuffer(g_DisplayHandle[0]);

	//----------------------------------------------------------------
	// 16�F�p�X�N���[���o�b�t�@�̔z����쐬
	//g_ScreenBuffer4bit = (CHAR_INFO*)calloc(g_ScreenBufferSize.X * g_ScreenBufferSize.Y, sizeof(CHAR_INFO));
	g_ScreenBuffer4bit = (WORD*)calloc(g_ScreenBufferSize.X * g_ScreenBufferSize.Y, sizeof(WORD));
	//----------------------------------------------------------------
	// 16�F�p�t���[���o�b�t�@�̔z����쐬
	g_FrameBuffer4bit = (char*)calloc(g_ScreenBufferSize.X * g_ScreenBufferSize.Y, sizeof(char));

	//----------------------------------------------------------------
	//�P�s�N�Z���̂q�f�a�l�ݒ�Ɏg��"�O�O�O"�`"�Q�T�T"�̐��l�����R�[�h������Ă����B
	for (int i = 0; i < 256; i++) {
		CharRGBconvTBL[0][i] = 0x30 + (i / 100);		//�O�O�O�`�Q�T�T�̂P�O�O�̈�
		CharRGBconvTBL[1][i] = 0x30 + ((i % 100) / 10);	//�O�O�O�`�Q�T�T�̂P�O�̈�
		CharRGBconvTBL[2][i] = 0x30 + (i % 10);			//�O�O�O�`�Q�T�T�̂P�̈�
	}

	//----------------------------------------------------------------
	//�t���J���[�摜�p�̏�����
	init_24bit_color_image();
	//----------------------------------------------------------------
	//�Q�T�U�F�p���b�g�摜�p�̏�����
	init_256color_image();
	//�����܂łɐݒ�ς݂̉�ʂƃt�H���g�̃T�C�Y���擾
	GetConWinSize(g_ConWinSize);
	//�_�u���o�b�t�@������
	if (_init_wbuf) {
		InitDoubleBuffer();
	}

	//�E�B���h�E�T�C�Y�̌Œ�
	FixWin();
	//�������ׂ̈ɃL�[���͌Ăяo��
	GetKeyAll();
	//�t���[�������̏�����
	InitFrameSync(60.0);
	return;
#endif	USED2D
}	//InitConioEx

//################################################################################################################################
//################################################################################################################################
//################################################################################################################################
/**
* @brief	conioex�̏I������
* @param	�Ȃ�
* @return	�Ȃ�
*/
void EndConioEx(void)
{

#ifdef USED2D
	EndD2D();
#else
	//�X�N���[���o�b�t�@(24bit�t���J���[)�p�̔z����
	if (g_ScreenBufferFull != NULL) {
		free(g_ScreenBufferFull);
		g_ScreenBufferFull = NULL;
	}
	//�t���[���o�b�t�@(24bit�t���J���[)�p�̔z����
	if (g_FrameBufferFull != NULL) {
		free(g_FrameBufferFull);
		g_FrameBufferFull = NULL;
	}
	//�X�N���[���o�b�t�@(�P�U�F�摜)�p�̔z����
	if (g_ScreenBuffer4bit) {
		free(g_ScreenBuffer4bit);
		g_ScreenBuffer4bit = NULL;
	}
	//�t���[���o�b�t�@(�P�U�F�摜)�p�̔z����
	if (g_FrameBuffer4bit) {
		free(g_FrameBuffer4bit);
		g_FrameBuffer4bit = NULL;
	}
#endif // USED2D

	SetConsoleActiveScreenBuffer(g_OrgOutputHandle);	//�A�N�e�B�u�ȃR���\�[����ʃo�b�t�@��ؑւ�
	SetConsoleMode(g_OrgOutputHandle, g_OrgOutputHandleMode);	//�R���\�[�����[�h���A
	SetConsoleCursorInfo(g_OrgOutputHandle, &g_OrgCursorInfo);	//�J�[�\���\����Ԃ̕��A
	//�t�H���g�T�C�Y��"SetCurrentConsoleFontEx()"�������Ă��߂�݂��������E�E�E
	g_OrgFontSizeEx.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	SetCurrentConsoleFontEx(g_OrgOutputHandle, FALSE, &g_OrgFontSizeEx);	//�t�H���g�T�C�Y�����ɖ߂�
	//InitConioEx���O�̃X�N���[���o�b�t�@��Ԃɖ߂��B
	g_OrgScreenBufferInfoEx.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
	SetConsoleScreenBufferInfoEx(g_OrgOutputHandle, &g_OrgScreenBufferInfoEx);	//���ꂪ�����ƃp���b�g���߂�Ȃ�
#ifdef _DEBUG
	//TEST:�ݒ�l���i�[����Ă��邩�m�F
	CONSOLE_SCREEN_BUFFER_INFOEX	csbiex{ sizeof(CONSOLE_SCREEN_BUFFER_INFOEX) };
	GetConsoleScreenBufferInfoEx(g_OrgOutputHandle, &csbiex);
#endif // _DEBUG

#ifdef USED2D
	//�_�u���o�b�t�@�폜
	if (g_DisplayHandleD2D != NULL) {
		CloseHandle(g_DisplayHandleD2D);
		g_DisplayHandleD2D = NULL;
	}
#else
	//�_�u���o�b�t�@�폜
	if (g_DisplayHandle[0] != NULL) {
		CloseHandle(g_DisplayHandle[0]);
		g_DisplayHandle[0] = NULL;
	}
	if (g_DisplayHandle[1] != NULL) {
		CloseHandle(g_DisplayHandle[1]);
		g_DisplayHandle[1] = NULL;
	}
#endif // USED2D

	//window style recover.
#ifdef USED2D
	SetWindowLongPtr(g_hConWnd, GWL_STYLE, g_OrgWindowStylePtr);
#else
	SetWindowLong(g_hConWnd, GWL_STYLE, g_OrgWindowStyle);
#endif // USED2D
}	//EndConioEx

//################################################################################################################################
//################################################################################################################################
//################################################################################################################################

/**
* @brief	�X�N���[���o�b�t�@�̐���
* @param	CONSOLE_SCREEN_BUFFER_INFOEX*:�X�N���[���o�b�t�@���̃|�C���^
* @param	CONSOLE_FONT_INFOEX*:�t�H���g���̃|�C���^
* @return	�X�N���[���o�b�t�@�̃n���h��
*	INVALID_HANDLE_VALUE:�Ȃ玸�s
*/
static HANDLE create_screen_buffer(CONSOLE_SCREEN_BUFFER_INFOEX* pCsbix, CONSOLE_FONT_INFOEX* pCfix)
{
	HANDLE new_handle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
#ifdef USED2D
	if (g_DisplayHandleD2D == INVALID_HANDLE_VALUE) {
		//printf("�X�N���[���o�b�t�@�̃n���h���擾�Ɏ��s���܂���\n");
		return INVALID_HANDLE_VALUE;
	}
#else
	if (g_DisplayHandle[0] == INVALID_HANDLE_VALUE) {
		//printf("�X�N���[���o�b�t�@�̃n���h���擾�Ɏ��s���܂���\n");
		return INVALID_HANDLE_VALUE;
	}
#endif // USED2D
	// �t�H���g�T�C�Y�̕ύX
	pCfix->cbSize = sizeof(CONSOLE_FONT_INFOEX);	//�T�C�Y�͖���ݒ肵���������S�B
	SetCurrentConsoleFontEx(new_handle, FALSE, pCfix);
	//�X�N���[���o�b�t�@���̐ݒ�
	pCsbix->cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);	//�T�C�Y�͖���ݒ肵���������S�B
	SetConsoleScreenBufferInfoEx(new_handle, pCsbix);
	SetConsoleMode(new_handle, CONSOLE_OUTPUT_MODE);	// �o�b�t�@���㏑�����[�h��
	//�J�[�\���\���͂n�e�e�ɂ��Ă����B
	CONSOLE_CURSOR_INFO	cci;
	cci.dwSize = 1;
	cci.bVisible = FALSE;
	SetConsoleCursorInfo(new_handle, &cci);
#ifdef _DEBUG
	//TEST:�ݒ�l���i�[����Ă��邩�m�F
	CONSOLE_SCREEN_BUFFER_INFOEX	csbiex{ sizeof(CONSOLE_SCREEN_BUFFER_INFOEX) };
#ifdef USED2D
	GetConsoleScreenBufferInfoEx(g_DisplayHandleD2D, &csbiex);
#else
	GetConsoleScreenBufferInfoEx(g_DisplayHandle[0], &csbiex);
#endif // USED2D
#endif // _DEBUG
	return new_handle;
}	//create_screen_buffer

#ifndef USED2D
/**
 * @brief	�_�u���o�b�t�@������
 *
 * @return	�o�b�t�@�n���h���擾���s
 */
int InitDoubleBuffer(void)
{
	// �_�u���o�b�t�@�p�̃������[���m��
	if (g_DisplayHandle[0] == NULL) {
		g_DisplayHandle[0] = create_screen_buffer(&g_ScreenBufferInfoEx, &g_FontSizeEx);
		if (g_DisplayHandle[0] == INVALID_HANDLE_VALUE) {
			printf("�_�u���o�b�t�@[0]�̃n���h���擾�Ɏ��s���܂���\n");
			return -1;
		}
	}
	if (g_DisplayHandle[1] == NULL) {
		g_DisplayHandle[1] = create_screen_buffer(&g_ScreenBufferInfoEx, &g_FontSizeEx);
		if (g_DisplayHandle[1] == INVALID_HANDLE_VALUE) {
			printf("�_�u���o�b�t�@[1]�̃n���h���擾�Ɏ��s���܂���\n");
			return -1;
		}
	}
	return 0;
}	//InitDoubleBuffer

/**
 * @brief	�_�u���o�b�t�@���̕`��ʐؑ�
 */
void FlipScreen(void)
{
	SetConsoleActiveScreenBuffer(g_DisplayHandle[g_SwapFlg]);	// �o�b�t�@�����ւ��\��
	g_SwapFlg = (g_SwapFlg) ? 0 : 1;
}	//FlipScreen

/**
 * @brief	�t�H���g�T�C�Y�ύX
 *
 * @param	width [����] �t�H���g�̉��T�C�Y(1�`)
 * @param	height [����] �t�H���g�̏c�T�C�Y(1�`)
 */
void SetScreenFontSize(int width, int height)
{
	// �t�H���g�T�C�Y�ύX
	g_FontSizeEx.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	g_FontSizeEx.dwFontSize.X = width;
	g_FontSizeEx.dwFontSize.Y = height;
	if (g_DisplayHandle[0] != NULL) {
		SetCurrentConsoleFontEx(g_DisplayHandle[0], FALSE, &g_FontSizeEx);
	}
	if (g_DisplayHandle[1] != NULL) {
		SetCurrentConsoleFontEx(g_DisplayHandle[1], FALSE, &g_FontSizeEx);
	}
}	//SetScreenFontSize
#endif // !USED2D

/*
* @brief	�R���\�[���̃N���C�A���g�̈�̃T�C�Y�ƃt�H���g�T�C�Y�̎擾
*
* @param	RECT& _r�F���ʂ�����RECT�\���̂̎Q��
*
* @return	RECT& : ���ʂ���ꂽRECT�\���̂̎Q��
*/
RECT& GetConWinSize(RECT& _r)
{
	CONSOLE_FONT_INFOEX fsx;
	fsx.cbSize = sizeof(CONSOLE_FONT_INFOEX);
#ifdef USED2D
	GetCurrentConsoleFontEx(g_DisplayHandleD2D, FALSE, &fsx);
#else
	GetCurrentConsoleFontEx(g_DisplayHandle[g_SwapFlg], FALSE, &fsx);
#endif // USED2D
	GetClientRect(g_hConWnd, &_r);
	_r.left = _r.right - _r.left;
	_r.top = _r.bottom - _r.top;
	_r.right = fsx.dwFontSize.X;
	_r.bottom = fsx.dwFontSize.Y;
	g_ConWinSize = _r;
	return _r;
}

#ifndef USED2D
/**
 * @brief	��ʁi�X�N���[���o�b�t�@�j����
 */
void ClearScreen(void)
{
	DWORD fill_num;
	COORD screen_origin = { 0, 0 };	//����������l

	g_ScreenBufferAttribute = g_ScreenBufferInfoEx.wAttributes;

	FillConsoleOutputAttribute(g_DisplayHandle[g_SwapFlg],
		g_ScreenBufferInfoEx.wAttributes,
		g_ScreenBufferInfoEx.dwSize.X * g_ScreenBufferInfoEx.dwSize.Y,
		screen_origin,
		&fill_num);
	FillConsoleOutputCharacter(g_DisplayHandle[g_SwapFlg],
		TEXT(' '),
		g_ScreenBufferInfoEx.dwSize.X * g_ScreenBufferInfoEx.dwSize.Y,
		screen_origin,
		&fill_num);
	SetCursorPosition(g_ScreenBufferInfoEx.srWindow.Left + 1, g_ScreenBufferInfoEx.srWindow.Top + 1);
	//�摜�p�o�b�t�@����
	ClearScreenBuffer(0);	//�X�N���[���o�b�t�@(16�F)����
	ClearFrameBuffer();		//�t���[���o�b�t�@(16�F)����
	ClearFrameBufferFull();	//�t���[���o�b�t�@(24bit�t���J���[)����
}
#endif // !USED2D

//================================================================
// �E�B���h�E
//================================================================
#ifdef USED2D
LONG_PTR FixWin(void)
{
	//�E�B���h�E�T�C�Y�ύX�֎~
	//HWND hCon = GetConsoleWindow();
	LONG_PTR lastStylePtr = GetWindowLongPtr(g_hConWnd, GWL_STYLE);
	LONG_PTR lStylePtr = lastStylePtr;
	lStylePtr &= ~(WS_THICKFRAME | WS_HSCROLL | WS_VSCROLL);	//'~'�r�b�g���Ƃ̔��](�P�̕␔)
	lStylePtr = SetWindowLongPtr(g_hConWnd, GWL_STYLE, lStylePtr);
	//SetWindowPos(hCon, NULL, 0, 0, frmb.width + 20, frmb.height, SWP_NOSIZE | SWP_NOZORDER);
	return lastStylePtr;
}	//FixWin
/**
* @brief	�E�B���h�E�T�C�Y���Œ肷��
*
* @param	int _x,int _y	�\���ʒu�̎w��
*
* @return	LONG	�ύX�O�̏�Ԃ�Ԃ�
*/
LONG_PTR FixWin(int _x, int _y)
{
	//�E�B���h�E�T�C�Y�ύX�֎~
	//HWND hCon = GetConsoleWindow();
	LONG_PTR lastStylePtr = GetWindowLong(g_hConWnd, GWL_STYLE);
	LONG_PTR lStyle = lastStylePtr;
	lStyle &= ~(WS_THICKFRAME | WS_HSCROLL | WS_VSCROLL);	//'~'�r�b�g���Ƃ̔��](�P�̕␔)
	lStyle = SetWindowLongPtr(g_hConWnd, GWL_STYLE, lStyle);
	//SWP_NOSIZE���w�肵�Ă���̂ŁA���W(_x,_y)�݂̂��ɕύX�����B
	SetWindowPos(g_hConWnd, NULL, _x, _y, -1, -1, SWP_NOSIZE | SWP_NOZORDER);
	return lastStylePtr;
}	//FixWin
#else
/**
* @brief	�E�B���h�E�T�C�Y���Œ肷��
*
* @return	LONG	�ύX�O�̏�Ԃ�Ԃ�
*/
LONG FixWin(void)
{
	//�E�B���h�E�T�C�Y�ύX�֎~
	//HWND hCon = GetConsoleWindow();
	LONG lastStyle = GetWindowLong(g_hConWnd, GWL_STYLE);
	LONG lStyle = lastStyle;
	lStyle &= ~(WS_THICKFRAME | WS_HSCROLL | WS_VSCROLL);	//'~'�r�b�g���Ƃ̔��](�P�̕␔)
	lStyle = SetWindowLong(g_hConWnd, GWL_STYLE, lStyle);
	//SetWindowPos(hCon, NULL, 0, 0, frmb.width + 20, frmb.height, SWP_NOSIZE | SWP_NOZORDER);
	return lastStyle;
}	//FixWin
/**
* @brief	�E�B���h�E�T�C�Y���Œ肷��
*
* @param	int _x,int _y	�\���ʒu�̎w��
*
* @return	LONG	�ύX�O�̏�Ԃ�Ԃ�
*/
LONG FixWin(int _x, int _y)
{
	//�E�B���h�E�T�C�Y�ύX�֎~
	//HWND hCon = GetConsoleWindow();
	LONG lastStyle = GetWindowLong(g_hConWnd, GWL_STYLE);
	LONG lStyle = lastStyle;
	lStyle &= ~(WS_THICKFRAME | WS_HSCROLL | WS_VSCROLL);	//'~'�r�b�g���Ƃ̔��](�P�̕␔)
	lStyle = SetWindowLong(g_hConWnd, GWL_STYLE, lStyle);
	//SWP_NOSIZE���w�肵�Ă���̂ŁA���W(_x,_y)�݂̂��ɕύX�����B
	SetWindowPos(g_hConWnd, NULL, _x, _y, -1, -1, SWP_NOSIZE | SWP_NOZORDER);
	return lastStyle;
}	//FixWin
#endif	//USED2D

/**
* @brief	���݂̃f�B�X�v���C�n���h�����擾����B
*
* @return	HANDLE	���݂̃f�B�X�v���C�n���h��
*/
HANDLE GetCurrentHandle(void) {
#ifdef USED2D
	return g_DisplayHandleD2D;
#else
	return g_DisplayHandle[g_SwapFlg];
#endif // USED2D
}

/**
* @brief	�R���\�[���E�B���h�E�̃^�C�g���o�[�Ƀe�L�X�g��ݒ�
*
* @param	title [����] �E�B���h�E�^�C�g���ɕ\������e�L�X�g
*/
void SetCaption(const char* title)
{
	SetConsoleTitleA(title);
}

/**
* @brief	�R���\�[���E�B���h�E�̃^�C�g���o�[�ɏ����w�肵�ăe�L�X�g��ݒ�
*
* @param	const char *_format�F�����w�蕶����
* @param	...�F�ϒ�����
*/
void SetCaptionF(const char* _format, ...)
{
	va_list ap;
	va_start(ap, _format);
	size_t length = _vscprintf(_format, ap) + 1;	//'\0'�܂܂Ȃ��̂Ł{�P���Ă���
	char* buf = (char*)_malloca(length);
	vsprintf_s(buf, length, _format, ap);
	SetConsoleTitleA(buf);
	va_end(ap);
}	//SetCaptionFA

/**
* @brief	�R���\�[���E�B���h�E�̃^�C�g���o�[�ɕ\�������e�L�X�g���擾
*
* @param	title [�o��] ���݂̃E�B���h�E�^�C�g���̃e�L�X�g
* @param	len [����] �E�B���h�E�^�C�g���̕�����
*
* @retval	��0	���݂̃E�B���h�E�^�C�g���̕�����
* @retval	0	�G���[
*/
int GetCaption(char* title, int len)
{
	return GetConsoleTitleA(title, len);
}	//GetCaption

#ifdef USED2D

//================================================================
//�J�[�\��
//================================================================
/**
 * @brief	���������̃J�[�\���ʒu���擾
 *
 * @return	���݂̃J�[�\���ʒu��X���W(1�`)
 */
int GetCursorX(void)
{
	//return g_ScreenBufferInfoEx.dwCursorPosition.X - g_ScreenBufferInfoEx.srWindow.Left + 1;
	return	g_CursorPosD2D.X + 1;
}	//GetCursorX

/**
 * @brief	���������̃J�[�\���ʒu���擾
 *
 * @return	���݂̃J�[�\���ʒu��Y���W(1�`)
 */
int GetCursorY(void)
{
	//return g_ScreenBufferInfoEx.dwCursorPosition.Y - g_ScreenBufferInfoEx.srWindow.Top + 1;
	return	g_CursorPosD2D.Y + 1;
}	//GetCursorY

/**
 * @brief	�J�[�\���ʒu�̈ړ�
 *
 * @param	x [����] X���W(1�`)
 * @param	y [����] Y���W(1�`)
 */
void SetCursorPosition(int _csr_x, int _csr_y)
{
	g_CursorPosD2D.X = _csr_x - 1;
	g_CursorPosD2D.Y = _csr_y - 1;
}	//SetCursorPosition

/**
 * @brief	�J�[�\���^�C�v�ݒ�
 *
 * @param	type [����]\n
 *						NOCURSOR �J�[�\���\���Ȃ�\n
 *						SOLIDCURSOR (��Ή�)\n
 *						NORMALCURSOR �J�[�\���̒ʏ�\��\n
 */
void SetCursorType(int type)
{
}	//SetCursorType
#else

//================================================================
//�J�[�\��
//================================================================
/**
 * @brief	���������̃J�[�\���ʒu���擾
 *
 * @return	���݂̃J�[�\���ʒu��X���W(1�`)
 */
int GetCursorX(void)
{
	return g_ScreenBufferInfoEx.dwCursorPosition.X - g_ScreenBufferInfoEx.srWindow.Left + 1;
}	//GetCursorX

/**
 * @brief	���������̃J�[�\���ʒu���擾
 *
 * @return	���݂̃J�[�\���ʒu��Y���W(1�`)
 */
int GetCursorY(void)
{
	return g_ScreenBufferInfoEx.dwCursorPosition.Y - g_ScreenBufferInfoEx.srWindow.Top + 1;
}	//GetCursorY

/**
 * @brief	�J�[�\���ʒu�̈ړ�
 *
 * @param	x [����] X���W(1�`)
 * @param	y [����] Y���W(1�`)
 */
void SetCursorPosition(int x, int y)
{
	COORD lc;

	lc.X = x - 1;
	lc.Y = g_ScreenBufferInfoEx.srWindow.Top + y - 1;

	SetConsoleCursorPosition(g_DisplayHandle[g_SwapFlg], lc);
}	//SetCursorPosition

/**
 * @brief	�J�[�\���^�C�v�ݒ�
 *
 * @param	type [����]\n
 *						NOCURSOR �J�[�\���\���Ȃ�\n
 *						SOLIDCURSOR (��Ή�)\n
 *						NORMALCURSOR �J�[�\���̒ʏ�\��\n
 */
void SetCursorType(int type)
{
	CONSOLE_CURSOR_INFO	cursor_info;
	int size = -1;

	if (size < 0) {
		if (GetConsoleCursorInfo(g_DisplayHandle[g_SwapFlg], &cursor_info)) {
			size = (int)cursor_info.dwSize;
		}
		else {
			size = 25;
		}
	}
	cursor_info.dwSize = (type < NORMALCURSOR) ? 100 : size;
	cursor_info.bVisible = (type != NOCURSOR);
	SetConsoleCursorInfo(g_DisplayHandle[g_SwapFlg], &cursor_info);
}	//SetCursorType
#endif // USED2D

/**
* @brief	�}�E�X���W�̎擾
*
* @param	POINT* _mp�F���W���󂯎��POINT�\���̂ւ̃|�C���^
*
* @return	POINT�F�}�E�X�̍��W�i�����P�ʁj
*
* @note		_mp�ɂ̓N���C�A���g���W���Ԃ����
*			�i�|�C���^��NULL�Ȃ�N���C�A���g���W�͊i�[���Ȃ��j
*			�߂�l�͕����P�ʂɊ��Z�������W���Ԃ����
*/
POINT GetCursorMousePos(POINT* _mp)
{
	POINT mpos = { 0,0 };	//�߂�l�p
	GetCursorPos(&mpos);	//���݂̈ʒu
	ScreenToClient(GetConsoleWindow(), &mpos);	//�N���C�A���g���W�֕ϊ�
	if (_mp != NULL) {
		*_mp = mpos;	//�N���C�A���g���W��Ԃ�
	}
	//�����P�ʂ̍��W�ɕϊ�
	mpos.x /= g_FontSizeEx.dwFontSize.X;
	mpos.y /= g_FontSizeEx.dwFontSize.Y;
	return mpos;	//�����P�ʂ̍��W�Ƃ��ĕԂ�
}	//GetCursorMousePos

#ifdef USED2D
//================================================================
//������`��
//================================================================
/**
 * @brief	������̏o�́i�}���`�o�C�g�����p�j
 *
 * @param	_srcbuf [����] �o�͕�����z��̃|�C���^
 * @param	_size [����] �o�͕�����
 */
void PrintStringA(const char* _srcbuf, int _size)
{
	//�\�����������w�蕶����̃T�C�Y���I�[�o�[���Ă�����␳����
	int src_len = (int)strlen(_srcbuf);
	if (_size < 0) {
		_size = src_len;
	}
	else if (_size > src_len) {
		_size = (int)strlen(_srcbuf);
	}
	g_PrintStringCompatibleMode = true;
	//void WriteTextA(int _xp, int _yp, const char* _text, double _scale, D2D1::ColorF _fgc, D2D1::ColorF _bgc, bool _new_line)
	WriteTextA(g_CursorPosD2D.X, g_CursorPosD2D.Y, _srcbuf, 1.0f, D2D1::ColorF(1, 1, 1, 1), D2D1::ColorF(0, 0, 0, 0), false);
}	//PrintStringA

/**
 * @brief	������̏o�́iUnicode�����p�j
 *
 * @param	_srcbuf [����] �o�͕�����z��̃|�C���^
 * @param	_size [����] �o�͕�����
 */
void PrintStringW(const wchar_t* _srcbuf, int _size)
{
	//�\�����������w�蕶����̃T�C�Y���I�[�o�[���Ă�����␳����
	int src_len = (int)wcslen(_srcbuf);
	if (_size < 0) {
		_size = src_len;
	}
	else if (_size > src_len) {
		_size = (int)wcslen(_srcbuf);
	}
	g_PrintStringCompatibleMode = true;
	WriteTextW(g_CursorPosD2D.X, g_CursorPosD2D.Y, _srcbuf, 1.0f, D2D1::ColorF(1, 1, 1, 1), D2D1::ColorF(0, 0, 0, 0), false);
}	//PrintStringW

#ifdef UNICODE
	/**
	 * @brief	������̏o��(�}���`�o�C�g��Unicode�ϊ���)
	 *
	 * @param	_src [����] �o�͕�����z��̃|�C���^
	 * @param	_size [����] �o�͕�����
	 */
void PrintString(const char* _src, int _size)
{
	//�w�肳�ꂽ�}���`�o�C�g�����S�Ă����C�h����(Unicode����)�ϊ������ꍇ��
	//�K�v�ȃo�b�t�@�[�T�C�Y (�I�[�� null �������܂�) �𕶎��P�ʂŎZ�o���A
	//���̕��������̃o�b�t�@�[���m�ۂ���B
	int wc_src_siz = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, _src, -1, NULL, 0);
	size_t wc_src_bytes = (wc_src_siz * sizeof(wchar_t));	//Unicode�������Z�̃o�C�g���B
	wchar_t* wc_src = (wchar_t*)_malloca(wc_src_bytes);	//�X�^�b�N��Ɋm�ہFfree�s�v
	memset(wc_src, 0, wc_src_bytes);	//�O�N���A�F�������݂��r���܂łł�'\0'�I�[������ɂȂ�B
	//�w��T�C�Y�����̕��������I�[�o�[���Ă���ꍇ�̕␳
	if ((int)strlen(_src) < _size) {
		_size = (-1);	//(-1)�w���'\0'�܂ŕϊ��B
	}
	//�w��T�C�Y���ϊ�����i_size == (-1))�Ȃ�'\0'�܂őS�ĕϊ�����j
	//�߂�l�͕ϊ�����(�o�b�t�@�ɏ������܂ꂽ)���������Ԃ�B
	//�y���z(-1)�w��ŕϊ������ꍇ�A�߂�l��'\0'���܂ޕ������ɂȂ�B
	int disp_siz = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, _src, _size, wc_src, wc_src_siz);
	//disp_siz��'\0'���܂ޕ�������������Ȃ��̂ŁA�ϊ������������ɂȂ�l�ɍČv�Z����B
	disp_siz = (int)wcslen(wc_src);
	//DWORD num;	//���ۂɏ������܂ꂽ���������󂯎��ϐ�
	//WriteConsoleW(g_DisplayHandle[g_SwapFlg], wc_src, disp_siz, &num, NULL);
	g_PrintStringCompatibleMode = true;
	WriteTextW(g_CursorPosD2D.X, g_CursorPosD2D.Y, wc_src, 1.0f, D2D1::ColorF(1, 1, 1, 1), D2D1::ColorF(0, 0, 0, 0), false);
}	//PrintString
#endif	//UNICODE

//----------------
//�����S��
//----------------
void SetHighVideoColor(void) {}
void SetLowVideoColor(void) {}
void SetNormalVideoColor(void) {}
void SetTextBackColor(int color) {}
void SetConsoleTextColor(int color) {}
void SetTextAttribute(int attribute) {}
//----------------
//�s����
//----------------
void ClearLine(void) {}
void InsertLine(void) {}
void DeleteLine(void) {}

#else

//================================================================
//������`��
//================================================================
/**
 * @brief	������̏o�́i�}���`�o�C�g�����p�j
 *
 * @param	_srcbuf [����] �o�͕�����z��̃|�C���^
 * @param	_size [����] �o�͕�����
 */
void PrintStringA(const char* _srcbuf, int _size)
{
	//�\�����������w�蕶����̃T�C�Y���I�[�o�[���Ă�����␳����
	int src_len = (int)strlen(_srcbuf);
	if (_size < 0) {
		_size = src_len;
	}
	else if (_size > src_len) {
		_size = (int)strlen(_srcbuf);
	}
	DWORD num;
	WriteConsoleA(g_DisplayHandle[g_SwapFlg], _srcbuf, _size, &num, NULL);
}	//PrintStringA

/**
 * @brief	������̏o�́iUnicode�����p�j
 *
 * @param	_srcbuf [����] �o�͕�����z��̃|�C���^
 * @param	_size [����] �o�͕�����
 */
void PrintStringW(const wchar_t* _srcbuf, int _size)
{
	//�\�����������w�蕶����̃T�C�Y���I�[�o�[���Ă�����␳����
	int src_len = (int)wcslen(_srcbuf);
	if (_size < 0) {
		_size = src_len;
	}
	else if (_size > src_len) {
		_size = (int)wcslen(_srcbuf);
	}
	DWORD num;
	WriteConsoleW(g_DisplayHandle[g_SwapFlg], _srcbuf, _size, &num, NULL);
}	//PrintStringW

#ifdef UNICODE
	/**
	 * @brief	������̏o��(�}���`�o�C�g��Unicode�ϊ���)
	 *
	 * @param	_src [����] �o�͕�����z��̃|�C���^
	 * @param	_size [����] �o�͕�����
	 */
void PrintString(const char* _src, int _size)
{
#if true

	//�w�肳�ꂽ�}���`�o�C�g�����S�Ă����C�h����(Unicode����)�ϊ������ꍇ��
	//�K�v�ȃo�b�t�@�[�T�C�Y (�I�[�� null �������܂�) �𕶎��P�ʂŎZ�o���A
	//���̕��������̃o�b�t�@�[���m�ۂ���B
	int wc_src_siz = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, _src, -1, NULL, 0);
	size_t wc_src_bytes = (wc_src_siz * sizeof(wchar_t));	//Unicode�������Z�̃o�C�g���B
	wchar_t* wc_src = (wchar_t*)_malloca(wc_src_bytes);	//�X�^�b�N��Ɋm�ہFfree�s�v
	memset(wc_src, 0, wc_src_bytes);	//�O�N���A�F�������݂��r���܂łł�'\0'�I�[������ɂȂ�B
	//�w��T�C�Y�����̕��������I�[�o�[���Ă���ꍇ�̕␳
	if ((int)strlen(_src) < _size) {
		_size = (-1);	//(-1)�w���'\0'�܂ŕϊ��B
	}
	//�w��T�C�Y���ϊ�����i_size == (-1))�Ȃ�'\0'�܂őS�ĕϊ�����j
	//�߂�l�͕ϊ�����(�o�b�t�@�ɏ������܂ꂽ)���������Ԃ�B
	//�y���z(-1)�w��ŕϊ������ꍇ�A�߂�l��'\0'���܂ޕ������ɂȂ�B
	int disp_siz = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, _src, _size, wc_src, wc_src_siz);
	//disp_siz��'\0'���܂ޕ�������������Ȃ��̂ŁA�ϊ������������ɂȂ�l�ɍČv�Z����B
	disp_siz = (int)wcslen(wc_src);
	DWORD num;	//���ۂɏ������܂ꂽ���������󂯎��ϐ�
	WriteConsoleW(g_DisplayHandle[g_SwapFlg], wc_src, disp_siz, &num, NULL);
#else
	DWORD num;
	WCHAR wide_char[256];
	memset(wide_char, 0, sizeof(wide_char));
	//MB_COMPOSITE�ˑ��_�E�����_�������Q�����ɕϊ������F��j�p�˃p�A�΁˂�
	int ret_val = MultiByteToWideChar(CP_ACP, MB_COMPOSITE, _src, _size, wide_char, _size);
	WriteConsoleW(g_DisplayHandle[g_SwapFlg], wide_char, ret_val, &num, NULL);
#endif // true
}	//PrintString
#endif	//UNICODE

//----------------
//�����S��
//----------------
/**
 * @brief	�����F���P�x��
 */
void SetHighVideoColor(void)
{
	SetConsoleTextAttribute(g_DisplayHandle[g_SwapFlg], g_ScreenBufferInfoEx.wAttributes |= FOREGROUND_INTENSITY);
}	//SetHighVideoColor

/**
 * @brief	�����F��P�x��
 */
void SetLowVideoColor(void)
{
	SetConsoleTextAttribute(g_DisplayHandle[g_SwapFlg], g_ScreenBufferInfoEx.wAttributes &= ~FOREGROUND_INTENSITY);
}	//SetLowVideoColor

/**
 * @brief	���蕶���F�ݒ�
 */
void SetNormalVideoColor(void)
{
	SetConsoleTextAttribute(g_DisplayHandle[g_SwapFlg], LIGHTGRAY);
}	//SetNormalVideoColor

//----------------
//���������w��
//----------------
/**
 * @brief	�����w�i�F�ݒ�
 *
 * @param	color [����] �����w�i�F
 * @note
 *	�w�i�F��enum COLORS���Q�Ƃ���
 */
void SetTextBackColor(int color)
{
	g_ScreenBufferInfoEx.wAttributes &= ~0x00f0;
	//g_ScreenBufferInfoEx.wAttributes |= ((color & 0x07) << 4);
	g_ScreenBufferInfoEx.wAttributes |= ((color & 0x0F) << 4);
	SetConsoleTextAttribute(g_DisplayHandle[g_SwapFlg], g_ScreenBufferInfoEx.wAttributes);
}	//SetTextBackColor

/**
 * @brief	�����F�ݒ�
 *
 * @param	color [����] �����F
 * @note
 *	�����F��enum COLORS���Q�Ƃ���
 */
void SetConsoleTextColor(int color)
{
	g_ScreenBufferInfoEx.wAttributes &= ~0x000f;
	g_ScreenBufferInfoEx.wAttributes |= (color & 0x0f);
	SetConsoleTextAttribute(g_DisplayHandle[g_SwapFlg], g_ScreenBufferInfoEx.wAttributes);
}	//SetConsoleTextColor

/**
 * @brief	�����F�w�i�F�����ݒ�
 *
 * @param	attribute [����] �����w�i���
 * @note
 *	�ȉ��̊e�ݒ�l���r�b�g����OR���Z��p���Ĉ����Ɏw�肷��
 *	  FOREGROUND_BLUE	   0x0001 // text color contains blue.
 *	  FOREGROUND_GREEN	   0x0002 // text color contains green.
 *	  FOREGROUND_RED	   0x0004 // text color contains red.
 *	  FOREGROUND_INTENSITY 0x0008 // text color is intensified.
 *	  BACKGROUND_BLUE	   0x0010 // background color contains blue.
 *	  BACKGROUND_GREEN	   0x0020 // background color contains green.
 *	  BACKGROUND_RED	   0x0040 // background color contains red.
 *	  BACKGROUND_INTENSITY 0x0080 // background color is intensified.
 *	  COMMON_LVB_LEADING_BYTE	 0x0100 // Leading Byte of DBCS
 *	  COMMON_LVB_TRAILING_BYTE	 0x0200 // Trailing Byte of DBCS
 *	  COMMON_LVB_GRID_HORIZONTAL 0x0400 // DBCS: Grid attribute: top horizontal.
 *	  COMMON_LVB_GRID_LVERTICAL  0x0800 // DBCS: Grid attribute: left vertical.
 *	  COMMON_LVB_GRID_RVERTICAL  0x1000 // DBCS: Grid attribute: right vertical.
 *	  COMMON_LVB_REVERSE_VIDEO	 0x4000 // DBCS: Reverse fore/back ground attribute.
 *	  COMMON_LVB_UNDERSCORE 	 0x8000 // DBCS: Underscore.
 *	  COMMON_LVB_SBCSDBCS		 0x0300 // SBCS or DBCS flag.
 */
void SetTextAttribute(int attribute)
{
	SetConsoleTextAttribute(g_DisplayHandle[g_SwapFlg], attribute);
}	//SetTextAttribute
//----------------
//�s����
//----------------
/**
 * @brief	�s���܂ŏ���
 */
void ClearLine(void)
{
	DWORD fill_num;

	FillConsoleOutputAttribute(g_DisplayHandle[g_SwapFlg],
		g_ScreenBufferInfoEx.wAttributes,
		g_ScreenBufferInfoEx.srWindow.Right - g_ScreenBufferInfoEx.dwCursorPosition.X + 1,
		g_ScreenBufferInfoEx.dwCursorPosition,
		&fill_num);
	FillConsoleOutputCharacter(g_DisplayHandle[g_SwapFlg],
		TEXT(' '),
		g_ScreenBufferInfoEx.srWindow.Right - g_ScreenBufferInfoEx.dwCursorPosition.X + 1,
		g_ScreenBufferInfoEx.dwCursorPosition,
		&fill_num);
}	//ClearLine

/**
 * @brief	���ݍs�ɑ}��
 */
void InsertLine(void)
{
	COORD	lc;
	DWORD	len;
	DWORD	num;
	LPTSTR	psz;
	LPWORD	pw;

	lc.X = g_ScreenBufferInfoEx.srWindow.Left;
	len = g_ScreenBufferInfoEx.srWindow.Right - g_ScreenBufferInfoEx.srWindow.Left + 1;
	psz = (LPTSTR)_malloca(len * sizeof(TCHAR));
	pw = (LPWORD)_malloca(len * sizeof(WORD));
	for (lc.Y = g_ScreenBufferInfoEx.srWindow.Bottom; lc.Y > g_ScreenBufferInfoEx.dwCursorPosition.Y; lc.Y--) {
		lc.Y--;
		ReadConsoleOutputAttribute(g_DisplayHandle[g_SwapFlg], pw, len, lc, &num);
		ReadConsoleOutputCharacter(g_DisplayHandle[g_SwapFlg], psz, len, lc, &num);
		lc.Y++;
		WriteConsoleOutputAttribute(g_DisplayHandle[g_SwapFlg], pw, len, lc, &num);
		WriteConsoleOutputCharacter(g_DisplayHandle[g_SwapFlg], psz, len, lc, &num);
	}
	FillConsoleOutputAttribute(g_DisplayHandle[g_SwapFlg], g_ScreenBufferInfoEx.wAttributes, len, lc, &num);
	FillConsoleOutputCharacter(g_DisplayHandle[g_SwapFlg], TEXT(' '), len, lc, &num);
}	//InsertLine

/**
 * @brief	���ݍs�̍폜
 */
void DeleteLine(void)
{
	DWORD	read_num;
	DWORD	write_num;
	DWORD	fill_num;
	COORD	calc_coord;
	DWORD	line_len;
	LPTSTR	receive_character;
	LPWORD	receive_attribute;

	calc_coord.X = g_ScreenBufferInfoEx.srWindow.Left;
	line_len = g_ScreenBufferInfoEx.srWindow.Right - g_ScreenBufferInfoEx.srWindow.Left + 1;
	receive_character = (LPTSTR)_malloca(line_len * sizeof(TCHAR));
	receive_attribute = (LPWORD)_malloca(line_len * sizeof(WORD));

	for (calc_coord.Y = g_ScreenBufferInfoEx.dwCursorPosition.Y; calc_coord.Y < g_ScreenBufferInfoEx.srWindow.Bottom; calc_coord.Y++) {
		calc_coord.Y++;
		ReadConsoleOutputAttribute(g_DisplayHandle[g_SwapFlg], receive_attribute, line_len, calc_coord, &read_num);
		ReadConsoleOutputCharacter(g_DisplayHandle[g_SwapFlg], receive_character, line_len, calc_coord, &read_num);
		calc_coord.Y--;
		WriteConsoleOutputAttribute(g_DisplayHandle[g_SwapFlg], receive_attribute, line_len, calc_coord, &write_num);
		WriteConsoleOutputCharacter(g_DisplayHandle[g_SwapFlg], receive_character, line_len, calc_coord, &write_num);
	}

	FillConsoleOutputAttribute(g_DisplayHandle[g_SwapFlg], g_ScreenBufferAttribute, line_len, calc_coord, &fill_num);
	FillConsoleOutputCharacter(g_DisplayHandle[g_SwapFlg], TEXT(' '), line_len, calc_coord, &fill_num);
}	//DeleteLine
#endif // !USED2D


//================================================================
// �g�������񏈗�
//================================================================
/**
* @brief
* ���p������S�p�����ɕϊ�����i�}���`�o�C�g�Łj
*
* @param	const char* _src	�ϊ����ɂȂ镶����i�}���`�o�C�g�����j
*
* @return	char*\n
* �ϊ���̕����񂪓����Ă���o�b�t�@�ւ̃|�C���^�B\n
* �y���z�߂�l��malloc()�����|�C���^��Ԃ��̂ŁA�Ăяo�������ŕK��free()���鎖�B
*/
char* HanToZenA(const char* _src)
{
	//const char* _src = "�ϊ�����abcxyz;*@������1234567890";
	/*
	* ��UUnicode������ɕϊ��������̂�S�p�����ɕϊ����ă}���`�o�C�g������ɖ߂��Ă���B
	*	MultiByteToWideChar()�F�}���`�o�C�g�����񂩂�Unicode������֕ϊ�
	*	 LCMapStringEx()�F���p����S�p�֕ϊ�
	*	 WideCharToMultiByte()�F�}���`�o�C�g�����񂩂�Ubicode������֕ϊ�
	*/
	//---- �}���`�o�C�g�������Unicode������ɕϊ�����
	int wc_count = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, _src, -1, NULL, 0);	//'\0'���܂ޕ��������Ԃ�
	size_t wc_src_bytes = (wc_count * sizeof(wchar_t));
	wchar_t* src_txt = (wchar_t*)_malloca(wc_src_bytes);
	memset(src_txt, 0, wc_src_bytes);
	int disp_siz = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, _src, -1, src_txt, wc_count);
	//---- Unicode������̔��p������S�p�����ɕϊ����� ----
	DWORD flags = LCMAP_FULLWIDTH;		//�S�p�����ɕϊ�
	//	DWORD flags = LCMAP_HALFWIDTH;		//���p�����ɕϊ�
	//	DWORD flags = LCMAP_HIRAGANA;		//�Ђ炪�Ȃɕϊ�
	//	DWORD flags = LCMAP_KATAKANA;		//�J�^�J�i�ɕϊ�
	int dest_size = LCMapStringEx(LOCALE_NAME_USER_DEFAULT, flags, src_txt, -1, NULL, 0, NULL, NULL, 0);
	wchar_t* dest_buf = (wchar_t*)_malloca(dest_size * sizeof(wchar_t));
	memset(dest_buf, 0, dest_size * sizeof(wchar_t));
	int output_size = LCMapStringEx(LOCALE_NAME_USER_DEFAULT, flags, src_txt, -1, dest_buf, dest_size, NULL, NULL, 0);
	//---- Unicode��������}���`�o�C�g������ɕϊ����� ----
	//���������v��
	int mb_bytes = WideCharToMultiByte(CP_ACP, 0, dest_buf, -1, NULL, 0, NULL, NULL);	//'\0'�܂ރT�C�Y���߂�
	//�ϊ���o�b�t�@���m�ہB
	char* mb_dest_buff = (char*)calloc(mb_bytes, sizeof(char));
	memset(mb_dest_buff, 0, mb_bytes);	//�ϊ���o�b�t�@���O�ŏ�����
	//�ϊ�
	int res = WideCharToMultiByte(CP_ACP, 0, dest_buf, -1, mb_dest_buff, mb_bytes, NULL, NULL);
	return mb_dest_buff;	//�ϊ��ςݕ�����o�b�t�@(�y���z���I�������m�ۂ����|�C���^)��Ԃ��B
}	//HanToZenA

/**
* @brief
* ���p������S�p�����ɕϊ�����iUnicode�Łj
*
* @param	const wchar_t* _src	�ϊ����ɂȂ镶����iUnicode�����j
*
* @return	wchar_t*\n
* �ϊ���̕����񂪓����Ă���o�b�t�@�ւ̃|�C���^�B\n
* �y���z�߂�l��malloc()�����|�C���^��Ԃ��̂ŁA�Ăяo�������ŕK��free()���鎖�B
*/
wchar_t* HanToZenW(const wchar_t* _src)
{
	//---- Unicode������̔��p������S�p�����ɕϊ����� ----
	DWORD flags = LCMAP_FULLWIDTH;		//�S�p�����ɕϊ�
	//	DWORD flags = LCMAP_HALFWIDTH;		//���p�����ɕϊ�
	//	DWORD flags = LCMAP_HIRAGANA;		//�Ђ炪�Ȃɕϊ�
	//	DWORD flags = LCMAP_KATAKANA;		//�J�^�J�i�ɕϊ�
	//���������v��
	int dest_size = LCMapStringEx(LOCALE_NAME_USER_DEFAULT, flags, _src, -1, NULL, 0, NULL, NULL, 0);
	//�ϊ���o�b�t�@���m�ہB
	wchar_t* dest_buf = (wchar_t*)calloc(dest_size, sizeof(wchar_t));
	memset(dest_buf, 0, dest_size * sizeof(wchar_t));	//�ϊ���o�b�t�@���O�ŏ�����
	//�ϊ�
	int output_size = LCMapStringEx(LOCALE_NAME_USER_DEFAULT, flags, _src, -1, dest_buf, dest_size, NULL, NULL, 0);
	return dest_buf;	//�ϊ��ςݕ�����o�b�t�@(�y���z���I�������m�ۂ����|�C���^)��Ԃ��B
}	//HanToZenW

/**
* @brief
* �����w��t������`��i�������X�g�Łj�i�}���`�o�C�g�����p�j
*
* @param	bool _zenkaku	true�Ȃ�S�Ă̕�����S�p�ŏo��
* ��j
*	false:"�S�p%d",99 -> "�S�p99"
*	true:"�S�p%d",99 -> "�S�p�X�X"
* @param	const char* _format	�����w�蕶����i�}���`�o�C�g�����j
* @param	va_list _ap	�C�ӂ̉ϒ�����
*/
void VPrintStringFA(bool _zenkaku, const char* _format, va_list _ap)
{
	size_t length = _vscprintf(_format, _ap) + 1;	//'\0'�܂܂Ȃ��̂Ł{�P���Ă���
	char* buf = (char*)_malloca(length);
	vsprintf_s(buf, length, _format, _ap);
	if (_zenkaku == true) {
		char* p = HanToZen(buf);
		PrintString(p, -1);
		free(p);
	}
	else {
		PrintString(buf, -1);
	}
}	//VPrintStringFA

/**
* @brief
* �����w��t������`��i�������X�g�Łj�iUnicode�����p�j
*
* @param	bool _zenkaku	true�Ȃ�S�Ă̕�����S�p�ŏo��
* ��j
*	false:"�S�p%d",99 -> "�S�p99"
*	true:"�S�p%d",99 -> "�S�p�X�X"
* @param	const wchar_t* _format	�����w�蕶����iUnicode�����j
* @param	va_list _ap	�C�ӂ̉ϒ�����
*/
void VPrintStringFW(bool _zenkaku, const wchar_t* _format, va_list _ap)
{
	size_t length = _vscwprintf(_format, _ap) + 1;	//'\0'�܂܂Ȃ��̂Ł{�P���Ă���
	wchar_t* buf = (wchar_t*)_malloca(length * sizeof(wchar_t));
	vswprintf_s(buf, length, _format, _ap);
	if (_zenkaku == true) {
		wchar_t* p = HanToZenW(buf);
		PrintStringW(p, -1);
		free(p);
	}
	else {
		PrintStringW(buf, -1);
	}
}	//VPrintStringFW

/**
* @brief
* �����w��t������`��i�}���`�o�C�g�����p�j
*
* @param	bool _zenkaku	true���w�肷��ƁA�S�Ă̕���(��' '���܂�)��S�p�ɕϊ����ĕ\������B\n
* ��j\n
*	false:"�S�p%d",99 -> "�S�p99"\n
*	 true:"�S�p%d",99 -> "�S�p�X�X"\n
*	false:"�S�p%s","��1��2��"-> "�S�p��1��2��"\n
*	 true:"�S�p%s","��1��2��"-> "�S�p���P���Q��"\n
*	false:"�S�p%-4s","�X"-> "�S�p�X  "\n
*	 true:"�S�p%-4s","�X"-> "�S�p�X�@�@"\n
* �y���z������w��ɕ��w�肪�����Ă��鎞�A�S�p���p������̕����񂾂ƈʒu�����킹�ɂ����B\n
*		�ϊ��������l�̏ꍇ�A�S�p/���p�̋�ʂ͖������A\n
*		�ϊ�����������̏ꍇ�A�S�p/���p��������ꍇ������̂ŁA�ʒu���킹������Ȃ�B\n
* @param	const char* _format	�����w�蕶����i�}���`�o�C�g�����j
* @param	...	�C�ӂ̉ϒ�����
*
* @note
* ��_zenkaku�ȍ~��printf()�Ɠ����d�l�B
*/
void PrintStringFA(bool _zenkaku, const char* _format, ...)
{
	va_list ap;
	va_start(ap, _format);
	VPrintStringFA(_zenkaku, _format, ap);
	va_end(ap);
}	//PrintStringFA

/**
* @brief
* �����w��t������`��iUnicode�����p�j
*
* @param	bool _zenkaku	true���w�肷��ƁA�S�Ă̕���(��' '���܂�)��S�p�ɕϊ����ĕ\������B\n
* ��j\n
*	false:"�S�p%d",99 -> "�S�p99"\n
*	 true:"�S�p%d",99 -> "�S�p�X�X"\n
*	false:"�S�p%s","��1��2��"-> "�S�p��1��2��"\n
*	 true:"�S�p%s","��1��2��"-> "�S�p���P���Q��"\n
*	false:"�S�p%-4s","�X"-> "�S�p�X  "\n
*	 true:"�S�p%-4s","�X"-> "�S�p�X�@�@"\n
* �y���z������w��ɕ��w�肪�����Ă��鎞�A�S�p���p������̕����񂾂ƈʒu�����킹�ɂ����B\n
*		�ϊ��������l�̏ꍇ�A�S�p/���p�̋�ʂ͖������A\n
*		�ϊ�����������̏ꍇ�A�S�p/���p��������ꍇ������̂ŁA�ʒu���킹������Ȃ�B\n
* @param	const wchar_t* _format	�����w�蕶����iUnicode�����j
* @param	...	�C�ӂ̉ϒ�����
*
* @note
* ��_zenkaku�ȍ~��printf()�Ɠ����d�l�B
*/
void PrintStringFW(bool _zenkaku, const wchar_t* _format, ...)
{
	va_list ap;
	va_start(ap, _format);
	VPrintStringFW(_zenkaku, _format, ap);
	va_end(ap);
}	//PrintStringFW

/**
* @brief
* �ʒu�w�聕�����w��t������`��i�}���`�o�C�g�����p�j
*
* @param	int _xp	���W�w�w��i�P�I���W���j
* @param	int _yp	���W�x�w��i�P�I���W���j
* @param	bool _zenkaku	true���w�肷��ƁA�S�Ă̕���(��' '���܂�)��S�p�ɕϊ����ĕ\������B
* @param	const wchar_t* _format	�����w�蕶����i�}���`�o�C�g�����j
* @param	...	�C�ӂ̉ϒ�����\n
*
* @note
* ��_zenkaku�ȍ~��printf()�Ɠ����d�l�B
*/
void PosPrintStringFA(int _xp, int _yp, bool _zenkaku, const char* _format, ...)
{
	SetCursorPosition(_xp, _yp);
	va_list ap;
	va_start(ap, _format);
	VPrintStringFA(_zenkaku, _format, ap);
	va_end(ap);
}	//PosPrintStringFA

/**
* @brief
* �ʒu�w�聕�����w��t������`��iUnicode�����p�j
*
* @param	int _xp	���W�w�w��i�P�I���W���j
* @param	int _yp	���W�x�w��i�P�I���W���j
* @param	bool _zenkaku	true���w�肷��ƁA�S�Ă̕���(��' '���܂�)��S�p�ɕϊ����ĕ\������B
* @param	const wchar_t* _format	�����w�蕶����iUnicode�����j
* @param	...	�C�ӂ̉ϒ�����\n
*
* @note
* ��_zenkaku�ȍ~��printf()�Ɠ����d�l�B
*/
void PosPrintStringFW(int _xp, int _yp, bool _zenkaku, const wchar_t* _format, ...)
{
	SetCursorPosition(_xp, _yp);
	va_list ap;
	va_start(ap, _format);
	VPrintStringFW(_zenkaku, _format, ap);
	va_end(ap);
}	//PosPrintStringFW

/**
* @brief	���W�w��i��ʍ������(0,0)���W�j�{�����w��t������`��i�}���`�o�C�g�����p�j
*
* @param	int _x	�\���w���W�i�O�I���W���j
* @param	int _y	�\���x���W�i�O�I���W���j
* @param	bool _zenkaku	true���w�肷��ƁA�S�Ă̕���(��' '���܂�)��S�p�ɕϊ����ĕ\������B
* @param	const char* _format	�����w�蕶����i�}���`�o�C�g�����j
* @param	...	�ϒ�����
*/
void DrawStringFA(int _x, int _y, bool _zenkaku, const char* _format, ...)
{
#ifdef USED2D
	g_CursorPosD2D.X = _x;
	g_CursorPosD2D.Y = _y;
#else
	SetConsoleCursorPosition(g_DisplayHandle[g_SwapFlg], COORD{ (SHORT)_x,(SHORT)(g_ScreenBufferInfoEx.srWindow.Top + _y) });
#endif // USED2D
	va_list ap;
	va_start(ap, _format);
	VPrintStringFA(_zenkaku, _format, ap);
	va_end(ap);
}	//DrawStringFA

/**
* @brief	���W�w��i��ʍ������(0,0)���W�j�{�����w��t������`��iUnicode�����p�j
*
* @param	int _x	�\���w���W�i�O�I���W���j
* @param	int _y	�\���x���W�i�O�I���W���j
* @param	bool _zenkaku	true���w�肷��ƁA�S�Ă̕���(��' '���܂�)��S�p�ɕϊ����ĕ\������B
* @param	const wchar_t* _format	�����w�蕶����iUnicode�����j
* @param	...	�ϒ�������
*/
void DrawStringFW(int _x, int _y, bool _zenkaku, const wchar_t* _format, ...)
{
#ifdef USED2D
	g_CursorPosD2D.X = _x;
	g_CursorPosD2D.Y = _y;
#else
	SetConsoleCursorPosition(g_DisplayHandle[g_SwapFlg], COORD{ (SHORT)_x,(SHORT)(g_ScreenBufferInfoEx.srWindow.Top + _y) });
#endif // USED2D
	va_list ap;
	va_start(ap, _format);
	VPrintStringFW(_zenkaku, _format, ap);
	va_end(ap);
}	//DrawStringFW

//================================================================
// �p���b�g�֌W
//================================================================
/**
* @brief	RGBQUAD:{R,G,B,0}�^��COLORREF:0x00BBGGRR�^�ɕϊ�
*
* @param	RGBQUAD rgb	RGBQUAD�F
*/
COLORREF RGBQtoCREF(RGBQUAD rgb)
{
	return (rgb.rgbRed & 0x0000FF) | ((rgb.rgbGreen << 8) & 0x00FF00) | ((rgb.rgbBlue << 16) & 0xFF0000);
}	//RGBQtoCREF

/**
* @brief	COLORREF:0x00BBGGRR�^��RGBQUAD:{R,G,B,0}�^�ɕϊ�
*
* @param	COLORREF ref	COLORREF�F
*/
RGBQUAD CREFtoRGBQ(COLORREF ref)
{
	RGBQUAD rgb = { (BYTE)((ref & 0x00FF0000) >> 16)/*Blue*/,(BYTE)((ref & 0x0000FF00) >> 8)/*Green*/,(BYTE)(ref & 0x000000FF)/*Red*/,0x00/*Reserved*/ };
	return rgb;
}	//CREFtoRGBQ

/**
* @brief	�p���b�g�ϊ��FRGBQ[16] -> COLORREF[16]
*
* @param	const RGBQUAD* _rgb	�ϊ����P�U�F
* @param	COLORREF* _cref	�ϊ���P�U�F
*/
COLORREF* ConvRGBQtoCREF(const RGBQUAD* _rgb, COLORREF* _cref)
{
	for (int n = 0; n < NUM_PALETTE; n++) {
		_cref[n] = RGBQtoCREF(_rgb[n]);
	}
	return _cref;
}

/**
* @brief	�p���b�g�ϊ��FRGBQ[16] -> COLORREF[16]
*
* @param	const COLORREF* _cref	�ϊ����P�U�F
* @param	RGBQUAD* _rgb	�ϊ���P�U�F
*/
RGBQUAD* ConvCREFtoRGBQ(const COLORREF* _cref, RGBQUAD* _rgb)
{
	for (int n = 0; n < NUM_PALETTE; n++) {
		_rgb[n] = CREFtoRGBQ(_cref[n]);
	}
	return _rgb;
}

/**
* @brief	�p���b�g�̐ݒ�
*
* @param	COLORREF* _pal16�F�p���b�g�f�[�^�ւ̃|�C���^
* @param	int  _p1�F�Z�b�g�������p���b�g�̊J�n�ԍ�
* @param	int  _p2�F�Z�b�g�������p���b�g�̏I���ԍ�
*/
void SetPalette(const COLORREF* _pal16, int _p1, int _p2)
{
	if (_pal16 == NULL) {
		//�p���b�g����
		return;
	}
#ifdef USED2D
	//�l�̕␳
	if (_p1 < 0) { _p1 = 0; }
	if (_p2 < 0) { _p2 = 0; }
	if (_p1 >= NUM_D2D_PAL) { _p1 = NUM_D2D_PAL - 1; }
	if (_p2 >= NUM_D2D_PAL) { _p2 = NUM_D2D_PAL - 1; }
	//_p1 <= _p2�ɂ���
	if (_p1 > _p2) {
		int t = _p1;
		_p1 = _p2;
		_p2 = t;
	}
	for (int n = _p1; n <= _p2; n++) {
		g_PaletteD2D[n].rgbBlue = (_pal16[n] & 0x00FF0000) >> 16;	//00BBGGRR
		g_PaletteD2D[n].rgbGreen = (_pal16[n] & 0x0000FF00) >> 8;	//00BBGGRR
		g_PaletteD2D[n].rgbRed = (_pal16[n] & 0x000000FF);	//00BBGGRR
		g_PaletteD2D[n].rgbReserved = 0;	//00BBGGRR
	}
#else
	//�l�̕␳
	if (_p1 < 0) { _p1 = 0; }
	if (_p2 < 0) { _p2 = 0; }
	if (_p1 >= NUM_PALETTE) { _p1 = NUM_PALETTE - 1; }
	if (_p2 >= NUM_PALETTE) { _p2 = NUM_PALETTE - 1; }
	//_p1 <= _p2�ɂ���
	if (_p1 > _p2) {
		int t = _p1;
		_p1 = _p2;
		_p2 = t;
	}
	//�\���̂̐ݒ�
	g_ScreenBufferInfoEx.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
	//�R���\�[���̃p���b�g�ǂݍ���
	GetConsoleScreenBufferInfoEx(g_DisplayHandle[0], &g_ScreenBufferInfoEx);
	//window�T�C�Y�����Z�b�g���Ȃ��ƁA�����Âω�����H
	//g_ScreenBufferInfoEx.srWindow.Right = g_ScreenBufferInfoEx.dwSize.X;
	//g_ScreenBufferInfoEx.srWindow.Bottom = g_ScreenBufferInfoEx.dwSize.Y;
	g_ScreenBufferInfoEx.srWindow.Right = g_ScreenBufferInfoEx.dwMaximumWindowSize.X;
	g_ScreenBufferInfoEx.srWindow.Bottom = g_ScreenBufferInfoEx.dwMaximumWindowSize.Y;
	//�p���b�g(COLORREF[])��]������
	for (int n = _p1; n <= _p2; n++) {
		g_ScreenBufferInfoEx.ColorTable[n] = _pal16[n];
	}
	//�R���\�[���Ƀp���b�g�𔽉f����
	if (g_DisplayHandle[0] != NULL) {
		SetConsoleScreenBufferInfoEx(g_DisplayHandle[0], &g_ScreenBufferInfoEx);
	}
	if (g_DisplayHandle[1] != NULL) {
		SetConsoleScreenBufferInfoEx(g_DisplayHandle[1], &g_ScreenBufferInfoEx);
	}
#endif // USED2D
}	//SetPalette

/**
* @brief	Bmp����p���b�g�P�U�F�̐ݒ�
*
* @param	Bmp* _pBmp�F�p���b�g�f�[�^�ւ̃|�C���^
*/
void SetPalette(const Bmp* _pBmp)
{
	if (_pBmp->pal == NULL) {
		return;
	}
	if (_pBmp->numpal <= 0) {
		return;
	}
	SetPalette(_pBmp->pal, 0, _pBmp->numpal - 1);
}

//================================================================
// �t���[���o�b�t�@�摜�`��
//================================================================
#ifndef USED2D
/**
* @brief	�X�N���[���o�b�t�@�̏�����
*
* @param	const char _code:�����l���S�Ă̗v�f�����̒l�ŏ����������
*
* @note		�]�����ƂȂ�16�F�X�N���[���o�b�t�@���w��l�Ŗ��߂�
*/
void ClearScreenBuffer(const char _code)
{
	//memset(g_ScreenBuffer4bit, _code, g_ScreenBufferSize.X * g_ScreenBufferSize.Y * sizeof(CHAR_INFO));
	memset(g_ScreenBuffer4bit, _code, g_ScreenBufferSize.X * g_ScreenBufferSize.Y * sizeof(WORD));
}	//ClearScreenBuffer

/**
* @brief	�ꊇ�]���p�o�b�t�@�N���A
*
* @param	buf [����] �X�N���[���o�b�t�@�̃|�C���^
*
* @note
* �X�N���[���o�b�t�@�̓E�B���h�E�T�C�Y�̉����~�c����\n
* �o�C�g�T�C�Y�ȏ��char�^�z��Ƃ���\n
* �X�N���[���o�b�t�@�̓��e��S��0�ŃN���A����
*/
void ClearFrameBuffer(char* buf)
{
	ZeroMemory(buf, sizeof(char) * g_ScreenBufferSize.X * g_ScreenBufferSize.Y);
}	//ClearFrameBuffer

/**
* @brief	�ꊇ�]���p�o�b�t�@�N���A
*
* @param	buf [����] �X�N���[���o�b�t�@�̃|�C���^
*
* @note
* �X�N���[���o�b�t�@�̓E�B���h�E�T�C�Y�̉����~�c���~24bit��\n
* �o�C�g�T�C�Y�ȏ��char�^�z��Ƃ���\n
* �X�N���[���o�b�t�@�̓��e��S��0�ŃN���A����
*/
void ClearFrameBufferFull(char* buf)
{
	ZeroMemory(buf, sizeof(char) * 3 * g_ScreenBufferSize.X * g_ScreenBufferSize.Y);
}	//ClearFrameBuffer

/**
* @brief	�F���̈ꊇ�]��\n
*	�F���i�w�i�ƕ����F�j�����������鎖�ŁA�P����(char)���P�s�N�Z���̗l�Ɉ����B\n
*	char�l�͂P�U�F�p���b�g�̃p���b�g�ԍ��i0x0F�`0x0F�j\n
*	���t�H���g�T�C�Y�����������ĉ摜�\���Ɏg��\n
*	���t�H���g�T�C�Y������������̂ŕ����\�����قڏo���Ȃ��Ȃ�\n
*
* @param	buf [����] �X�N���[���o�b�t�@�̃|�C���^
* �y���zInitConio�Ŏw�肵���X�N���[���T�C�Y�̖ʐ�(�^�e�~���R)�Ɠ����T�C�Y�̂P�����z��łȂ��Ƃ����Ȃ��B
*/
void PrintFrameBuffer(const char* _buf_8bit)
{
	DWORD size = g_ScreenBufferSize.X * g_ScreenBufferSize.Y;
	DWORD num;
	WORD* dp = g_ScreenBuffer4bit;

	// �摜�`��(��ʊO�ւ̂͂ݏo�������Ȃ�)
	for (int i = 0; i < size; i++) {
		*dp = ((*_buf_8bit) << 4) | ((*_buf_8bit) & 0x0F);
		dp++;
		_buf_8bit++;
	}
	//�����̂ݏ���������
	WriteConsoleOutputAttribute(g_DisplayHandle[g_SwapFlg], g_ScreenBuffer4bit, size, { 0,0 }, &num);
}	//PrintFrameBuffer

/**
* @brief	�_��ł�
*
* @param	int _x,_y�F���W
* @param	int _c�F�F�i�p���b�g�ԍ��O�`�P�T�j
* @param	int _tr�F���������itrue:�F�F�O�Ȃ�`�����܂Ȃ�/false�F�F�F�O���������ށj
*/
void DrawPixel(int _x, int _y, int _c)
{
	if ((_x >= 0) && (_y >= 0) && (_y < g_ScreenBufferSize.Y) && (_x < g_ScreenBufferSize.X)) {
		//�͈͓��̂ݏ�������
		//g_ScreenBuffer4bit[_y * g_ScreenBufferSize.X + _x].Attributes = (_c & 0x0F) | ((_c << 4) & 0xF0);
		////�]���J�n�ʒu���}�C�i�X�l�̎���"dest_rect"�S�̂��]������Ȃ��Ȃ�̂ŁA�O�ɂ��Ă����B
		//SMALL_RECT dest_rect = { (SHORT)_x, (SHORT)_y, (SHORT)(_x), (SHORT)(_y) };
		//WriteConsoleOutputA(g_DisplayHandle[g_SwapFlg], g_ScreenBuffer4bit, g_ScreenBufferSize, { (SHORT)_x,(SHORT)_y }, &dest_rect);
		g_FrameBuffer4bit[_y * g_ScreenBufferSize.X + _x] = _c;
	}
}	//DrawPixel
#endif // USED2D

//----------------------------------------------------------------
// BMP�摜�̕`��(LoadBmp/CreateBmpChar/CreateBmpString�Ő�������Bmp�摜)
//----------------------------------------------------------------
/**
* @brief	Bmp(4�r�b�g/Pixel)�摜�̏o��
*
* @param	int _xp	[����] �\�����W�i�O�`�j
* @param	int _yp [����] �\�����W�i�O�`�j
* @param	Bmp _bmp [����] Bmp�\���̂ւ̃|�C���^
* @param	int _inv�F���]�t���O�F0=���]����/BMP_HINV=�������]/BMP_VINV=�������]/BMP_HVINV=�����������](�P�W�O����])
* @param	bool _tr�F�����t���O�Ftrue=����/false=�s�����@--- '�O'�̕�����`�����܂Ȃ����Ƃœ�������������B
*
*/
#ifdef USED2D
//static COLORREF* pal_ptr = NULL;	//[256] = {};
//static RGBQUAD* pal_rgb_ptr = NULL;	//[256] = {};
static const Bmp* bmp_ptr = NULL;
inline void pixel_copy04(const BYTE* buf, int xx, int yy, bool _tr) {
	if (bmp_ptr->pal != NULL) {
		if ((xx >= 0) && (yy >= 0) && (yy < g_ScreenBufferSize.Y) && (xx < g_ScreenBufferSize.X)) {
			//��ʓ��ł���
			if ((!_tr) || (*buf != 0)) {
				//�����w�肪����or�����s�N�Z���ł͖���
				const char* src = (char*)&bmp_ptr->pal[*buf % bmp_ptr->numpal];
				char* dest = (char*)&g_FrameBuffer32bitD2D[yy * g_ScreenBufferSize.X + xx];
				//RGBQUAD <= COLORREF
				dest[0] = src[2];
				dest[1] = src[1];
				dest[2] = src[0];
				dest[3] = src[3];
			}
		}
	}
	else if (bmp_ptr->pal_rgb != NULL) {
		if ((xx >= 0) && (yy >= 0) && (yy < g_ScreenBufferSize.Y) && (xx < g_ScreenBufferSize.X)) {
			//��ʓ��ł���
			if ((!_tr) || (*buf != 0)) {
				//�����w�肪����or�����s�N�Z���ł͖���
				//RGBQUAD <= RGBQUAD
				g_FrameBuffer32bitD2D[yy * g_ScreenBufferSize.X + xx] = bmp_ptr->pal_rgb[*buf % bmp_ptr->numpal];
			}
		}
	}
	else {
		if ((xx >= 0) && (yy >= 0) && (yy < g_ScreenBufferSize.Y) && (xx < g_ScreenBufferSize.X)) {
			//��ʓ��ł���
			if ((!_tr) || (*buf != 0)) {
				//�����w�肪����or�����s�N�Z���ł͖���
				//RGBQUAD <= COLORREF
				g_FrameBuffer32bitD2D[yy * g_ScreenBufferSize.X + xx] = g_PaletteD2D[*buf % bmp_ptr->numpal];
			}
		}
	}
}
#else
inline void pixel_copy04(const unsigned char* buf, int xx, int yy, bool _tr) {
	if ((xx >= 0) && (yy >= 0) && (yy < g_ScreenBufferSize.Y) && (xx < g_ScreenBufferSize.X)) {
		//��ʓ��ł���
		if ((!_tr) || (*buf != 0)) {
			//�����w�肪����or�����s�N�Z���ł͖���
			g_FrameBuffer4bit[yy * g_ScreenBufferSize.X + xx] = *buf;
		}
	}
}
#endif // USED2D
static void draw_bmp_bpp04(int _xp, int _yp, const Bmp* _bmp, int _inv, bool _tr)
{
#ifdef USED2D
	bmp_ptr = _bmp;
#endif // USED2D

	//�t���[���o�b�t�@�̎w����W�֓]��
	unsigned char* buf = (unsigned char*)_bmp->pixel;
	if (_inv == 0) {
		//���]��]�����i������E�@�����j
		for (int y = 0; y < _bmp->height; y++) {
			for (int x = 0; x < _bmp->width; x++) {
				pixel_copy04(buf, x + _xp, y + _yp, _tr);
				buf++;
			}
		}
	}
	else if (_inv == BMP_HINV) {
		//�������]�̂݁i������E�@���E�j
		for (int y = 0; y < _bmp->height; y++) {
			for (int x = _bmp->width - 1; x >= 0; x--) {
				pixel_copy04(buf, x + _xp, y + _yp, _tr);
				buf++;
			}
		}
	}
	else if (_inv == BMP_VINV) {
		//�������]�̂݁i�������E�@���E�j
		for (int y = _bmp->height - 1; y >= 0; y--) {
			for (int x = 0; x < _bmp->width; x++) {
				pixel_copy04(buf, x + _xp, y + _yp, _tr);
				buf++;
			}
		}
	}
	else if (_inv == BMP_HVINV) {
		//�����{�������]�i�������E�@�����j�i�P�W�O�x��]�j
		for (int y = _bmp->height - 1; y >= 0; y--) {
			for (int x = _bmp->width - 1; x >= 0; x--) {
				pixel_copy04(buf, x + _xp, y + _yp, _tr);
				buf++;
			}
		}
	}
	else if (_inv & BMP_ROT90) {
		//���|���ɂȂ��Ă���ꍇ
		_inv &= (~BMP_ROT90);	//���|���t���O�͏���
		if (_inv == 0) {
			//���|���̂݁i�����E�E�@�����j����
			for (int x = _bmp->height - 1; x >= 0; x--) {
				for (int y = 0; y < _bmp->width; y++) {
					pixel_copy04(buf, x + _xp, y + _yp, _tr);
					buf++;
				}
			}
		}
		else if (_inv == BMP_HINV) {
			//���|���{�������]�i�������E�@�����j����
			for (int x = 0; x < _bmp->height; x++) {
				for (int y = 0; y < _bmp->width; y++) {
					pixel_copy04(buf, x + _xp, y + _yp, _tr);
					buf++;
				}
			}
		}
		else if (_inv == BMP_VINV) {
			//���|���{�������]�i�����E�E�@����j����
			for (int x = _bmp->height - 1; x >= 0; x--) {
				for (int y = _bmp->width - 1; y >= 0; y--) {
					pixel_copy04(buf, x + _xp, y + _yp, _tr);
					buf++;
				}
			}
		}
		else if (_inv == BMP_HVINV) {
			//���|���{�����{�������]�i�������E�@����j����
			for (int x = 0; x < _bmp->height; x++) {
				for (int y = _bmp->width - 1; y >= 0; y--) {
					pixel_copy04(buf, x + _xp, y + _yp, _tr);
					buf++;
				}
			}
		}
	}
#ifdef USED2D
	bmp_ptr = NULL;
#endif // USED2D
}	//draw_bmp_bpp04

/**
* @brief	24�r�b�g/Pixel�摜�̏o��
*
* @param	int _xp	[����] �\�����W�i�O�`�j
* @param	int _yp [����] �\�����W�i�O�`�j
* @param	Bmp _bmp [����] Bmp�\���̂ւ̃|�C���^
* @param	int _inv�F���]�t���O�F0=���]����/BMP_HINV=�������]/BMP_VINV=�������]/BMP_HVINV=�����������](�P�W�O����])
* @param	bool _tr�F�����t���O�Ftrue=����/false=�s�����@--- '�O'�̕�����`�����܂Ȃ����Ƃœ�������������B
*
*/
#ifdef USED2D
inline void pixel_copy24(const unsigned char* in_buf, int xx, int yy, int, bool _tr) {
	if ((xx >= 0) && (yy >= 0) && (yy < g_ScreenBufferSize.Y) && (xx < g_ScreenBufferSize.X)) {
		DWORD rgb = *((DWORD*)in_buf);
		//��ʓ��ł���
		if ((!_tr) || ((rgb & 0x00FFFFFF) != 0)) {
			//�����w�肪����or�����s�N�Z���ł͖���
			DWORD* dest = (DWORD*)&g_FrameBuffer32bitD2D[yy * g_ScreenBufferSize.X + xx];
			*dest = rgb;
		}
	}
	//in_buf += 3;
}
#else
inline void pixel_copy24(const unsigned char* in_buf, int xx, int yy, int x_stride, bool _tr) {
	if ((xx >= 0) && (yy >= 0) && (yy < g_ScreenBufferSize.Y) && (xx < g_ScreenBufferSize.X)) {
		//��ʓ��ł���
		if ((!_tr) || ((*((DWORD*)in_buf) & 0x00FFFFFF) != 0)) {
			//�����w�肪����or�����s�N�Z���ł͖���
			char* dest = &g_FrameBufferFull[yy * x_stride + (xx * 3)];
			dest[2] = in_buf[0];
			dest[1] = in_buf[1];
			dest[0] = in_buf[2];
		}
	}
	//in_buf += 3;
}
#endif // USED2D
static void draw_bmp_bpp24(int _xp, int _yp, const Bmp* _bmp, int _inv, bool _tr)
{
	const unsigned char* in_buf = (const unsigned char*)_bmp->pixel;	//CharRGBconvTBL[][]��index�Ƃ��Ĉ����̂ŁA���������ɂ��Ă���B
#ifdef USED2D
	if ((in_buf == NULL) || (g_FrameBuffer32bitD2D == NULL)) {
		return;
	}
#else
	if ((in_buf == NULL) || (g_FrameBufferFull == NULL)) {
		return;
	}
#endif // USED2D
	int x_stride = (g_ScreenBufferSize.X * 3);
	if (_inv == 0) {
		//���]��]�����i������E�@�����j
		for (int y = 0; y < _bmp->height; y++) {
			for (int x = 0; x < _bmp->width; x++) {
				pixel_copy24(in_buf, x + _xp, y + _yp, x_stride, _tr);
				in_buf += 3;
			}
		}
	}
	else if (_inv == BMP_HINV) {
		//�������]�̂݁i������E�@���E�j
		for (int y = 0; y < _bmp->height; y++) {
			for (int x = _bmp->width - 1; x >= 0; x--) {
				pixel_copy24(in_buf, x + _xp, y + _yp, x_stride, _tr);
				in_buf += 3;
			}
		}
	}
	else if (_inv == BMP_VINV) {
		//�������]�̂݁i�������E�@���E�j
		for (int y = _bmp->height - 1; y >= 0; y--) {
			for (int x = 0; x < _bmp->width; x++) {
				pixel_copy24(in_buf, x + _xp, y + _yp, x_stride, _tr);
				in_buf += 3;
			}
		}
	}
	else if (_inv == BMP_HVINV) {
		//�����{�������]�i�������E�@�����j�i�P�W�O�x��]�j
		for (int y = _bmp->height - 1; y >= 0; y--) {
			for (int x = _bmp->width - 1; x >= 0; x--) {
				pixel_copy24(in_buf, x + _xp, y + _yp, x_stride, _tr);
				in_buf += 3;
			}
		}
	}
	else if (_inv & BMP_ROT90) {
		//���|���ɂȂ��Ă���ꍇ
		_inv &= (~BMP_ROT90);	//���|���t���O�͏���
		if (_inv == 0) {
			//���|���̂݁i�����E�E�@�����j����
			for (int x = _bmp->height - 1; x >= 0; x--) {
				for (int y = 0; y < _bmp->width; y++) {
					pixel_copy24(in_buf, x + _xp, y + _yp, x_stride, _tr);
					in_buf += 3;
				}
			}
		}
		else if (_inv == BMP_HINV) {
			//���|���{�������]�i�������E�@�����j����
			for (int x = 0; x < _bmp->height; x++) {
				for (int y = 0; y < _bmp->width; y++) {
					pixel_copy24(in_buf, x + _xp, y + _yp, x_stride, _tr);
					in_buf += 3;
				}
			}
		}
		else if (_inv == BMP_VINV) {
			//���|���{�������]�i�����E�E�@����j����
			for (int x = _bmp->height - 1; x >= 0; x--) {
				for (int y = _bmp->width - 1; y >= 0; y--) {
					pixel_copy24(in_buf, x + _xp, y + _yp, x_stride, _tr);
					in_buf += 3;
				}
			}
		}
		else if (_inv == BMP_HVINV) {
			//���|���{�����{�������]�i�������E�@����j����
			for (int x = 0; x < _bmp->height; x++) {
				for (int y = _bmp->width - 1; y >= 0; y--) {
					pixel_copy24(in_buf, x + _xp, y + _yp, x_stride, _tr);
					in_buf += 3;
				}
			}
		}
	}
}	//draw_bmp_bpp24

/**
* @brief	�摜�̏o��
*
* @param	int _xp�F�\�����W�i�O�`�j
* @param	int _yp�F�\�����W�i�O�`�j
* @param	Bmp _bmp�FBmp�\���̂ւ̃|�C���^
* @param	int _inv�F���]�t���O�F0=���]����/BMP_HINV=�������]/BMP_VINV=�������]/BMP_HVINV=�����������](�P�W�O����])
* @param	bool _tr�F�����t���O�Ftrue=����/false=�s�����@--- '�O'�̕�����`�����܂Ȃ����Ƃœ�������������B
*
* @note		Bmp�摜�̐F�����������肵�ĕ`�悵�Ă���
*/
void DrawBmp(int _xp, int _yp, const Bmp* _bmp, int _inv, bool _tr)
{
	_ASSERT(_bmp);
	if (_bmp == NULL) { return; }
	if (_bmp->colbit == 4) {
		draw_bmp_bpp04(_xp, _yp, _bmp, _inv, _tr);
	}
	else if (_bmp->colbit == 8) {
		draw_bmp_bpp04(_xp, _yp, _bmp, _inv, _tr);
	}
	else if (_bmp->colbit == 24) {
		draw_bmp_bpp24(_xp, _yp, _bmp, _inv, _tr);
	}
}	//DrawBmp
/**
* @brief	�摜�̏o��
*
* @param	int _xp�F�\�����W�i�O�`�j
* @param	int _yp�F�\�����W�i�O�`�j
* @param	Bmp _bmp�FBmp�\���̂ւ̃|�C���^
* @param	bool _tr�F�����t���O�Ftrue=����/false=�s�����@--- '�O'�̕�����`�����܂Ȃ����Ƃœ�������������B
*
* @note		Bmp�摜�̐F�����������肵�ĕ`�悵�Ă���
*/
void DrawBmp(int _xp, int _yp, const Bmp* _bmp, bool _tr)
{
	DrawBmp(_xp, _yp, _bmp, 0, _tr);
}

//================================================================
// �r�b�g�}�b�v�t�@�C�����샆�[�e�B���e�B�[
//================================================================
//--------------------------------
//�摜�̃r�b�g�𑜓x�ʕϊ��֐��B
//----------------------------------------------------------------
/**
* @brief	�P�r�b�g�^�s�N�Z���̂P���C�������֐�
*
* @note
*	�P�s�N�Z���̒l�͂O�`�P�̃p���b�g�ԍ�
*
* @param	int w_pixels	�o�̓s�N�Z����
* @param	PBYTE dst_pxbuf		�o�̓o�b�t�@
* @param	int stride	���̓s�N�Z����
* @param	PBYTE src_ppx	���̓o�b�t�@
*
* @return	�Ȃ�
*/
static void print_line_1bpp(int w_pixels, PBYTE dst_pxbuf, int stride, const PBYTE src_ppx)
{
	for (int x = 0; x < stride; x++) {
		for (int mask8 = 0b10000000; mask8 != 0; mask8 >>= 1) {
			*dst_pxbuf = (src_ppx[x] & mask8) ? 0x01 : 0x00;
			dst_pxbuf++;
			w_pixels--;
			if (w_pixels <= 0) {
				return;
			}
		}
	}
}	//print_line_1bpp

/**
* @brief	�S�r�b�g�^�s�N�Z���̂P���C�������֐�
*
* @note
*	�P�s�N�Z���̒l�͂O�`�P�T�̃p���b�g�ԍ�
*
* @param	int w_pixels	�o�̓s�N�Z����
* @param	PBYTE dst_pxbuf		�o�̓o�b�t�@
* @param	int stride	���̓s�N�Z����
* @param	PBYTE src_ppx	���̓o�b�t�@
*
* @return	�Ȃ�
*/
static void print_line_4bpp(int w_pixels, PBYTE dst_pxbuf, int stride, const PBYTE src_ppx)
{
	for (int x = 0; x < stride; x++) {
		*dst_pxbuf = (src_ppx[x] >> 4) & 0x0F;
		dst_pxbuf++;
		w_pixels--;
		if (w_pixels <= 0) {
			return;
		}
		*dst_pxbuf = src_ppx[x] & 0x0F;
		dst_pxbuf++;
		w_pixels--;
		if (w_pixels <= 0) {
			return;
		}
	}
}	//print_line_4bpp

/**
* @brief	�W�r�b�g(1byte)�^�s�N�Z���̂P���C�������֐�
*
* @note
*	�P�s�N�Z���̒l�͂O�`�Q�T�T�̃p���b�g�ԍ�
*
* @param	int w_pixels	�o�̓s�N�Z����
* @param	PBYTE dst_pxbuf		�o�̓o�b�t�@
* @param	int stride	���̓s�N�Z����
* @param	PBYTE src_ppx	���̓o�b�t�@
*
* @return	�Ȃ�
*/
static void print_line_8bpp(int w_pixels, PBYTE dst_pxbuf, int stride, const PBYTE src_ppx)
{
	for (int x = 0; x < w_pixels; x++) {
		*dst_pxbuf = src_ppx[x];
		dst_pxbuf++;
	}
}	//print_line_8bpp

/**
* @brief	�P�U�r�b�g(2byte)�^�s�N�Z���̂P���C�������֐�
*
* @note
*	�P�s�N�Z���̒l�͂P�U�r�b�g�̂q�f�a�l\n
*	16bpp\n
*	16bit / 1pixel\n
*	WORD{0b0rrrrrgggggbbbbb}\n
*	RGB�l�ɕϊ�����ꍇ�́E�E�E\n
*	(BYTE)[0x07],[0x29] -> (WORD)0x2907 -> 0b0010_1001_0000_0111 -> 0b0_01010_01000_00111 -> 0A,08,07\n
*	-> (0A*FF)/1F,(08*FF)/1F,(07*FF)/1F -> RGB(52,41,39)
*
* @param	int w_pixels	�o�̓s�N�Z����
* @param	PBYTE dst_pxbuf		�o�̓o�b�t�@
* @param	int stride	���̓s�N�Z����
* @param	PBYTE src_ppx	���̓o�b�t�@
*
* @return	�Ȃ�
*/
static void print_line_16bpp(int w_pixels, PBYTE dst_pxbuf, int stride, const PBYTE src_ppx)
{
	WORD* dwpx = (WORD*)dst_pxbuf;
	WORD* swp = (WORD*)src_ppx;
	for (int x = 0; x < w_pixels; x++) {
		*dwpx = swp[x];
		dwpx++;
	}
}	//print_line_16bpp

/**
* @brief	�Q�S�r�b�g(3byte)�^�s�N�Z���̂P���C�������֐�
*
* @note
*	�P�s�N�Z���̒l�͂Q�S�r�b�g�̂q�f�a�l\n
*	24bpp\n
*	24bit / 1pixel\n
*	BYTE[BB],[GG],[RR],[??] -> DWORD{0x??RRGGBB & 0x00FFFFFF} -> (DWORD)0x00RRGGBB -> 0xRRGGBB
*
* @param	int w_pixels	�o�̓s�N�Z����
* @param	PBYTE dst_pxbuf		�o�̓o�b�t�@
* @param	int stride	���̓s�N�Z����
* @param	PBYTE src_ppx	���̓o�b�t�@
*
* @return	�Ȃ�
*/
static void print_line_24bpp(int w_pixels, PBYTE dst_pxbuf, int stride, const PBYTE src_ppx)
{
	PBYTE src = src_ppx;
	for (int x = 0; x < w_pixels; x++, src += 3) {
		dst_pxbuf[0] = src[0];	//BB
		dst_pxbuf[1] = src[1];	//GG
		dst_pxbuf[2] = src[2];	//RR
		dst_pxbuf += 3;
	}
}	//print_line_24bpp

/**
* @brief	�R�Q�r�b�g(4byte)�^�s�N�Z���摜�̂P���C�������֐�
*
* @note
*	�P�s�N�Z���̒l�͂R�Q�r�b�g�̂q�f�a�l\n
*	32bpp\n
*	32bit / 1pixel\n
*	(DWORD)0xaaRRGGBB
*
* @param	int w_pixels	�o�̓s�N�Z����
* @param	PBYTE dst_pxbuf		�o�̓o�b�t�@
* @param	int stride	���̓s�N�Z����
* @param	PBYTE src_ppx	���̓o�b�t�@
*
* @return	�Ȃ�
*/
static void print_line_32bpp(int w_pixels, PBYTE dst_pxbuf, int stride, const PBYTE src_ppx)
{
	DWORD* ddwpx = (DWORD*)dst_pxbuf;
	DWORD* sdwp = (DWORD*)src_ppx;
	for (int x = 0; x < w_pixels; x++) {
		*ddwpx = sdwp[x];
		ddwpx++;
	}
}	//print_line_32bpp

/**
* @brief	�𑜓x�ʕ\���֐��̃|�C���^�e�[�u���i�z��j
*
* @note	void (*fp)();	//�֐��|�C���^\n
* fp();	//�ďo\n
* void (*fpp[])();	//�֐��|�C���^�z��\n
* fpp[n]();	//�ďo\n
*/
void(*print_line[])(int, PBYTE, int, const PBYTE) = {
	print_line_1bpp,	/**�P�r�b�g�^�s�N�Z��*/
	print_line_4bpp,	/**�S�r�b�g�^�s�N�Z��*/
	print_line_8bpp,	/**�W�r�b�g(1byte)�^�s�N�Z��*/
	print_line_16bpp,	/**�P�U�r�b�g(2byte)�^�s�N�Z��*/
	print_line_24bpp,	/**�Q�S�r�b�g(3byte)�^�s�N�Z��*/
	print_line_32bpp	/**�R�Q�r�b�g(4byte)�^�s�N�Z��*/
};

/**
* @brief	�֐��z��C���f�b�N�X�p�񋓎q
*/
enum FUNC_NUM {
	BPP_1 = 0,	/**�P�r�b�g�^�s�N�Z��*/
	BPP_4,		/**�S�r�b�g�^�s�N�Z��*/
	BPP_8,		/**�W�r�b�g(1byte)�^�s�N�Z��*/
	BPP_16,		/**�P�U�r�b�g(2byte)�^�s�N�Z��*/
	BPP_24,		/**�Q�S�r�b�g(3byte)�^�s�N�Z��*/
	BPP_32		/**�R�Q�r�b�g(4byte)�^�s�N�Z��*/
};

/**
* @brief	BMP�t�@�C���̓ǂݍ��݁B
*
* @note	�p���b�g�̌^�ɂ��āF
*	COLORREF�^����������̕��т�
*		BYTE[]�œǂݎ���{[0]Red,[1]Green,[2]Blue,[3]0}�ƂȂ�
*		DWORD�^�œǂݎ���{0x00BBGGRR}
*	�ƂȂ�B
*	RGBQUAD�^����������̕��т�
*		BYTE[]�œǂݎ���{[0]Blue,[1]Green,[2]Red,[3]0}�ƂȂ�
*		DWORD�^�œǂݎ���{0x00RRGGBB}
*	�ƂȂ�B
*
* @note	24/32�s�N�Z���̃f�[�^�̕��тɂ��āF
*	BITMAP�̃s�N�Z���f�[�^{24bit/pixel|32bit/pixel}��,
*		24bit/pixel�̏ꍇBYTE[3]{Blue,Green,Red}/DWORD{0x**RRGGBB}
*		32bit/pixel�̏ꍇBYTE[4]{Blue,Green,Blue,alpha?}/DWORD{0xaaRRGGBB}
*	�ƂȂ��Ă���B
*
* @param	const char* _file_name		BMP�t�@�C�����i�p�X�j
* @param	bool _palset_or_swap24RB	�p���b�g�F�摜�f�[�^�̏ꍇ�̃p���b�g�ݒ�(true=����/false=���Ȃ�)
*										24bit�t���J���[�摜�̏ꍇ��Red��Blue������(true=����/false=���Ȃ�)
*
* @return	Bmp*	����ɓǂݍ��߂��ꍇ��Bmp�\���̂̃|�C���^��Ԃ��B\n
*	�y���z����ɕԂ��ꂽ�|�C���^�́A�g���I�������K��DeleteBmp(Bmp*)���Ăяo���č폜���鎖�B
*/
Bmp* LoadBmp(const char* _file_name, bool _palset_or_swap24RB)
{
	//Bmp4�\���̂̃������̈���m��
	Bmp* pb = (Bmp*)calloc(1, sizeof(Bmp));	//calloc�Ŋm�ۂ��Ă���̂ŗ̈�͂O�N���A�ς�(^_^�j
	if (!pb) {
		//NULL�|�C���^�i�l�O�j���Ԃ��Ă����玸�s(;_;)
		return pb;	//Bmp4�\���̂̊m�ێ��s
	}
	//-------------------------------------------------------------
	//BMP�t�@�C�����J���i�t�@�C���I�[�v���j
	FILE* fp = fopen(_file_name, "rb");	//���[�h�E�o�C�i�����[�h�B
	if (!fp) {
		DeleteBmp(&pb);
		return (Bmp*)NULL;
	}
	BITMAPFILEHEADER bfh;
	//BITMAPFILEHEADER(14byte)�̕�����ǂݍ��ށB
	//	WORD	bfType;			//0x4D42(�����R�[�h'B'��'M'�������Ă���BWORD�i16�r�b�g�l�j�Ō����)
	//	DWORD	bfSize;			//BMP�t�@�C���̃o�C�g��(131190)
	//	WORD	bfReserved1;	//�\��i���g�p�����j
	//	WORD	bfReserved2;	//�\��i���g�p�����j
	//	DWORD	bfOffBits;		//�s�N�Z�������܂ł̃o�C�g��
	size_t er = fread(&bfh, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
	if (!er) {
		DeleteBmp(&pb);
		fclose(fp);	//�t�@�C������
		//�ǂݍ��ݎ��s(;_;)
		return (Bmp*)NULL;
	}
	BITMAPINFOHEADER bih;
	//BITMAPINFOHEADER(40byte)������ǂݍ���
	//�ȉ��̃����o�ϐ��̏ڍׂ́FMS�̃h�L�������g<"https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapinfoheader">�Q��
	//	DWORD	   biSize;			//���̍\���́iBITMAPINFOHEADER�j�̃T�C�Y�i�o�C�g���j
	//	LONG	   biWidth;			//���̉摜�̕��i�s�N�Z�����j
	//	LONG	   biHeight;		//���̉摜�̍����i�s�N�Z�����j���v���X�i�{�j�l�Ȃ�u�{�g���A�b�v�i����������j�v�}�C�i�X�i�|�j�l�Ȃ�u�g�b�v�_�E���i���������j�v�Ńs�N�Z��������ł���
	//	WORD	   biPlanes;		//��ɂP
	//	WORD	   biBitCount;		//�P�s�N�Z��������̃r�b�g���ibpp)
	//	DWORD	   biCompression;	//��bmp_utl�ł͈��k�`���͈����Ă��Ȃ��̂ŁA�񈳏k�`����BI_RGB���������Ă���iBI_BITFIELDS�����邪��ʓI�Ɏg���Ă��Ȃ��̂Ŕ�Ή��j
	//	DWORD	   biSizeImage;		//�񈳏kRGB�r�b�g�}�b�v�̏ꍇ��0�ɏo����̂ŁA�l�������Ă��Ă��Q�Ƃ��Ȃ��BbiSizeImage �̐������l�� biWidth�CbiHeight�CbiBitCount ����v�Z�ł���
	//	LONG	   biXPelsPerMeter;	//�����𑜓x�F�P�ʂ�1m������̂������i��f��/���j���O�̏ꍇ������̂ŎQ�Ƃ��Ȃ�
	//	LONG	   biYPelsPerMeter;	//�����𑜓x�F�P�ʂ�1m������̂������i��f��/���j���O�̏ꍇ������̂ŎQ�Ƃ��Ȃ�
	//	DWORD	   biClrUsed;		//�p���b�g�̐��i�J���[�e�[�u�����j�F�O�Ȃ�biBitCount�̃r�b�g���ŕ\���ł���ő�l���e�[�u�����ƂȂ遦�ڍׂ�MS�̃h�L�������g�Q��
	//	DWORD	   biClrImportant;	//���̉摜��\������̂ɕK�v�ȐF���i�p���b�g���j�O�Ȃ�S�Ă̐F���K�v��bmp_utl�ł͂��̒l�͖������đS�F�K�v�Ƃ��Ă���
	er = fread(&bih, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	if (!er) {
		DeleteBmp(&pb);
		fclose(fp);	//�t�@�C������
		//�ǂݍ��ݎ��s(;_;)
		return (Bmp*)NULL;
	}
	//�p���b�g�i�J���[�e�[�u���j�����݂���΂��̗̈���m�ۂ��ēǂݍ��ށB
	//�p���b�g�����邩�Ȃ�����biBitCount�����Ĕ��f����
	pb->numpal = 0;	//���f�O�̓p���b�g�����i�O�j�ɂ��Ă���
	pb->pal = (COLORREF*)NULL;	//���f�O�̓p���b�g�����iNULL�j�ɂ��Ă���
#ifdef USED2D
	pb->pal_rgb = (RGBQUAD*)NULL;	//���f�O�̓p���b�g�����iNULL�j�ɂ��Ă���
#endif // USED2D
	//�P�`�W�̓p���b�g�݂�A����ȊO��1�s�N�Z����16�r�b�g�b24�r�b�g�b32�r�b�g
	if (bih.biBitCount >= 1 && bih.biBitCount <= 8) {
		//�Pbit�F�Q�F�A�Sbit�F�P�U�F�A�Wbit�F�Q�T�U�F
		pb->numpal = (1 << bih.biBitCount);	//���̃r�b�g����������ΕK�v�ȍő�p���b�g���ɂȂ�
		if (bih.biClrUsed > 0) {
			pb->numpal = bih.biClrUsed;	//biClrUsed�ɒl�������Ă���ꍇ�͗D�悷��B
		}
#ifndef USED2D
		//RGB�l��\��RGBQUAD�l�̔z����p���b�g���̐������m�ۂ���
		//�P�F��DWORD�Ńo�C�g���я���[B][G][R][A]�Ƃ��Ċi�[����Ă���
		//�P�F�̃f�[�^��RGBQUAD�\���̂Ƃ��ēǂݍ��ށ����g���G���f�B�A���Ŋi�[����Ă���̂�DWORD�^�œǂݍ��ނ�0xAARRGGBB�ƂȂ�
		RGBQUAD* pal_rgb = (RGBQUAD*)calloc(pb->numpal, sizeof(RGBQUAD));
		if (!pal_rgb) {
			//NULL�|�C���^�i�l�O�j���Ԃ��Ă����玸�s(;_;)
			DeleteBmp(&pb);
			fclose(fp);	//�t�@�C������
			return (Bmp*)NULL;
		}
		//�m�ۂ����T�C�Y���p���b�g�f�[�^�iRGBQUAD�~�p���b�g���j���t�@�C������ǂݍ��ށB
		size_t er = fread(pal_rgb, pb->numpal, sizeof(RGBQUAD), fp);
		if (!er) {
			DeleteBmp(&pb);
			fclose(fp);	//�t�@�C������
			return (Bmp*)NULL;
		}
		pb->pal = (COLORREF*)calloc(pb->numpal, sizeof(COLORREF));
		if (!pb->pal) {
			//NULL�|�C���^�i�l�O�j���Ԃ��Ă����玸�s(;_;)
			free(pal_rgb);
			DeleteBmp(&pb);
			fclose(fp);	//�t�@�C������
			return (Bmp*)NULL;
		}
		//RGBQUAD{B,G,B,0}:0x00RRGGBB�^��COLORREF:0x00BBGGRR�^�ɕϊ�
		for (int n = 0; n < pb->numpal; n++) {
			pb->pal[n] = (pal_rgb[n].rgbRed & 0x0000FF) | ((pal_rgb[n].rgbGreen << 8) & 0x00FF00) | ((pal_rgb[n].rgbBlue << 16) & 0xFF0000);
		}
#else
		//RGB�l��\��RGBQUAD�l�̔z����p���b�g���̐������m�ۂ���
		//�P�F��DWORD�Ńo�C�g���я���[B][G][R][A]�Ƃ��Ċi�[����Ă���
		//�P�F�̃f�[�^��RGBQUAD�\���̂Ƃ��ēǂݍ��ށ����g���G���f�B�A���Ŋi�[����Ă���̂�DWORD�^�œǂݍ��ނ�0xAARRGGBB�ƂȂ�
		RGBQUAD* pal_rgb4 = (RGBQUAD*)_malloca(pb->numpal * sizeof(RGBQUAD));	//�X�^�b�N��Ɋm�ہifree�s�v�j
		if (!pal_rgb4) {
			//NULL�|�C���^�i�l�O�j���Ԃ��Ă����玸�s(;_;)
			DeleteBmp(&pb);
			fclose(fp);	//�t�@�C������
			return (Bmp*)NULL;
		}
		//�m�ۂ����T�C�Y���p���b�g�f�[�^�iRGBQUAD�~�p���b�g���j���t�@�C������ǂݍ��ށB
		size_t er = fread(pal_rgb4, pb->numpal, sizeof(RGBQUAD), fp);
		if (!er) {
			DeleteBmp(&pb);
			fclose(fp);	//�t�@�C������
			return (Bmp*)NULL;
		}
		//
		pb->pal = (COLORREF*)calloc(pb->numpal, sizeof(COLORREF));
		if (!pb->pal) {
			//NULL�|�C���^�i�l�O�j���Ԃ��Ă����玸�s(;_;)
			DeleteBmp(&pb);
			fclose(fp);	//�t�@�C������
			return (Bmp*)NULL;
		}
		//RGBQUAD{B,G,B,0}:0x00RRGGBB�^��COLORREF:0x00BBGGRR�^�ɕϊ�
		for (int n = 0; n < pb->numpal; n++) {
			pb->pal[n] = (pal_rgb4[n].rgbRed & 0x0000FF) | ((pal_rgb4[n].rgbGreen << 8) & 0x00FF00) | ((pal_rgb4[n].rgbBlue << 16) & 0xFF0000);
		}
#endif // USED2D
	}
	//�c��̓s�N�Z���l�Ȃ̂ŁA�c���S���ǂݍ���
	//BITMAPFILEHEADER�{BITMAPINFOHEADER�i�{�p���b�g�z��j�����������c��̃T�C�Y����S�ēǂݍ���Ńt�@�C�������B
	DWORD pixel_data_size = (bfh.bfSize - bfh.bfOffBits);
	BYTE* pixel_data = (BYTE*)calloc(pixel_data_size, sizeof(BYTE));
	if (!pixel_data) {
		//NULL�|�C���^�i�l�O�j���Ԃ��Ă����玸�s(;_;)
		DeleteBmp(&pb);
		fclose(fp);	//�t�@�C������
		return (Bmp*)NULL;
	}
	//pfb->ppx_size = pixel_data_size;
	er = fread(pixel_data, sizeof(BYTE), pixel_data_size, fp);
	if (!er) {
		free(pixel_data);
		DeleteBmp(&pb);
		fclose(fp);	//�t�@�C������
		return (Bmp*)NULL;
	}
	//�t�@�C���̓ǂݍ��݂͏I�������̂Ńt�@�C���͕��Ă����B
	if (fp) {
		fclose(fp);
		fp = (FILE*)NULL;
	}
	//�摜�s�N�Z�������������₷���f�[�^�ɕϊ��F
	//	�u�P�`�S�r�b�g�^�s�N�Z���v�����u�P�o�C�g�^�s�N�Z���v
	//	�u�W�r�b�g�^�s�N�Z���v�������̂܂܁F�P�o�C�g�^�s�N�Z��
	//	�u�Q�S�^�s�N�Z���v�������̂܂܁F�R�o�C�g�^�s�N�Z��
	//	�u�R�Q�^�s�N�Z���v�������̂܂܁F�S�o�C�g�^�s�N�Z��
	PBYTE tmp_px_data = pixel_data;// + sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD)*pfb->pal_count);	//�摜�f�[�^�̐擪�ɂ���B
	//(((biWidth * biBitCount + 31)& ~31)>>3) : �{�R�P(1F)�Ɓ�~�R�P(E0)�łO�`�R�P�r�b�g�̒l�͐؂�グ�āA>>3�Ńr�b�g�����o�C�g���ɂ���B
	//	biWidth(�K�v�ȉ��s�N�Z����) * biBitCount(�P�s�N�Z���̃r�b�g��) �� �K�v�ȉ������̃r�b�g���B
	//	�{31 �� �S�o�C�g�̔{���ɂ������̂Łi�W�r�b�g�~�S�o�C�g�|�P�r�b�g�F0x1F:0b00011111�j�����Z �� �[�����P�r�b�g�ł������32bit(4byte)���Z�ɂȂ�B
	//	& ~31�� �� �`�R�P�i0x1F�̃r�b�g���]:0001_1111 �� 1110_0000:0xE0�j�Ń}�X�N����ƁA�S�̔{���ɂȂ�B
	//	>>3 �� >>1(1/2) �� >>2(1/4) �� >>3(1/8) �� �o�C�g���Ɋ��Z
	int ppx_stride = (((bih.biWidth * bih.biBitCount + 31) & ~31) >> 3);	//�L���s�N�Z�������܂񂾂S�̔{��(�S�o�C�g�A���C�����g)�̃s�N�Z�����ɂ���B
	LONG height = bih.biHeight;	//�摜�̏c�s�N�Z�����i�����Ȃ�g�b�v�_�E���j
	BOOL is_top_down = (height < 0);	//����(�O����)�̏ꍇ�̓g�b�v�_�E���B
	LONG add_stride = ppx_stride;	//Line���ɉ��Z����l�B�P�s���i4�o�C�g�A���C�����g�j��BMP�̃s�N�Z���f�[�^�͂S�o�C�g�A���C�����g�Ŋi�[����Ă���B
	if (is_top_down) {
		//�g�b�v�_�E��DIB.
		height = (-height);	//�g�b�v�_�E���Ȃ̂Ő��̐��ɕ␳�B
	}
	else {
		//�{�g���A�b�vDIB.
		tmp_px_data += (ppx_stride * (height - 1));	//��ԉ��̍s�擪�Ƀ|�C���g�B
		add_stride = (-add_stride);	//�������ɂP�s�Â��Z���Ă䂭�B
	}
	//��f�i�s�N�Z���j�̃r�b�g���ɑΉ������ϊ��p�̊֐��|�C���^��I������B
	pb->colbit = bih.biBitCount;
	FUNC_NUM	bpp_num = BPP_1;
	switch (pb->colbit) {
	case	1:	bpp_num = BPP_1;	break;
	case	4:	bpp_num = BPP_4;	break;
	case	8:	bpp_num = BPP_8;	break;
	case	16:	bpp_num = BPP_16;	break;
	case	24:	bpp_num = BPP_24;	break;
	case	32:	bpp_num = BPP_32;	break;
	default:	bpp_num = BPP_8;	break;
	}
	int bypp = (pb->colbit <= 8) ? 1 : pb->colbit / 8;	//�P�s�N�Z��������̃o�C�g��
	//(�� �~ ���� �~ 1�s�N�Z��������̃o�C�g���i�A��8�r�b�g�ȉ��͑S��1�o�C�g�����j�j�̗̈���m�ۂ��A�ϊ����Ă���i�[����
	pb->numpix = (bih.biWidth * height * bypp);
	pb->pixel = (char*)calloc(pb->numpix, sizeof(char));
	BYTE* tmp_ppx = (BYTE*)pb->pixel;
	if (!tmp_ppx) {
		//NULL�|�C���^�i�l�O�j���Ԃ��Ă����玸�s(;_;)
		free(pixel_data);
		DeleteBmp(&pb);
		return (Bmp*)NULL;
	}
	//�P���C���Â�f�i�s�N�Z���j�̃r�b�g���ɑΉ������f�[�^�ɕϊ����Ȃ���R�s�[����B
	for (int y = 0; y < height; y++) {
		//(int w_pixels, PBYTE dst_pxbuf, int stride, PBYTE src_ppx)
		print_line[bpp_num](bih.biWidth, tmp_ppx, ppx_stride, tmp_px_data);
		tmp_ppx += (bih.biWidth * bypp);
		tmp_px_data += add_stride;
	}
	//�ϊ����I������̂Ō��f�[�^�͉������
	free(pixel_data);
	//�摜�̃T�C�Y�i�s�N�Z���P�ʁj���g���₷���l�ɕʂ̕ϐ��ɃR�s�[����
	pb->width = bih.biWidth;
	pb->height = bih.biHeight;
	pb->swapRB = FALSE;
	if (_palset_or_swap24RB && (pb->colbit == 24)) {
		Bmp24SwapRB(pb);	//<=swapRB��TRUE�ɂȂ�
	}
	if (_palset_or_swap24RB && ((pb->colbit == 4) || (pb->colbit == 8))) {
		SetPalette(pb->pal, 0, 15);	//�R���\�[���̃p���b�g�ւ���BMP�̃p���b�g��]������i16�F���]���j
	}
	//����ɓǂݍ��߂��̂�PicBmp�\���̂ւ̃|�C���^��Ԃ�
	return pb;
}	//LoadBmp

/**
* @brief	�g���I�����BMP�摜�̍폜
*
* @param	Bmp** _pp_bmp	Bmp�\���̃|�C���^�ϐ��̃A�h���X�B
*
* @note		�n���ꂽBmp�\���̂̃����o���āF\n
*			pixel��pal�i�|�C���^�j�͊m�ۂ���Ă��郁��������������B\n
*			�n���ꂽ�|�C���^�ϐ����w������Bmp�\���̂��������NULL����������B\n
*/
void DeleteBmp(Bmp** _pp_bmp)
{
	if (_pp_bmp == NULL) {
		//NULL�|�C���^�Ȃ牽�����Ȃ�
		return;
	}
	if ((*_pp_bmp) == NULL) {
		//�|�C���^�̒��g�i�A�h���X�j��NULL�Ȃ牽�����Ȃ��iBmp�\���̂��m�ۂ���Ă��Ȃ��j
		return;
	}
	//�m�ۂ����������i�|�C���^�j�������Ă���΍폜����B
	if ((*_pp_bmp)->pixel != NULL) {
		//�s�N�Z���f�[�^�폜
		free((*_pp_bmp)->pixel);	//�폜
		//Bmp�̓��e������
		//(*_pp_bmp)->pixel = NULL;
	}
	if ((*_pp_bmp)->pal != NULL) {
		//�p���b�g�f�[�^�폜
		free((*_pp_bmp)->pal);	//�폜
		//Bmp�̓��e������
		//(*_pp_bmp)->pal = NULL;
	}
	//(*_pp_bmp)->width = 0;
	//(*_pp_bmp)->height = 0;
	//(*_pp_bmp)->colbit = 0;
	//
	free(*_pp_bmp);	//Bmp�{�̂����
	(*_pp_bmp) = NULL;	//NULL�ɂ��Ă���
	return;
}	//DeleteBmp

/**
* @brief	�Q�S�r�b�g�摜��'Red'��'Blue'�����ւ���
*
* @note		BMP�t�@�C���̂Q�S�r�b�g�摜�̂P�s�N�Z����RGB�l�̕���[B][R][G]���A<br/>
*			"conioex"��"PrintImage()"�ŏo�͂��鎞�̕���[R][G][B]�ɕϊ�����B
*
* @param	Bmp�\���̂ւ̃|�C���^
*/
void Bmp24SwapRB(Bmp* _bmp)
{
	if (_bmp == NULL) {
		return;
	}
	if (_bmp->colbit != 24) {
		return;
	}
	if (_bmp->swapRB) {
		return;	//���ɓ��֍ς�
	}
	char* pix = _bmp->pixel;
	int width = _bmp->width;
	int height = _bmp->height;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			char tmp = pix[2];	//2
			pix[2] = pix[0];	//2<-0
			pix[0] = tmp;		//0<-2
			pix += 3;
		}
	}
	_bmp->swapRB = TRUE;
}	//Bmp24SwapRB

/*
* @brief	Bmp�I�u�W�F�N�g�̐���
*
* @param	int _width,_height�F�摜�̕��ƍ����i�s�N�Z�����j
* @param	int _colbits�F�F���F�P�s�N�Z��������̃r�b�g��
* @param	int _numpal�F�p���b�g���F�O�Ȃ�A�p���b�g�p�o�b�t�@�m�ۂ��Ȃ��iCOLORREF*pal��NULL�ɂȂ�j
* @param	const COLORREF* const  _pal�F�]�����p���b�g�F_numpal���P�ȏ゠��ꍇ�A�]�����ɂȂ�p���b�g�B
*				���̃|�C���^��NULL��_numpal���P�ȏ゠��ꍇ�̓p���b�g�m�ۂ��ăf�t�H���g�F���Z�b�g����B
*
* @return	Bmp*�F�쐬����Bmp�\���̂ւ̃|�C���^�^�G���[�̏ꍇ��NULL��Ԃ�
*/
Bmp* CreateBmp(int _width, int _height, int _colbits, size_t _numpal, const COLORREF* const  _pal)
{
	//�T�C�Y���s���ȏꍇ�͐������Ȃ��B
	if ((_width <= 0) || (_height <= 0)) {
		return (Bmp*)NULL;
	}
	Bmp* p_bmp = (Bmp*)calloc(1, sizeof(Bmp));
	_ASSERT(p_bmp);

	//=== �p���b�g�ݒ� ===
	//�p���b�g���̕␳�i���̃p���b�g�����Ԉ���Ă����琳�����l���Z�o�j
	switch (_colbits) {
	case	1:
		_numpal = 2;
		break;
	case	4:
		//�p���b�g�����I�[�o�[���Ă�����␳
		if (_numpal > 16) {
			_numpal = 16;
		}
		break;
	case	8:
		//�p���b�g�����I�[�o�[���Ă�����␳
		if (_numpal > 256) {
			_numpal = 256;
		}
		break;
	case	16:
	case	24:
	case	32:
		_numpal = 0;
		break;
	default:
		//�p���b�g���K�v�ȐF�r�b�g������Ȃ���΃p���b�g�͊m�ۂ��Ȃ��̂Ńp���b�g�����O�ɂ���B
		_colbits = 4;	//�͈͊O��4bitColor
		_numpal = 0;	//�p���b�g�͖���
		break;
	}
	p_bmp->colbit = _colbits;		//�␳�ςݐF�r�b�g��
	p_bmp->numpal = (int)_numpal;	//�������l��V�������Bmp�̃p���b�g���ɃZ�b�g
	//
	//�p���b�g�p�o�b�t�@�̊m��
	p_bmp->pal = (COLORREF*)NULL;		//COLORREF*�p���b�g�i16�F�j�f�[�^�ւ̃|�C���^�������ꍇ��NULL
	if (p_bmp->numpal > 0) {
		//�p���b�g���w�肪�P�ȏ゠��̂Ń��������m��
		p_bmp->pal = (COLORREF*)calloc(p_bmp->numpal, sizeof(COLORREF));
		_ASSERT(p_bmp->pal);
		if (_pal != NULL) {
			//�]�����p���b�g�w�肪����΃R�s�[����B
			memcpy(p_bmp->pal, _pal, p_bmp->numpal * sizeof(COLORREF));
		}
		else {
			//�w�肪�����ꍇ�̓f�t�H���g�J���[�����Ă����B
			memcpy(p_bmp->pal, ANSI_PAL256_COLOR, p_bmp->numpal * sizeof(COLORREF));
		}
	}

	//=== �摜�s�N�Z�� ===
	p_bmp->width = _width;		//��
	p_bmp->height = _height;	//����
	int bypp = 1;
	switch (p_bmp->colbit) {
	case	1:
	case	2:
	case	4:
	case	8:
		//�P�s�N�Z��=1�o�C�g�Ȃ̂ł��̂܂�
		break;
	case	16:
		//�P�s�N�Z��=�Q�o�C�g
		bypp = 2;
		break;
	case	24:
		bypp = 3;
		break;
	case	32:
		bypp = 4;
		break;
	}
	p_bmp->numpix = (p_bmp->width * p_bmp->height) * bypp;	//�摜�f�[�^�̃o�C�g��
	//���������m��
	p_bmp->pixel = (char*)calloc(1, p_bmp->numpix);	//char*�摜�f�[�^�ւ̃|�C���^
	_ASSERT(p_bmp->pixel);
	//
	p_bmp->swapRB = false;	//24�r�b�g�ȏ�̉摜�ŁAR��B������ւ���Ă���ꍇ��'TRUE'�ɂȂ�
	//
	p_bmp->wch = 0;		//�ϊ����̕��� wchar_t
	p_bmp->aaLv = 0;	//�����\���̎��̃A���`�G�C���A�X���x��
	return p_bmp;
}	//CreateBmp

/*
* @brief	�摜�̕����Ǎ�
*			4�r�b�g�^�W�r�b�g�^�Q�S�r�b�g�摜�̂ݑΉ�
*
* @param	const char* _path	�������ɂȂ�摜�t�@�C����
* @param	int _x0,_y0		�����J�n������W
* @param	int _xpix,_ypix	��������Z���摜�P�̕��ƍ���
* @param	int _xcount		�������̌�
* @param	int _ycount		�c�����̌�
* @param	Bmp** _pp_bmp	��������Bmp*������z��̃A�h���X
*
* @return	bool�F�Ǎ��̐��ہitrue:����/false:���s�j
*/
bool LoadDivBmp(const char* _path, int _x0, int _y0, size_t _xpix, size_t _ypix, size_t _xcount, size_t _ycount, Bmp** _pp_bmp)
{
	if ((_path == NULL) || (_pp_bmp == NULL)) {
		return false;
	}
	if ((_xpix * _ypix * _xcount * _ycount) == 0) {
		return false;
	}
	//_pp_bmp = ppBmp;
	Bmp* ptb = LoadBmp(_path);	//
	_ASSERT(ptb);
	ptb->pixel;

	int dest_idx = 0;
	//���ˏc�����Ɏ�荞��ł䂭
	for (int y = 0; y < _ycount; y++) {
		for (int x = 0; x < _xcount; x++) {
			//�؂�o���T�C�Y�Ō���Bmp�Ɠ����F���̋��Bmp�����
			Bmp* pp = CreateBmp(_xpix, _ypix, ptb->colbit, ptb->numpal, ptb->pal);
			CopyBmp(pp, 0, 0, ptb, _x0 + (x * _xpix), _y0 + (y * _ypix), _xpix, _ypix, false);
			_pp_bmp[dest_idx] = pp;
			dest_idx++;
		}
	}
	return true;
}	//LoadDivBmp

/*
* @brief	Bmp�̑S�s�N�Z��������������
*
* @param	int	_color	�p���b�g�ԍ�����RGB�l���w��
*				�p���b�g�̂���摜�̏ꍇ�̓p���b�g�ԍ����w�肷��
*				�t���J���[�̏ꍇ��0x00RRGGBB���w�肷��
*/
void ClearBmp(Bmp* _p, int _color)
{
	if (_p->colbit == 4) {
		memset(_p->pixel, _color & 0xF, _p->numpix);
	}
	else if (_p->colbit == 8) {
		memset(_p->pixel, _color & 0xFF, _p->numpix);
	}
	else if (_p->colbit == 24) {
		int rr = (_color & 0xFF0000) >> 16;
		int gg = (_color & 0x00FF00) >> 8;
		int bb = (_color & 0x0000FF);
		for (int i = 0; i < _p->numpix; i += 3) {
			_p->pixel[i + 0] = bb;
			_p->pixel[i + 1] = gg;
			_p->pixel[i + 2] = rr;
		}
	}
}	//ClearBmp

/*
* @brief	Bmp�摜����Bmp�摜�ւ̋�`�]��
*			�E�]����Ɠ]�����̉摜�̑傫���͈���Ă��Ă��ǂ�
*			�E�]����Ɠ]�����̉摜�̃J���[�r�b�g���͓����łȂ���΂Ȃ�Ȃ�
*			�E�]����̃p���b�g�͓]�����̃p���b�g�ŏ㏑�������i�p���b�g�̃T�C�Y���Ⴄ�ꍇ�͏��������ɍ��킹��j
*
* @param	Bmp* _dest			�]����Bmp�摜�i���ɉ摜�͏㏑�������j
* @param	int _dx,_dy			�]����̍��W
* @param	const Bmp* _src		�]����Bmp�摜
* @param	int	_sx,_sy			�]�����̍��W
* @param	int	_width,_height	�]�����̃T�C�Y
*/
Bmp* CopyBmp(Bmp* _dest, int _dx, int _dy, const Bmp* _src, int _sx, int _sy, int _width, int _height, bool _tr)
{
	if ((_dest == NULL) || (_src == NULL) || (_width <= 0) || (_height <= 0)) {
		return NULL;
	}
	if (_src->colbit != _dest->colbit) {
		return NULL;
	}
	//�p���b�g�̃R�s�[
	int pal_size = _src->numpal;	//�]�����̃T�C�Y
	if (_dest->numpal < _src->numpal) {
		//�]����̃T�C�Y���������ꍇ�́A�]����̃T�C�Y�ɍ��킹��B
		pal_size = _dest->numpal;
	}
	memcpy(_dest->pal, _src->pal, pal_size);	//�p���b�g�]�����s
	//////memset(_dest->pixel, 0, _dest->numpix);	//�s�N�Z���S�����O�ŏ��������Ă����i�]�����Ȃ������͂O�ɂȂ�j
	//��`�]�� ---
	if ((_src->colbit == 4) || (_src->colbit == 8)) {
		for (int y = 0; y < _height; y++) {
			for (int x = 0; x < _width; x++) {
				int dx = _dx + x;
				int dy = _dy + y;
				if ((dx < 0) || (dx >= _dest->width) || (dy < 0) || (dy >= _dest->height)) {
					continue;
				}
				int xx = _sx + x;
				int yy = _sy + y;
				if ((xx >= 0) && (xx < _src->width) && (yy >= 0) && (yy < _src->height)) {
					//���摜�͈͓̔��̂ݓ]������˔͈͊O�͌��̉摜�̂܂�
					char c = _src->pixel[yy * _src->width + xx];
					if ((!_tr) || (c != 0)) {
						//�����w�肪����or�����s�N�Z���ł͖���
						//*dest_p = c;
						_dest->pixel[dy * _dest->width + dx] = c;
					}
				}
				//dest_p++;
			}
		}
	}
	else if (_src->colbit == 16) {
		//��`�u���b�N�]��
		for (int y = 0; y < _height; y++) {
			for (int x = 0; x < _width; x++) {
				int dx = _dx + x;
				int dy = _dy + y;
				if ((dx < 0) || (dx >= _dest->width) || (dy < 0) || (dy >= _dest->height)) {
					continue;
				}
				int xx = _sx + x;
				int yy = _sy + y;
				if ((xx >= 0) && (xx < _src->width) && (yy >= 0) && (yy < _src->height)) {
					//���摜�͈͓̔��̂ݓ]������˔͈͊O�͌��̉摜�̂܂�
					UINT16 ui16 = ((UINT16*)_src->pixel)[yy * _src->width + xx];
					if ((!_tr) || (ui16 != 0)) {
						//�����w�肪����or�����s�N�Z���ł͖���
						((UINT16*)_dest->pixel)[dy * _dest->width + dx] = ui16;
					}
				}
			}
		}
	}
	else if (_src->colbit == 24) {
		//��`�u���b�N�]��
		for (int y = 0; y < _height; y++) {
			for (int x = 0; x < _width; x++) {
				int dx = _dx + x;
				int dy = _dy + y;
				if ((dx < 0) || (dx >= _dest->width) || (dy < 0) || (dy >= _dest->height)) {
					continue;
				}
				int xx = _sx + x;
				int yy = _sy + y;
				if ((xx >= 0) && (xx < _src->width) && (yy >= 0) && (yy < _src->height)) {
					char* pd = &_dest->pixel[dy * (_dest->width * 3) + (dx * 3)];
					char* ps = &_src->pixel[yy * (_src->width * 3) + (xx * 3)];
					int c = (ps[0] | ps[1] | ps[2]);
					if ((!_tr) || (c != 0)) {
						pd[0] = ps[0];
						pd[1] = ps[1];
						pd[2] = ps[2];
					}
				}
				//dest_p += 3;
			}
		}
	}
	else if (_src->colbit == 32) {
		//��`�u���b�N�]��
		for (int y = 0; y < _height; y++) {
			for (int x = 0; x < _width; x++) {
				int dx = _dx + x;
				int dy = _dy + y;
				if ((dx < 0) || (dx >= _dest->width) || (dy < 0) || (dy >= _dest->height)) {
					continue;
				}
				int xx = _sx + x;
				int yy = _sy + y;
				if ((xx >= 0) && (xx < _src->width) && (yy >= 0) && (yy < _src->height)) {
					//���摜�͈͓̔��̂ݓ]������˔͈͊O�͌��̉摜�̂܂�
					UINT32 ui32 = ((UINT32*)_src->pixel)[yy * _src->width + xx];
					if ((!_tr) || (ui32 != 0)) {
						//�����w�肪����or�����s�N�Z���ł͖���
						((UINT32*)_dest->pixel)[dy * _dest->width + dx] = ui32;
					}
				}
			}
		}
	}
	return _dest;
}	//CopyBmp

/*
* @brief	Bmp�摜�̎w��͈͂����ɁA�V����Bmp�𐶐�����B
*
* @param	const Bmp* _src		���̉摜
* @param	int _xp,_yp			�w��͈͂̍�����W
* @param	int _width,_height	�w��͈͂̕��ƍ���
*
* @return	Bmp*	�V���������Bmp�摜�̃|�C���^
*/
Bmp* CreateFromBmp(const Bmp* _src, int _xp, int _yp, int _width, int _height)
{
	if ((_src == NULL) || (_width <= 0) || (_height <= 0)) {
		return NULL;
	}
	Bmp* p_dest = CreateBmp(_width, _height, _src->colbit, _src->numpal, _src->pal);
	_ASSERT(p_dest);
	CopyBmp(p_dest, _src, _xp, _yp, _width, _height, false);
	return p_dest;
}	//CreateFromBmp


//================================================================
// Bitmap�t�H���g�����֐�
// 
// �w��̃t�H���g���g���Ă��̃t�H���g�Ő������ꂽ������C���[�W���r�b�g�}�b�v�摜�f�[�^�ɕϊ�����B
// �y���z�R���\�[���v���O������p�˓����ŃR���\�[���E�B���h�E�̃n���h�����g���Ă���B
//
// ToDo:
// �p���b�g�̉��Ԃ̐F���g�������O������o����l�ɂ���B
// ex)�Q�l�F�̎��A'0'->pal[13], '1'->pal[14]�Ƃ��B
//================================================================

//const DWORD Gray65[65] = {
//	//======== START ========
//	0x00000000,
//	0x00030303,0x00070707,0x000b0b0b,0x000f0f0f,0x00131313,0x00171717,0x001b1b1b,0x001f1f1f,
//	0x00232323,0x00272727,0x002b2b2b,0x002f2f2f,0x00333333,0x00373737,0x003b3b3b,0x003f3f3f,//[16]
//	0x00434343,0x00474747,0x004b4b4b,0x004f4f4f,0x00535353,0x00575757,0x005b5b5b,0x005f5f5f,
//	0x00636363,0x00676767,0x006b6b6b,0x006f6f6f,0x00737373,0x00777777,0x007b7b7b,0x007f7f7f,//[32]
//	0x00838383,0x00878787,0x008b8b8b,0x008f8f8f,0x00939393,0x00979797,0x009b9b9b,0x009f9f9f,
//	0x00a3a3a3,0x00a7a7a7,0x00ababab,0x00afafaf,0x00b3b3b3,0x00b7b7b7,0x00bbbbbb,0x00bfbfbf,//[48]
//	0x00c3c3c3,0x00c7c7c7,0x00cbcbcb,0x00cfcfcf,0x00d3d3d3,0x00d7d7d7,0x00dbdbdb,0x00dfdfdf,
//	0x00e3e3e3,0x00e7e7e7,0x00ebebeb,0x00efefef,0x00f3f3f3,0x00f7f7f7,0x00fbfbfb,0x00ffffff,//[64]
//	//======== END ========
//};
//const DWORD Gray17[17] = {
//	//======== START ========
//	0x00000000,
//	0x000f0f0f,0x001f1f1f,0x002f2f2f,0x003f3f3f,0x004f4f4f,0x005f5f5f,0x006f6f6f,0x007f7f7f,
//	0x008f8f8f,0x009f9f9f,0x00afafaf,0x00bfbfbf,0x00cfcfcf,0x00dfdfdf,0x00efefef,0x00ffffff,
//	//======== END ========
//};
//const DWORD Gray5[5] = {
//	//======== START ========
//	0x00000000,
//	0x003f3f3f,0x007f7f7f,0x00bfbfbf,0x00ffffff,
//	//======== END ========
//};
//const DWORD Gray2[2] = {
//	//======== START ========
//	0x00000000,
//	0x00ffffff,
//	//======== END ========
//};

const DWORD Gray2[2] = {
	//======== START ========
	0x00000000,
	0xFFffffff,//[ 1]
	//======== END ========
};
const DWORD Gray5[5] = {
	//======== START ========
	0x00000000,
	0x3F3f3f3f,0x7F7f7f7f,0xBFbfbfbf,0xFFffffff,//[ 4]
	//======== END ========
};
const DWORD Gray17[17] = {
	//======== START ========
	0x00000000,
	0x0F0f0f0f,0x1F1f1f1f,0x2F2f2f2f,0x3F3f3f3f,0x4F4f4f4f,0x5F5f5f5f,0x6F6f6f6f,0x7F7f7f7f,
	0x8F8f8f8f,0x9F9f9f9f,0xAFafafaf,0xBFbfbfbf,0xCFcfcfcf,0xDFdfdfdf,0xEFefefef,0xFFffffff,//[16]
	//======== END ========
};
const DWORD Gray65[65] = {
	//======== START ========
	0x00000000,
	0x03030303,0x07070707,0x0B0b0b0b,0x0F0f0f0f,0x13131313,0x17171717,0x1B1b1b1b,0x1F1f1f1f,0x23232323,0x27272727,0x2B2b2b2b,0x2F2f2f2f,0x33333333,0x37373737,0x3B3b3b3b,0x3F3f3f3f,//[16]
	0x43434343,0x47474747,0x4B4b4b4b,0x4F4f4f4f,0x53535353,0x57575757,0x5B5b5b5b,0x5F5f5f5f,0x63636363,0x67676767,0x6B6b6b6b,0x6F6f6f6f,0x73737373,0x77777777,0x7B7b7b7b,0x7F7f7f7f,//[32]
	0x83838383,0x87878787,0x8B8b8b8b,0x8F8f8f8f,0x93939393,0x97979797,0x9B9b9b9b,0x9F9f9f9f,0xA3a3a3a3,0xA7a7a7a7,0xABababab,0xAFafafaf,0xB3b3b3b3,0xB7b7b7b7,0xBBbbbbbb,0xBFbfbfbf,//[48]
	0xC3c3c3c3,0xC7c7c7c7,0xCBcbcbcb,0xCFcfcfcf,0xD3d3d3d3,0xD7d7d7d7,0xDBdbdbdb,0xDFdfdfdf,0xE3e3e3e3,0xE7e7e7e7,0xEBebebeb,0xEFefefef,0xF3f3f3f3,0xF7f7f7f7,0xFBfbfbfb,0xFFffffff,//[64]
	//======== END ========
};

#if false
//=== �r�b�g�}�b�v�t�H���g�p�O���C�X�P�[���o�̓R�[�h ===
void GrayDump(void)
{
	double rgb = 0;
	const int resolution = 65;	//{2|5|17|65}
	const double step = (1.0 / (double)(resolution - 1));
	_RPTN(_CRT_WARN, "const DWORD Gray%d[%d]={\n", resolution, resolution);
	_RPT0(_CRT_WARN, "//======== START ========\n");
	_RPT0(_CRT_WARN, "0x00000000,\n");
	for (int n = 1; n < resolution; n++) {
		int rgb = (int)((step * (double)n) * 255.0);
		_RPTN(_CRT_WARN, "0x00%02x%02x%02x,", rgb, rgb, rgb);
		if ((n % 16) == 0) {
			_RPTN(_CRT_WARN, "//[%2d]\n", n);
		}
		else if (n == (resolution - 1)) {
			_RPTN(_CRT_WARN, "//[%2d]\n", n);
		}
	}
	_RPT0(_CRT_WARN, "//======== END ========\n");
	_RPT0(_CRT_WARN, "};\n");
}
#endif // false

/**
* @brief	1bpp�̉摜��8bpp�̉摜�ɕϊ�����B
*
* @param	Bmp* _pbc	�r�b�g�}�b�v�����f�[�^�ւ̃|�C���^
* @param	GLYPHMETRICS* _pgm	�ϊ��������̃O���t���
*
* @return
* 	�Ȃ�
*/
static void convert_bpp1_to_bpp8(Bmp* _pbc, const GLYPHMETRICS* _pgm)
{
	int w_pix = _pgm->gmBlackBoxX;
	int h_pix = _pgm->gmBlackBoxY;
	int stride = (_pbc->numpix / _pgm->gmBlackBoxY);
	int stride4 = (w_pix + 0b0011) & (~0b0011);			//�Wbpp�摜�̂S�o�C�g���E�̃o�C�g��
	int bits_size = stride4 * h_pix;
	char* pFontBitmap = (char*)calloc(bits_size, sizeof(char));
	_ASSERT(pFontBitmap != NULL);
	//ZeroMemory(pFontBitmap, bits_size);
	for (int y = 0; y < h_pix; y++) {
		for (int x = 0; x < stride; x++) {
			int idxSrc = (y * stride + x);
			UINT bit8 = _pbc->pixel[idxSrc];
			int idxDest = (y * stride4) + (x * 8);
			for (int bitN = 0; bitN < 8; bitN++) {
				if ((idxDest + bitN) < bits_size) {
					//pFontBitmap[idxDest + bitN] = (bit8 & (0b10000000 >> bitN)) ? 1 : 0;	//0xFF : 0x00;
					if ((bit8 & (0b10000000 >> bitN)) != 0) {
						pFontBitmap[idxDest + bitN] = 1;
					}
				}
			}
		}
	}
	//�Â�1bpp�̃o�b�t�@�͍폜���ĐV���������8bpp�o�b�t�@�ɓ���ւ���B
	free(_pbc->pixel);
	_pbc->pixel = pFontBitmap;
	_pbc->numpix = bits_size;
}	//convert_bpp1_to_bpp8

/**
*  @brief
* 	�r�b�g�}�b�v�����P�����̕\���ʒu�𒲐����ăr�b�g�}�b�v����蒼���B
*
* @param	BitmapChar* _pbc	�r�b�g�}�b�v�����̃|�C���^�B���̃|�C���^���w���r�b�g�}�b�v�����f�[�^�̕\���ʒu�𒲐����ăo�b�t�@����蒼�����B
* @param	GLYPHMETRICS* _pgm	�ϊ��������̃O���t���
* @param	TEXTMETRIC* _ptxm	�ϊ����t�H���g�̌v���i�����̐��@�j���
*
* @return
* 	����
*/
static void build_bmp_char(Bmp* _pbc, const GLYPHMETRICS* _pgm, const TEXTMETRICW* _ptxm)
{
	//�]����o�b�t�@�����
	int	dest_width = _pgm->gmCellIncX;
	int dest_height = _ptxm->tmHeight;
	int dest_buf_size = dest_width * dest_height;
	char* pDest = (char*)calloc(dest_buf_size, sizeof(char));	//�]�����S�ĂO�ŏ������i�h�b�g�̖����ꏊ�i�������Ȃ��ꏊ�j�͂O�ɂȂ�j
	_ASSERT(pDest != NULL);
	//�]�����T�C�Y���v�Z�i�����͂S�̔{���j
	int width = _pgm->gmBlackBoxX;
	//int widthBytes = (width + 0b0011) & (~0b0011);	//�����̃o�C�g���͂S�̔{���ɍ��킹��
	int height = _pgm->gmBlackBoxY;
	int stride = _pbc->numpix / _pbc->height;	//�]�����o�b�t�@�̉���
	//
	for (int y = 0; y < height; y++)
	{
		int yp = (_ptxm->tmAscent - _pgm->gmptGlyphOrigin.y) + y;
		if ((yp < 0) || (yp >= dest_height))
		{
			//�㉺�����ɔ͈͊O�Ȃ珈�����Ȃ�
			continue;
		}
		for (int x = 0; x < width; x++)
		{
			int xp = _pgm->gmptGlyphOrigin.x + x;
			if ((xp < 0) || (xp >= dest_width))
			{
				//���E�����ɔ͈͊O�Ȃ珈�����Ȃ�
				continue;
			}
			//X,Y�ʒu����P�����z��̓ǂݏo���ʒu���Z�o
			//int read_idx = (y * widthBytes + x);
			int read_idx = (y * stride + x);
			if ((read_idx < 0) || (read_idx >= (int)_pbc->numpix))
			{
				//�]�����͈̔͂𒴂��Ă����珈�����Ȃ�
				continue;
			}
			//unsigned char dot = _pbc->pPix[y * stride + x];
			unsigned char dot = _pbc->pixel[read_idx];	//�]��������P�s�N�Z���ǂݏo��
			if (dot == 0x00)
			{
				//�F�R�[�h�O�͓��������Ȃ̂ŏ������Ȃ�
				continue;
			}
#ifndef USED2D
			/*
			* ToDo:�p���b�g�̉��Ԃ̐F���g�������O�����瑀��ł���悤�ɂ������B
			* ex)�Q�l�F�̎��A'0'->pal[13],'1'->pal[14]�Ƃ��B
			*/
			if (_pbc->aaLv == 2)
			{
				//�Q�l�̏ꍇ�́o�O�b�P�p�Ȃ̂łO�ȊO�͑S��'�e'�Ƃ���B
				dot = 0x0F;
			}
			else
			{
				//�A���`�G�C���A�X���~���w���dot�̒l�����ɁA�P�U�i�K(16�F)�̔Z�x�ɑΉ��������l�ɕϊ�����B
				dot = (unsigned char)((double)(16.0 / (double)(_pbc->aaLv - 1)) * (double)dot);
				if (dot > 0x0F) {
					dot = 0x0F;	//�p���b�g�͂P�U�F�����Ȃ��̂łP�T�𒴂��Ȃ��l�ɂ���B
				}
			}
#endif // USED2D
			pDest[yp * dest_width + xp] = dot;
		}
	}
	free(_pbc->pixel);
	_pbc->pixel = pDest;
	_pbc->numpix = dest_buf_size;
	_pbc->width = dest_width;
	_pbc->height = dest_height;
	return;
}	//build_bmp_char

#ifndef UNICODE
/**
* @brief	�}���`�o�C�g�����񂩂玟�̂P���������o��UINT�ɕϊ����ĕԂ��B\n
*			�n���ꂽ�|�C���^���}���`�o�C�g�������Z�łP�����i�߂�B
*
* @param	BYTE** p�F�}���`�o�C�g������̃|�C���^�̃|�C���^
*
* @return	UINT�F�}���`�o�C�g�����R�[�h
*/
static UINT get_MBC(BYTE** p) {
	UINT mbc = **p;	//�P�o�C�g��荞��
	if (IsDBCSLeadByte(mbc)) {
		mbc <<= 8;	//�P�o�C�g�ڂ͏�ʃo�C�g�֓����
		(*p)++;	//�S�p�����͂Q�o�C�g�ڂ���荞��
		mbc |= **p;	//�Q�o�C�g�ڂ͉��ʃo�C�g�ɓ����
	}
	(*p)++;	//�|�C���^�i�߂�
	return mbc;
}	//get_MBC
#endif // UNICODE

/**
* @brief	������i���C�h�����bUnocode�����j��Bmp�摜�̔z��Ƃ��Đ�������B\n
*
* @param	const TCHAR* _font_name : �t�H���g���i���C�h�����bUnocode�����j
* @param	int _font_size : �t�H���g�E�T�C�Y
* @param	int _bold : �����w��Ftrue�ő���
* @param	int _ggo : �A���`�E�G�C���A�X���~���w��\n
*			{GGO_BITMAP,GGO_GRAY2_BITMAP,GGO_GRAY4_BITMAP,GGO_GRAY8_BITMAP}\n
*			��WIN32PAI::GetGlyphOutline()�֐��Q��
* @param	const TCHAR* _text : �ϊ�������������i���C�h�����bUnocode�����j
*
* @return	Bmp*	�ϊ���̃r�b�g�}�b�v�����̔z��ւ̃|�C���^�B(�^�[�~�l�[�^�[�Ƃ��đS�����o��NULL��Bmp������)
*					���Ԃ��ꂽBmp�͕K��DeleteBmp()�ō폜���鎖
*
* @note
*		�w��̃t�H���g�ŏo�����r�b�g�}�b�v����Bmp�̔z����쐬���擪�̃|�C���^��Ԃ��B\n
*		�����C�h����(char)/Unocode����(wchar_t)���Ή��B\n
*		���R���p�C������TCHAR�����C�h����(char)��Unocode����(wchar_t)�ɐؑւ��B\n
*
*/
Bmp* CreateBmpChar(const TCHAR* _font_name, int _font_size, int _bold, int _ggo, const TCHAR* _text)
{
	//��]�s��
	MAT2	mat2{ {0,1},{0,0},{0,0},{0,1} };
	//�t�H���g�̐ݒ�`�쐬
	LOGFONT	lf;
	lf.lfHeight = _font_size;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;	//�����P�ʂ̉�]�p�x�����
	lf.lfOrientation = 0;
	if (_bold) {
		lf.lfWeight = FW_BOLD;	//�����ݒ�
	}
	else {
		lf.lfWeight = FW_NORMAL;
	}
	lf.lfItalic = FALSE;	//�Α�
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
#ifdef UNICODE
	lf.lfCharSet = ANSI_CHARSET;
#else
	lf.lfCharSet = SHIFTJIS_CHARSET;
#endif // UNICODE
	lf.lfOutPrecision = OUT_TT_PRECIS;	//OUT_DEFAULT_PRECIS
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = PROOF_QUALITY;
	lf.lfPitchAndFamily = (DEFAULT_PITCH | FF_MODERN);
	lf.lfFaceName[0] = '\0';
	//���̖����R�s�[�i��������������j
	if (_font_name != nullptr) {
		CopyMemory(lf.lfFaceName, _font_name, LF_FACESIZE * sizeof(TCHAR));
	}
	else {
		//�w�肪�������́u�l�r�����v�Ƃ���
		CopyMemory(lf.lfFaceName, _T("�l�r ����"), LF_FACESIZE * sizeof(TCHAR));
		//CopyMemory(lf.lfFaceName, _T(""), LF_FACESIZE * sizeof(TCHAR));
	}
	//�t�H���g����
	HFONT hFont = CreateFontIndirect(&lf);
	_ASSERT(hFont);
	if (hFont == NULL) {
		return	NULL;
	}
	// �f�o�C�X�Ƀt�H���g��I������
	HWND hWnd = GetConsoleWindow();	//���������̃R���\�[���̃E�B���h�E�n���h��
	HDC hdc = GetDC(hWnd);
	HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
	//���������t�H���g�̌v���f�[�^���擾����
	TEXTMETRICW	txm;		//�ϊ������t�H���g�̏�������\����
	GetTextMetricsW(hdc, &txm);	//�v���f�[�^���擾
	int aa_level = 2;
	switch (_ggo)
	{
	default:	//�Q�l
	case GGO_BITMAP:		aa_level = 2;		break;	//�Q�l
	case GGO_GRAY2_BITMAP:	aa_level = 5;		break;	//�T�K��
	case GGO_GRAY4_BITMAP:	aa_level = 17;	break;	//�P�V�K��
	case GGO_GRAY8_BITMAP:	aa_level = 65;	break;	//�U�T�K��
	}
	//�w��̃t�H���g�ŏo�����r�b�g�}�b�v�����ŕ�������쐬����B
	GLYPHMETRICS	gm{ 0,0,0,0,0,0 };	//�O���t�ݒ�f�[�^
	UINT code = 0;
	//������̕����������߂�B
#ifdef UNICODE
	size_t length = (int)wcslen(_text);	//���C�h����(Unicode)�̕������𐔂���
	size_t buff_len = length + 1;
	const TCHAR* code_ary = (TCHAR*)_text;
#else
	//�}���`�o�C�g�����̏ꍇ�͑S�p�����Q��������UINT�ɕϊ�����
	size_t length = _mbstrlen(_text);	//�S�p�����p���P�����Ƃ��Đ�����
	size_t buff_len = length + 1;
	UINT* code_ary = (UINT*)_alloca(buff_len * sizeof(UINT));	//'\0'�܂ޕ�������Bmp���m�ۂ���
	memset(code_ary, 0, buff_len * sizeof(UINT));
	const BYTE* p = (BYTE*)_text;
	for (int i = 0; (*p != '\0') && (i < length); i++) {
		code_ary[i] = get_MBC((BYTE**)&p);
	}
#endif // UNICODE
	//Bmp�p�o�b�t�@�𕶎������m�ہi�Ō��'\0'�p��Bmp���܂߂�j�i�S�ĂO�ŏ������j
	Bmp* pBmpChr = (Bmp*)calloc(buff_len, sizeof(Bmp));
	_ASSERT(pBmpChr != NULL);
	//�P�����ɕt���P��Bmp�I�u�W�F�N�g�𐶐�����Bmp�̔z��Ɋi�[���čs��
	for (size_t txn = 0; txn < length; txn++) {
		code = (UINT)code_ary[txn];
		//���ꂩ�琶�����镶���r�b�g�}�b�v�f�[�^�̃o�C�g�����擾����B
		int buff_size = GetGlyphOutline(hdc, code, _ggo, &gm, 0, NULL, &mat2);
		//if (buff_size > 0)
		if (code != 0)
		{
			//�擾�����T�C�Y���̃o�b�t�@���m�ۂ���B�f �f�󔒂̏ꍇ�͂O(zero)���Ԃ邪�A���̂܂�malloc����B
			pBmpChr[txn].pixel = (char*)calloc(buff_size, sizeof(char));
			//�f �f�󔒂̏ꍇbuff_size���O�ł�gm�ɂ͐������l���Z�b�g����Ă���l���B
			GetGlyphOutline(hdc, code, _ggo, &gm, buff_size, pBmpChr[txn].pixel, &mat2);
			if (_ggo == GGO_BITMAP)
			{
				//�Pbpp�̃r�b�g�}�b�v�͕\�����ɂ����̂łWbpp�ɕϊ�����B
				pBmpChr[txn].numpix = buff_size;		//�o�b�t�@�T�C�Y
				convert_bpp1_to_bpp8(&pBmpChr[txn], &gm);	//�P�r�b�g/�s�N�Z���摜���W�r�b�g/�s�N�Z���摜�ɕϊ�
				buff_size = pBmpChr[txn].numpix;
			}
			pBmpChr[txn].width = gm.gmBlackBoxX;	//���s�N�Z����
			pBmpChr[txn].height = gm.gmBlackBoxY;	//�c�s�N�Z����
			//�S�Ă̕����摜���W�r�b�g/�s�N�Z���̉摜�Ƃ��Ĉ���
			pBmpChr[txn].colbit = 8;				//�W�r�b�g/pixel�摜
			pBmpChr[txn].numpix = buff_size;		//�o�b�t�@�T�C�Y
			pBmpChr[txn].aaLv = aa_level;			//�A���`�G�C���A�X���~�����x��
			pBmpChr[txn].wch = code;				//�ϊ����̕����R�[�h
			//�����ʒu�𒲐����ăo�b�t�@����蒼���B
			build_bmp_char(&pBmpChr[txn], &gm, &txm);
#ifdef USED2D
			//�ʒu�������ς񂾉摜�Ƀp���b�g(RGBQUAD�^)���m�ۂ���
			pBmpChr[txn].pal = 0;	//COLORREF�^�͊m�ۂ��Ȃ�
			pBmpChr[txn].pal_rgb = (RGBQUAD*)calloc(NUM_D2D_PAL, sizeof(RGBQUAD));
			pBmpChr[txn].numpal = NUM_D2D_PAL;	//�p���b�g����256�F�Œ�Ƃ���
			switch (_ggo)
			{
			default:	//�Q�l
			case GGO_BITMAP:
				memcpy_s(pBmpChr[txn].pal_rgb, NUM_D2D_PAL * sizeof(RGBQUAD), Gray2, sizeof(Gray2));
				break;	//�Q�l
			case GGO_GRAY2_BITMAP:
				memcpy_s(pBmpChr[txn].pal_rgb, NUM_D2D_PAL * sizeof(RGBQUAD), Gray5, sizeof(Gray5));
				break;	//�T�K��
			case GGO_GRAY4_BITMAP:
				memcpy_s(pBmpChr[txn].pal_rgb, NUM_D2D_PAL * sizeof(RGBQUAD), Gray17, sizeof(Gray17));
				break;	//�P�V�K��
			case GGO_GRAY8_BITMAP:
				memcpy_s(pBmpChr[txn].pal_rgb, NUM_D2D_PAL * sizeof(RGBQUAD), Gray65, sizeof(Gray65));
				break;	//�U�T�K��
			}
#endif // USED2D
		}
	}
	//�f�o�C�X�̃t�H���g�I������������i���ɖ߂��j
	SelectObject(hdc, oldFont);
	DeleteObject(hFont);
	ReleaseDC(hWnd, hdc);
	_ASSERT(pBmpChr);
	return	pBmpChr;
}	//CreateBmpChar

/**
* @brief	������i���C�h�����bUnocode�����j���P���̉摜�Ƃ��Đ�������B
*
* @param	const TCHAR* _font_name	�t�H���g���i���C�h�����bUnocode�����j
* @param	int _font_size : �t�H���g�E�T�C�Y
* @param	int _bold : �����w��Ftrue�ő���
* @param	int _ggo : �A���`�E�G�C���A�X���~���w��\n
*			{GGO_BITMAP,GGO_GRAY2_BITMAP,GGO_GRAY4_BITMAP,GGO_GRAY8_BITMAP}\n
*			��WIN32PAI::GetGlyphOutline()�֐��Q��
* @param	const TCHAR* _text	�ϊ�������������i���C�h�����bUnocode�����j
*
* @return	Bmp* : �ϊ���̃r�b�g�}�b�v�����̔z��ւ̃|�C���^�B
*
* @note		�w��̃t�H���g�ŏo�����r�b�g�}�b�v�������Bmp�摜���쐬���A���̃|�C���^��Ԃ��B\n
* 			CreateBmpChar()�œ���ꂽBmp�̔z��i�P�������Ƃ̉摜�̔z��j��A�����ĂP���̉摜�ɂ��A
* 			���̃o�b�t�@�iBmpString�j�̃|�C���^��Ԃ��B
*			�����C�h����(char)/Unocode����(wchar_t)���Ή��B\n
*			���R���p�C������TCHAR�����C�h����(char)��Unocode����(wchar_t)�ɐؑւ��B\n
*/
Bmp* CreateBmpString(const TCHAR* _font_name, int _font_size, int _bold, int _ggo, const TCHAR* _text)
{
	//�悸�A�P�������Ƃ̉摜�̔z����쐬����B
	Bmp* pBc = CreateBmpChar(_font_name, _font_size, _bold, _ggo, _text);
	_ASSERT(pBc);
	int n = 0;
	int xpos = 0;
	if (pBc != NULL) {
		while (pBc[n].pixel != NULL) {
			//�S�Ă̕������q�����킹�����̕��i�s�N�Z�����j���v�Z����
			xpos += pBc[n].width;	//���̕����̉������ʒu���Z�b�g
			n++;	//���̕���
		}
	}
	int width = xpos; //������摜�S�̂̕��i�s�N�Z�����j
	int height = _font_size;	//���̕�����摜�̍����i�s�N�Z�����j
	Bmp* bm_str = (Bmp*)calloc(1, sizeof(Bmp));	//Bmp�I�u�W�F�N�g�P�����F�S�Ă̕���Bmp���P��Bmp�ɏW�񂷂�B
	//ZeroMemory(bm_str, sizeof(Bmp));
	bm_str->numpix = (width * height * sizeof(char));	//������摜�̃s�N�Z���T�C�Y
	bm_str->pixel = (char*)calloc(bm_str->numpix, sizeof(char));	//�摜�o�b�t�@�m��
	bm_str->aaLv = pBc[0].aaLv;	//�擪����[0]���~���R�[�h���g��
	bm_str->width = width;		//�摜�̕��i�s�N�Z���j
	bm_str->height = height;	//�摜�����i�s�N�Z���j
	//�p���b�g�쐬�ƃR�s�[
	bm_str->colbit = pBc[0].colbit;	//�擪����[0]�̃r�b�g��/�s�N�Z�����g��
	bm_str->numpal = pBc[0].numpal;	//�擪����[0]�̃p���b�g��
#ifdef USED2D
	bm_str->pal_rgb = (RGBQUAD*)calloc(pBc[0].numpal, sizeof(RGBQUAD));	//[0]�Ɠ����p���b�g���m�ۂ���
	memcpy_s(bm_str->pal_rgb, NUM_D2D_PAL * sizeof(RGBQUAD), pBc[0].pal_rgb, NUM_D2D_PAL * sizeof(RGBQUAD));
#endif // USED2D
	//
	n = 0;
	xpos = 0;
	while (pBc[n].pixel != NULL) {
		int pn = 0;
		for (int y = 0; y < pBc[n].height; y++) {
			for (int x = 0; x < pBc[n].width; x++) {
				int xp = (xpos + x);
				if ((xp >= 0) && (xp < bm_str->width) && (y >= 0) && (y < bm_str->height)) {
					//�t���[���o�b�t�@�͈̔͂𒴂��ĂȂ���Ώ�������
					bm_str->pixel[y * bm_str->width + xp] = pBc[n].pixel[pn];	//�P�s�N�Z����������
				}
				pn++;	//���̃s�N�Z���ǂݏo���ʒu
			}
		}
		xpos += pBc[n].width;	//���̉������ʒu
		n++;
	}
	//�m�ۂ����������̊J��
	n = 0;
	while (pBc[n].pixel != NULL) {
		free(pBc[n].pixel);
		pBc[n].pixel = NULL;
#ifdef USED2D
		if (pBc[n].pal_rgb != NULL) {
			free(pBc[n].pal_rgb);
			pBc[n].pal_rgb = NULL;
		}
		if (pBc[n].pal != NULL) {
			free(pBc[n].pal);
			pBc[n].pal = NULL;
		}
#endif // USED2D
		n++;
	}
	free(pBc);
	return bm_str;
}	//CreateBmpString

/**
* @brief	������i���C�h�����bUnocode�����j�������w�肵�ĂP���̉摜�Ƃ��Đ�������B
*
* @param	const TCHAR* _font_name	�t�H���g���i���C�h�����bUnocode�����j
* @param	int _font_size : �t�H���g�E�T�C�Y
* @param	int _bold : �����w��Ftrue�ő���
* @param	int _ggo : �A���`�E�G�C���A�X���~���w��\n
*			{GGO_BITMAP,GGO_GRAY2_BITMAP,GGO_GRAY4_BITMAP,GGO_GRAY8_BITMAP}\n
*			��WIN32PAI::GetGlyphOutline()�֐��Q��
* @param	const char* _format�F�����w�蕶����
* @param	...�F�ϒ�����
*
* @return	Bmp* : �ϊ���̃r�b�g�}�b�v�����̔z��ւ̃|�C���^�B
*
* @note		�w��̃t�H���g�ŏo�����r�b�g�}�b�v��������쐬���A���̃|�C���^��Ԃ��B\n
* 			CreateBmpString�œ���ꂽBmp�̃|�C���^��Ԃ��B\n
*			�����C�h����(char)/Unocode����(wchar_t)���Ή��B\n
*			���R���p�C������TCHAR�����C�h����(char)��Unocode����(wchar_t)�ɐؑւ��B\n
*/
Bmp* CreateBmpStringF(const TCHAR* _font_name, int _font_size, int _bold, int _ggo, bool _zenkaku, const TCHAR* _format, ...)
{
	Bmp* p_bmp = nullptr;
	va_list ap;
	va_start(ap, _format);
	//VPrintStringFA(_zenkaku, _format, ap);
#ifdef UNICODE
	size_t length = _vscwprintf(_format, ap) + 1;	//'\0'�܂܂Ȃ��̂Ł{�P���Ă���
	wchar_t* buf = (wchar_t*)_malloca(length * sizeof(wchar_t));
	vswprintf_s(buf, length, _format, ap);
	if (_zenkaku == true) {
		//�S�đS�p�ɕϊ����Ă��琶��
		wchar_t* p = HanToZenW(buf);
		p_bmp = CreateBmpString(_font_name, _font_size, _bold, _ggo, p);
		free(p);
	}
	else {
		//���p�̂܂ܐ���
		p_bmp = CreateBmpString(_font_name, _font_size, _bold, _ggo, buf);
	}
#else
	//VPrintStringFA(_zenkaku, _format, ap);
	size_t length = _vscprintf(_format, ap) + 1;	//'\0'�܂܂Ȃ��̂Ł{�P���Ă���
	char* buf = (char*)_malloca(length);
	vsprintf_s(buf, length, _format, ap);
	if (_zenkaku == true) {
		//�S�đS�p�ɕϊ����Ă��琶��
		char* p = HanToZen(buf);
		p_bmp = CreateBmpString(_font_name, _font_size, _bold, _ggo, p);
		free(p);
	}
	else {
		//���p�̂܂ܐ���
		p_bmp = CreateBmpString(_font_name, _font_size, _bold, _ggo, buf);
	}
#endif // UNICODE
	va_end(ap);
	return p_bmp;
}	//CreateBmpStringF

Bmp* CreateBmpStringF(const TCHAR* _font_name, int _font_size, int _bold, bool _zenkaku, const TCHAR* _format, ...)
{
	Bmp* p_bmp = nullptr;
	va_list ap;
	va_start(ap, _format);
	//VPrintStringFA(_zenkaku, _format, ap);
#ifdef UNICODE
	size_t length = _vscwprintf(_format, ap) + 1;	//'\0'�܂܂Ȃ��̂Ł{�P���Ă���
	wchar_t* buf = (wchar_t*)_malloca(length * sizeof(wchar_t));
	vswprintf_s(buf, length, _format, ap);
	if (_zenkaku == true) {
		//�S�đS�p�ɕϊ����Ă��琶��
		wchar_t* p = HanToZenW(buf);
		p_bmp = CreateBmpString(_font_name, _font_size, _bold, GGO_BITMAP, p);
		free(p);
	}
	else {
		//���p�̂܂ܐ���
		p_bmp = CreateBmpString(_font_name, _font_size, _bold, GGO_BITMAP, buf);
	}
#else
	//VPrintStringFA(_zenkaku, _format, ap);
	size_t length = _vscprintf(_format, ap) + 1;	//'\0'�܂܂Ȃ��̂Ł{�P���Ă���
	char* buf = (char*)_malloca(length);
	vsprintf_s(buf, length, _format, ap);
	if (_zenkaku == true) {
		//�S�đS�p�ɕϊ����Ă��琶��
		char* p = HanToZen(buf);
		p_bmp = CreateBmpString(_font_name, _font_size, _bold, GGO_BITMAP, p);
		free(p);
	}
	else {
		//���p�̂܂ܐ���
		p_bmp = CreateBmpString(_font_name, _font_size, _bold, GGO_BITMAP, buf);
	}
#endif // UNICODE
	va_end(ap);
	return p_bmp;
}	//CreateBmpStringF

//================================================================
// �L�[���͊֌W
//================================================================
/**
 * @brief	�L�[��񃊃Z�b�g
 */
void ResetKeyMap(void)
{
	for (int count = 0; count < 8; count++) {
		g_ConioKeyMap[count] = 0;
	}
}

/**
 * @brief	�L�[�{�[�h�E�}�E�X����
 *
 * @param	port [����] �|�[�g�ԍ�(P*_*)
 * @return	���͒l
 */
int InputKeyMouse(int port)
{
	DWORD event = 0;
	DWORD read = 0;
	volatile PINPUT_RECORD input_record;
	KEY_EVENT_RECORD* key_event;
	MOUSE_EVENT_RECORD* mouse_event;

	// �L�[�{�[�h�C�x���g�`�F�b�N
#ifdef USED2D
	if (GetNumberOfConsoleInputEvents(g_InputHandleD2D, &event) && event)
#else
	if (GetNumberOfConsoleInputEvents(g_InputHandle, &event) && event)
#endif // USED2D
	{
		read = 0;
		input_record = (PINPUT_RECORD)_malloca(event * sizeof(INPUT_RECORD));

#ifdef USED2D
		if (ReadConsoleInput(g_InputHandleD2D, input_record, event, &read) && read)
#else
		if (ReadConsoleInput(g_InputHandle, input_record, event, &read) && read)
#endif // USED2D
		{
			//input_record = input_record;
			for (unsigned int count = 0; count < read; count++, input_record++) {
				switch (input_record->EventType) {
				case KEY_EVENT: {
					key_event = &input_record->Event.KeyEvent;
					if (key_event->wVirtualKeyCode > 0x0FF) {
						break;
					}
					if (key_event->bKeyDown) {
						g_ConioKeyMap[key_event->wVirtualKeyCode >> 5] |= (0x01 << (key_event->wVirtualKeyCode & 31));
					}
					else {
						g_ConioKeyMap[key_event->wVirtualKeyCode >> 5] &= ~(0x01 << (key_event->wVirtualKeyCode & 31));
					}
					if (key_event->dwControlKeyState & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED)) {
						g_ConioKeyMap[VK_MENU >> 5] |= (0x01 << (VK_MENU & 31));
					}
					else {
						g_ConioKeyMap[VK_MENU >> 5] &= ~(0x01 << (VK_MENU & 31));
					}
					if (key_event->dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) {
						g_ConioKeyMap[VK_CONTROL >> 5] |= (0x01 << (VK_CONTROL & 31));
					}
					else {
						g_ConioKeyMap[VK_CONTROL >> 5] &= ~(0x01 << (VK_CONTROL & 31));
					}
					if (key_event->dwControlKeyState & SHIFT_PRESSED) {
						g_ConioKeyMap[VK_SHIFT >> 5] |= (0x01 << (VK_SHIFT & 31));
					}
					else {
						g_ConioKeyMap[VK_SHIFT >> 5] &= ~(0x01 << (VK_SHIFT & 31));
					}
					break;
				}
				case MOUSE_EVENT: {
					mouse_event = &input_record->Event.MouseEvent;
					g_ConioMousePosition = mouse_event->dwMousePosition;
					if (mouse_event->dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
						g_ConioKeyMap[VK_LBUTTON >> 5] |= (0x01 << (VK_LBUTTON & 31));
					}
					else {
						g_ConioKeyMap[VK_LBUTTON >> 5] &= ~(0x01 << (VK_LBUTTON & 31));
					}
					if (mouse_event->dwButtonState & FROM_LEFT_2ND_BUTTON_PRESSED) {
						g_ConioKeyMap[VK_MBUTTON >> 5] |= (0x01 << (VK_MBUTTON & 31));
					}
					else {
						g_ConioKeyMap[VK_MBUTTON >> 5] &= ~(0x01 << (VK_MBUTTON & 31));
					}
					if (mouse_event->dwButtonState & RIGHTMOST_BUTTON_PRESSED) {
						g_ConioKeyMap[VK_RBUTTON >> 5] |= (0x01 << (VK_RBUTTON & 31));
					}
					else {
						g_ConioKeyMap[VK_RBUTTON >> 5] &= ~(0x01 << (VK_RBUTTON & 31));
					}
					break;
				}
				default:
					break;
				}
			}
		}
	}

	// �}�E�X���W��Ԃ�
	switch (port) {
	case PM_CURX:
		return g_ConioMousePosition.X + 1;
	case PM_CURY:
		return g_ConioMousePosition.Y + 1;
	default:
		break;
	}
	// �L�[��Ԃ�Ԃ�
	return (g_ConioKeyMap[(port & 0x0FF) >> 5] & (0x01 << (port & 31))) != 0;
}

//================================================================
// �g���L�[����
//================================================================
/**
* @brief	�P��L�[�̓��́B�����̃R���\�[���E�B���h�E�Ƀt�H�[�J�X���������Ă��鎞�������͂���B
*
* @return	SHORT\tWin32api��"GetAsyncKeyState()"�Ɠ���
*/
SHORT GetKey(int _vk)
{
	if (GetForegroundWindow() != g_hConWnd) {
		//�t�H�[�J�X���O��Ă���B
		return 0;
	}
	//�t�H�[�J�X���������Ă��鎞�������͂���B
	return GetAsyncKeyState(_vk);
}	//GetKey

/**
* @brief	�L�[���͂�҂B�����̃R���\�[���E�B���h�E�Ƀt�H�[�J�X���������Ă��鎞�������͂���B
*
* @return	SHORT	Win32api��"GetAsyncKeyState()"�Ɠ���
*/
SHORT WaitKey(int _vk)
{
	SHORT k = 0;
	//�t�H�[�J�X���������Ă��鎞�������͂���B
	do {
		k = GetKey(_vk);
	} while (!k);
	return k;
}	//GetKey

/**
* @brief	�S�ẴL�[�̓��́B�����̃R���\�[���E�B���h�E�Ƀt�H�[�J�X���������Ă��鎞�������͂���B\n
* ���̊֐����Ăяo������A�S�ẴL�[���͂�ChkKeyEdge()/ChkKeyPress()�Ŕ���ł���B
*
* @param	_vk	���z�L�[�R�[�h�FPress�L�[�i�����L�[�j���͔��肷��
* @param	_chktype	�L�[���͔�����@�̑I���F1=Edge����/0=Press����
*
* @return	int\n
*	�L�[�n�m�Ȃ�P\n
*	�L�[�n�e�e�Ȃ�O
*/
int GetKeyEx(int _vk, int _chktype)
{
	GetKeyAll();
	if (_chktype == 1) {
		//Edge�L�[���͔���
		return ChkKeyEdge(_vk);
	}
	//Press�L�[���͔���
	return ChkKeyPress(_vk);
}
/**
* @brief	�S�ẴL�[�̓��́B�����̃R���\�[���E�B���h�E�Ƀt�H�[�J�X���������Ă��鎞�������͂���B\n
* ���̊֐����Ăяo������A�S�ẴL�[���͂�ChkKeyEdge()/ChkKeyPress()�Ŕ���ł���B
*
* @return
*	�Ȃ�
*/
void GetKeyAll(void)
{
	if (GetForegroundWindow() != g_hConWnd) {
		//�t�H�[�J�X���O��Ă���B
		return;
	}
	//�t�H�[�J�X���������Ă��鎞�������͂���B
	//�S�L�[����͂�Edge��Press�����B
	for (int vk = 0; vk < NUM_KEYS; vk++) {
		//���݂̉���������Ԃ��Z�b�g����B
		g_KeyPress[vk] = (int)((GetAsyncKeyState(vk) & (~0x1)) != 0);
		//�O��n�e�e�ˍ���n�m�̎������n�m�ɂ���B
		g_KeyEdge[vk] = (int)((g_KeyPress[vk] != 0) && (g_KeyLast[vk] == 0));
		//�O��̏�Ԃ��X�V����
		g_KeyLast[vk] = g_KeyPress[vk];
	}
}	//GetKeyAll

/**
* @brief	Edge�L�[���͔���FGetKeyAll()�œ��͂����L�[���ɂ���Edge�L�[�i�g���K�[�L�[�j���͔��肷��
*
* @param	vk	���z�L�[�R�[�h
*
* @return	int\n
*	�L�[�n�m�Ȃ�P\n
*	�L�[�n�e�e�Ȃ�O
*/
int ChkKeyEdge(int _vk) {
	return g_KeyEdge[_vk & 0xFF];
}	//ChkKeyEdge

/**
* @brief	Press�L�[���͔���FGetKeyAll()�œ��͂����L�[���ɂ���Press�L�[�i�����L�[�j���͔��肷��
*
* @param	vk	���z�L�[�R�[�h
*
* @return	int\n
*	�L�[�n�m�Ȃ�P\n
*	�L�[�n�e�e�Ȃ�O
*/
int ChkKeyPress(int _vk) {
	return g_KeyPress[_vk & 0xFF];
}	//ChkKeyPress

//================================================================
// �W���C�p�b�h���͊֌W
//================================================================
/**
 * @brief	�W���C�p�b�h����
 *
 * @param	port [����] �|�[�g�ԍ�(P*_*)
 * @return	���͒l
 */
int InputJoystick(int port)
{
	JOYINFO	joy_info;
	int id;
	int func;

	// �Q�[���p�b�h����
	if ((port & 0xfe00) == 0x0200) {
		id = (port & 0x01f0) >> 4;
		func = port & 0x0f;

		switch (func) {
		case 0:
		case 1:
		case 2:
		case 3:
			if (joyGetPos(id, &joy_info) != JOYERR_NOERROR) {
				return -1;
			}
			switch (func) {
			case 0:
				return joy_info.wXpos;
			case 1:
				return joy_info.wYpos;
			case 2:
				return joy_info.wZpos;
			case 3:
				return joy_info.wButtons;
			}
			break;
		default:
			break;
		}
		return 0;
	}
	return -1;
}

#if FALSE
/**
 * @brief	�W���C�p�b�h����(XInput�Ή�)
 *
 * @param	port [����] �|�[�g�ԍ�(P*_*)
 * @return	���͒l
 */
int InputJoystickX(int port)
{
	XINPUT_STATE controller_state[4];	// XInput�R���g���[�����
	int id;
	int func;
	unsigned int  result;

	// �Q�[���p�b�h����
	if ((port & 0xfe00) == 0x0200) {	//0x200�`0x236
		id = (port & 0x01f0) >> 4;		//bit4�`8(5bits)���R���g���[���ԍ�
		func = port & 0x0f;				//bit0�`3(4bits)���{�^���ԍ�

		// Simply get the state of the controller from XInput.
		result = XInputGetState(id, &controller_state[id]);
		if (result == ERROR_SUCCESS) {
			switch (func) {
			case 0:
				return controller_state[id].Gamepad.sThumbLX;
			case 1:
				return controller_state[id].Gamepad.sThumbLY;
			case 2:
				return controller_state[id].Gamepad.bLeftTrigger;
			case 3:
				return controller_state[id].Gamepad.sThumbRX;
			case 4:
				return controller_state[id].Gamepad.sThumbRY;
			case 5:
				return controller_state[id].Gamepad.bRightTrigger;
			case 6:
				return controller_state[id].Gamepad.wButtons;
			}
		}
	}
	return -1;
}
#else	//FALSE
/**
 * @brief	�W���C�p�b�h����(XInput�Ή�)
 *
 * @param	id		�R���g���[���ԍ�(ID)�F�O�`
 * @param	port [����] �|�[�g�ԍ�(P*_*)
 *
 * @retval	0		����I��
 * @retval	1�ȏ�	Joystick�̓��͒l
 * @retval	-1		�G���[
 *
 * @note
 *	LR�̃X�e�B�b�N�͒��S���獶�E�Ɉړ�����ۂ̃f�b�h�]�[�����p�ӂ���Ă���
 *	�i�f�b�h�]�[���͍��E�Ɉړ������Ƃ݂Ȃ��Ȃ��G���A�̂��Ɓj
 *	 #define XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  7849
 *	 #define XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 8689
 */
int InputJoystickX(int id, int port)
{
	XINPUT_STATE controller_state;	// XInput�R���g���[�����

	if (XInputGetState(id, &controller_state) != ERROR_SUCCESS) {
		return -1;
	}

	switch (port) {
	case PJX_LXPOS:
		return controller_state.Gamepad.sThumbLX;
	case PJX_LYPOS:
		return controller_state.Gamepad.sThumbLY;
	case PJX_LTRG:
		return controller_state.Gamepad.bLeftTrigger;
	case PJX_RXPOS:
		return controller_state.Gamepad.sThumbRX;
	case PJX_RYPOS:
		return controller_state.Gamepad.sThumbRY;
	case PJX_RTRG:
		return controller_state.Gamepad.bRightTrigger;
	case PJX_BTNS:
		if (controller_state.dwPacketNumber) {
			return controller_state.Gamepad.wButtons;
		}
	}
	return 0;
}	//InputJoystickX
#endif // FALSE

//================================================================
// �T�E���h�֌W
//================================================================
/**
* @brief	�T�E���h �t�@�C�����J��
*
* @param	path [����] �t�@�C����
* @retval	��0	�T�E���h �n���h��
* @retval	0	�G���[
*/
int* MciOpenSound(const char* path)
{
	union {
		MCI_WAVE_OPEN_PARMSA	wave_param;
		MCI_OPEN_PARMSA			open_param;
	} MciParam;
	TCHAR error_str[256];
	const char midi_ext[] = ".mid|.midi|.rmi";
	const char wave_ext[] = ".wav|.wave";
	const char mp3_ext[] = ".mp3";
	char input_ext[_MAX_EXT];
	DWORD_PTR mci_command;
	MCIERROR mci_error;
	MciSoundInfo* sound_info;

	sound_info = (MciSoundInfo*)calloc(1, sizeof(MciSoundInfo));
	if (sound_info == NULL) {
		return 0;
	}
	ZeroMemory(sound_info, sizeof(*sound_info));
	ZeroMemory(&MciParam, sizeof(MciParam));
	_splitpath_s(path, NULL, 0, NULL, 0, NULL, 0, input_ext, sizeof(input_ext));
	_strlwr_s(input_ext, strlen(input_ext) + 1);
	mci_command = MCI_OPEN_TYPE | MCI_OPEN_ELEMENT;
	if (strstr(midi_ext, input_ext)) {
		sound_info->device_type = MCI_DEVTYPE_SEQUENCER;
		lstrcpynA(sound_info->path, path, MAX_PATH);
		MciParam.open_param.lpstrDeviceType = (LPCSTR)MCI_DEVTYPE_SEQUENCER;
		MciParam.open_param.lpstrElementName = sound_info->path;
		mci_command |= MCI_OPEN_TYPE_ID;
	}
	else if (strstr(wave_ext, input_ext)) {
		sound_info->device_type = MCI_DEVTYPE_WAVEFORM_AUDIO;
		lstrcpynA(sound_info->path, path, MAX_PATH);
		MciParam.wave_param.lpstrDeviceType = (LPCSTR)MCI_DEVTYPE_WAVEFORM_AUDIO;
		MciParam.wave_param.lpstrElementName = sound_info->path;
		mci_command |= MCI_OPEN_TYPE_ID;
		// MciParam.wave_param.dwBufferSeconds	= 60;
		// mci_command |= MCI_WAVE_OPEN_BUFFER;
	}
	else if (strstr(mp3_ext, input_ext)) {
		sound_info->device_type = MCI_DEVTYPE_DIGITAL_VIDEO;
		lstrcpynA(sound_info->path, path, MAX_PATH);
		MciParam.open_param.lpstrDeviceType = "MPEGVideo";
		MciParam.open_param.lpstrElementName = sound_info->path;
	}
	else {
		free(sound_info);
		return 0;
	}
	mci_error = mciSendCommandA(0, MCI_OPEN, mci_command, (DWORD_PTR)&MciParam);
	if (mci_error != 0) {
		free(sound_info);
		mciGetErrorString(mci_error, error_str, sizeof(error_str) / sizeof(TCHAR));
		MessageBox(NULL, error_str, NULL, MB_ICONWARNING);
		return 0;
	}
	sound_info->device_id = MciParam.open_param.wDeviceID;
	return (int*)sound_info;
}	//MciOpenSound

/**
 * @brief	�T�E���h �t�@�C�������
 *
 * @param	sound_id [����] �T�E���h �n���h��
 */
void MciCloseSound(int* sound_id)
{
	MciSoundInfo* sound_info;

	if (!sound_id) {
		return;
	}
	sound_info = (MciSoundInfo*)sound_id;
	if (sound_info->device_id) {
		mciSendCommand(sound_info->device_id, MCI_CLOSE, 0, 0);
		sound_info->device_id = 0;
	}
	free(sound_info);
}	//MciCloseSound

/**
 * @brief	�T�E���h���Đ�����
 *
 * @param	sound_id [����] �T�E���h �n���h��
 * @param	repeat [����] ���[�v�L��
 */
void MciPlaySound(int* sound_id, int repeat)
{
	MciSoundInfo* sound_info;
	DWORD_PTR mci_command;
	MCI_PLAY_PARMS play_param;

	if (!sound_id) {
		return;
	}
	sound_info = (MciSoundInfo*)sound_id;
	if (!sound_info->device_id) {
		return;
	}
	sound_info->repeat = repeat;
	ZeroMemory(&play_param, sizeof(play_param));
	mci_command = 0;
	if (repeat) {
		switch (sound_info->device_type) {
		case MCI_DEVTYPE_DIGITAL_VIDEO:
			mci_command |= (MCI_FROM | MCI_DGV_PLAY_REPEAT);
			play_param.dwFrom = 0;
			break;
		case MCI_DEVTYPE_SEQUENCER:
		case MCI_DEVTYPE_WAVEFORM_AUDIO:
			break;
		default:
			break;
		}
	}
	mciSendCommand(sound_info->device_id, MCI_SEEK, MCI_SEEK_TO_START, 0);
	mciSendCommand(sound_info->device_id, MCI_PLAY, mci_command, (DWORD_PTR)&play_param);
}	//MciPlaySound

/**
 * @brief	�T�E���h�Đ����~����
 *
 * @param	sound_id [����] �T�E���h �n���h��
 */
void MciStopSound(int* sound_id)
{
	MciSoundInfo* sound_info;

	if (!sound_id) {
		return;
	}
	sound_info = (MciSoundInfo*)sound_id;
	if (!sound_info->device_id) {
		return;
	}
	sound_info->repeat = 0;
	mciSendCommand(sound_info->device_id, MCI_STOP, MCI_WAIT, 0);
	mciSendCommand(sound_info->device_id, MCI_SEEK, MCI_SEEK_TO_START, 0);
}	//MciStopSound

/**
 * @brief	�T�E���h�Đ���Ԃ̎擾
 *
 * @param	sound_id [����] �T�E���h �n���h��
 * @return	�Đ����Ȃ�� 0 �ȊO��Ԃ�
 */
int MciCheckSound(int* sound_id)
{
	MciSoundInfo* sound_info;
	MCI_STATUS_PARMS status_param;

	if (!sound_id) {
		return 0;
	}
	sound_info = (MciSoundInfo*)sound_id;
	if (!sound_info->device_id) {
		return 0;
	}
	ZeroMemory(&status_param, sizeof(status_param));
	status_param.dwItem = MCI_STATUS_MODE;
	if (mciSendCommand(sound_info->device_id, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&status_param)) {
		return 0;
	}
	return status_param.dwReturn == MCI_MODE_PLAY;
}	//MciCheckSound

/**
 * @brief	���[�v�Đ��̋����X�V
 *
 * @param	sound_id [����] �T�E���h �n���h��
 * @note
 *	�T�E���h����~�����瓯���T�E���h���Đ�����
 *	�X�V���͉��ʐݒ肪�W���l�ɖ߂�̂ōĐݒ���s���K�v������
 */
void MciUpdateSound(int* sound_id)
{
	MciSoundInfo* sound_info;
	MCI_STATUS_PARMS status_param;

	if (!sound_id) {
		return;
	}
	sound_info = (MciSoundInfo*)sound_id;
	if (!sound_info->device_id || !sound_info->repeat) {
		return;
	}

	switch (sound_info->device_type) {
	case MCI_DEVTYPE_DIGITAL_VIDEO:
		break;
	case MCI_DEVTYPE_SEQUENCER:
	case MCI_DEVTYPE_WAVEFORM_AUDIO:
		ZeroMemory(&status_param, sizeof(status_param));
		status_param.dwItem = MCI_STATUS_MODE;
		if (!mciSendCommand(sound_info->device_id, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&status_param)) {
			if (status_param.dwReturn == MCI_MODE_NOT_READY) {
			}
			else if (status_param.dwReturn == MCI_MODE_STOP) {
				if (sound_info->repeat > 0) {
					mciSendCommand(sound_info->device_id, MCI_SEEK, MCI_SEEK_TO_START, 0);
					mciSendCommand(sound_info->device_id, MCI_PLAY, 0, 0);
				}
			}
		}
		break;
	default:
		break;
	}
}	//MciUpdateSound

/**
 * @brief	�Đ����ʂ�ݒ肷��
 *
 * @param	sound_id [����] �T�E���h �n���h��
 * @param	percent [����] ���� (0 �` 100)
 */
void MciSetVolume(int* sound_id, int percent)
{
	MciSoundInfo* sound_info;
	MCI_DGV_SETAUDIO_PARMS	audio_param;
	DWORD volume;

	if (!sound_id) {
		return;
	}
	sound_info = (MciSoundInfo*)sound_id;
	if (!sound_info->device_id) {
		return;
	}
	switch (sound_info->device_type) {
	case MCI_DEVTYPE_DIGITAL_VIDEO:
		ZeroMemory(&audio_param, sizeof(audio_param));
		audio_param.dwItem = MCI_DGV_SETAUDIO_VOLUME;
		audio_param.dwValue = percent * 10;
		mciSendCommand(sound_info->device_id, MCI_SETAUDIO, MCI_DGV_SETAUDIO_ITEM | MCI_DGV_SETAUDIO_VALUE, (DWORD_PTR)&audio_param);
		break;
	case MCI_DEVTYPE_SEQUENCER:
		volume = 0x0ffff * percent / 100;
		midiOutSetVolume(0, (DWORD)MAKELONG(volume, volume));
		break;
	case MCI_DEVTYPE_WAVEFORM_AUDIO:
		volume = 0x0ffff * percent / 100;
		waveOutSetVolume(0, (DWORD)MAKELONG(volume, volume));
		break;
	default:
		break;
	}
}	//MciSetVolume

#ifndef USED2D
//================================================================
// �Q�S�r�b�g�F�摜
//================================================================
/**
* @brief	�Q�Sbit�J���[�摜�p(�o�b�t�@��)�̏�����
*/
static void init_24bit_color_image(void)
{
	if (g_FrameBufferFull == NULL) {
		//24�r�b�g�l�̔z��Ƃ��Ċm��
		g_FrameBufferFull = (char*)calloc(g_ScreenBufferSize.X * g_ScreenBufferSize.Y, sizeof(char) * 3);
	}
	if (g_ScreenBufferFull == NULL) {
		// �P�s���̃T�C�Y�i�o�C�g���Z�o�j�y���z�P�s�̍Ō�͉��s�R�[�h'\n'���P���������Ă���̂ŁA�Ō�Ɂ{�P���Ă���B
		g_ScreenBufferLineStride = (g_ScreenBufferSize.X * PIXEL24ESCSIZE + 1);
		// �X�N���[���o�b�t�@(�t���J���[�p)�𐶐�
		g_ScreenBufferFull = (char*)calloc(g_ScreenBufferLineStride * g_ScreenBufferSize.Y, sizeof(char));
		_ASSERT(g_ScreenBufferFull);
		char* out_buf = g_ScreenBufferFull;
		// �t���J���[�p�ɃG�X�P�[�v�V�[�P���X�������ݒ�
		for (int y = 0; y < g_ScreenBufferSize.Y; y++) {
			for (int x = 0; x < g_ScreenBufferSize.X; x++) {
				//�F�R�[�h�q�f�a�l(�e�q�f�a�l��ASCII�����̐����R�P�^)��"000"�ɂ����A�P�s�N�Z�����G�X�P�[�v�V�[�P���X��������������ށB
				//sprintf_s(out_buf, PIXEL24ESCSIZE, pixel24bitEsc);	//"\x1b[48;2;000;000;000m "��20����
				memcpy(out_buf, pixel24bitEsc, PIXEL24ESCSIZE);
				out_buf += PIXEL24ESCSIZE;	//�P�s�N�Z�����|�C���^��i�߂�B
			}
			*out_buf = '\x0a';	//�P�s���̍Ō�ɉ��s�R�[�hLF'0A'����������
			out_buf++;	//�|�C���^�����̍s�̐擪�ɐi�߂�
		}
		out_buf--;	//�Ō��'\n'�������Ă���̂łP�����߂��B
		*out_buf = '\0';	//�Ō�̂P�������I�[����'\0'�ɏ���������B
	}
}	//init_24bit_color_image

/**
 * @brief	24�r�b�g/Pixel�摜�̏o��
 *
 * @param	buf [����] RGB�摜�f�[�^�z��̃|�C���^(��ʃT�C�Y�ȏ�̃o�b�t�@�K�v)
 *
 * @note
 *	RGB�摜�f�[�^�z��̓X�N���[���̉����~�c���̃o�C�g���ȏ�̔z��Ƃ��A
 *	�z��̒��g��RGB�e1�o�C�g(���v3�o�C�g)��1��f�Ƃ����f�[�^�ɂ���B
 *	�S�Ẳ�f�͘A�����Ă���K�v����B
 *	��)��80�����~�c25�s�̏ꍇ�A80x25=200�o�C�g�ȏ�̔z���n��
 */
void PrintImage(const char* _buf)
{
	if ((_buf == NULL) || (g_ScreenBufferFull == NULL)) {
		return;
	}
	DWORD write_num;
	const unsigned char* in_buf = (const unsigned char*)_buf;	//CharRGBconvTBL[][]��index�Ƃ��Ĉ����̂ŁA���������ɂ��Ă���B
	char* out_buf = g_ScreenBufferFull;
	for (int y = 0; y < g_ScreenBufferSize.Y; y++) {
		for (int x = 0; x < g_ScreenBufferSize.X; x++) {
			// R�ݒ�
			out_buf[7 + 0] = CharRGBconvTBL[0][*in_buf];	//0x30 + (*in_buf / 100);
			out_buf[7 + 1] = CharRGBconvTBL[1][*in_buf];	//0x30 + (*in_buf % 100 / 10);
			out_buf[7 + 2] = CharRGBconvTBL[2][*in_buf];	//0x30 + (*in_buf % 10);
			in_buf++;
			// G�ݒ�
			out_buf[7 + 4] = CharRGBconvTBL[0][*in_buf];	//0x30 + (*in_buf / 100);
			out_buf[7 + 5] = CharRGBconvTBL[1][*in_buf];	//0x30 + (*in_buf % 100 / 10);
			out_buf[7 + 6] = CharRGBconvTBL[2][*in_buf];	//0x30 + (*in_buf % 10);
			in_buf++;
			// B�ݒ�
			out_buf[7 + 8] = CharRGBconvTBL[0][*in_buf];	//0x30 + (*in_buf / 100); 
			out_buf[7 + 9] = CharRGBconvTBL[1][*in_buf];	//0x30 + (*in_buf % 100 / 10);
			out_buf[7 + 10] = CharRGBconvTBL[2][*in_buf];	//0x30 + (*in_buf % 10);
			in_buf++;
			out_buf += PIXEL24ESCSIZE;
		}
		out_buf++;
	}
	WriteConsoleA(g_DisplayHandle[g_SwapFlg], g_ScreenBufferFull, (g_ScreenBufferSize.X * PIXEL24ESCSIZE) * g_ScreenBufferSize.Y + (g_ScreenBufferSize.Y - 1), &write_num, NULL);
}	//PrintImage
#endif // !USED2D

//================================================================
//�@256�F�摜�y�H�����z�d�l���ς��\�����傫��
//================================================================
/**
* �Q�T�U�F�p���b�g�ƂQ�T�U�F�摜�ɂ��āy�H�����z
*
* �R���\�[���̂Q�T�U�F�Ή��F
* Windows�R���\�[���ɂ͕����F�E�w�i�F�ɒʏ�P�U�F�ł���B
* ����(2022)��Windows�R���\�[����ASNI�G�X�P�[�v�V�[�P���X�ɑΉ����Ă���̂ŁA�Q�T�U�F�̃p���b�g��ݒ肷�邱�Ƃ��o����B
* �A���AWin32�̃R���\�[��API�ɂ͂Q�T�U�F�@�\�͂Ȃ��̂ŁA�F�w��ɂ̓G�X�P�[�v�V�[�P���X���g���K�v������B
*
* �p���b�g�F
* �ŏ��̂P�U�F���ʏ�̂P�U�F�ɑΉ�����B
* �P�V�F�`�Q�T�U�F���ǉ��̐F�ɂȂ�B
*/

/**
* @brief	ANSI color 256.\n
* COLORREF : #00BBGGRR
*/
const COLORREF ANSI_PAL256_COLOR[NUM_ANSI_PAL] = {
0x000000,0x800000,0x008000,0x808000,0x000080,0x800080,0x008080,0xC0C0C0,0x808080,0xFF0000,0x00FF00,0xFFFF00,0x0000FF,0xFF00FF,0x00FFFF,0xFFFFFF,
0x000000,0x00005F,0x000087,0x0000AF,0x0000D7,0x0000FF,0x005F00,0x005F5F,0x005F87,0x005FAF,0x005FD7,0x005FFF,0x008700,0x00875F,0x008787,0x0087AF,
0x0087D7,0x0087FF,0x00AF00,0x00AF5F,0x00AF87,0x00AFAF,0x00AFD7,0x00AFFF,0x00D700,0x00D75F,0x00D787,0x00D7AF,0x00D7D7,0x00D7FF,0x00FF00,0x00FF5F,
0x00FF87,0x00FFAF,0x00FFD7,0x00FFFF,0x5F0000,0x5F005F,0x5F0087,0x5F00AF,0x5F00D7,0x5F00FF,0x5F5F00,0x5F5F5F,0x5F5F87,0x5F5FAF,0x5F5FD7,0x5F5FFF,
0x5F8700,0x5F875F,0x5F8787,0x5F87AF,0x5F87D7,0x5F87FF,0x5FAF00,0x5FAF5F,0x5FAF87,0x5FAFAF,0x5FAFD7,0x5FAFFF,0x5FD700,0x5FD75F,0x5FD787,0x5FD7AF,
0x5FD7D7,0x5FD7FF,0x5FFF00,0x5FFF5F,0x5FFF87,0x5FFFAF,0x5FFFD7,0x5FFFFF,0x870000,0x87005F,0x870087,0x8700AF,0x8700D7,0x8700FF,0x875F00,0x875F5F,
0x875F87,0x875FAF,0x875FD7,0x875FFF,0x878700,0x87875F,0x878787,0x8787AF,0x8787D7,0x8787FF,0x87AF00,0x87AF5F,0x87AF87,0x87AFAF,0x87AFD7,0x87AFFF,
0x87D700,0x87D75F,0x87D787,0x87D7AF,0x87D7D7,0x87D7FF,0x87FF00,0x87FF5F,0x87FF87,0x87FFAF,0x87FFD7,0x87FFFF,0xAF0000,0xAF005F,0xAF0087,0xAF00AF,
0xAF00D7,0xAF00FF,0xAF5F00,0xAF5F5F,0xAF5F87,0xAF5FAF,0xAF5FD7,0xAF5FFF,0xAF8700,0xAF875F,0xAF8787,0xAF87AF,0xAF87D7,0xAF87FF,0xAFAF00,0xAFAF5F,
0xAFAF87,0xAFAFAF,0xAFAFD7,0xAFAFFF,0xAFD700,0xAFD75F,0xAFD787,0xAFD7AF,0xAFD7D7,0xAFD7FF,0xAFFF00,0xAFFF5F,0xAFFF87,0xAFFFAF,0xAFFFD7,0xAFFFFF,
0xD70000,0xD7005F,0xD70087,0xD700AF,0xD700D7,0xD700FF,0xD75F00,0xD75F5F,0xD75F87,0xD75FAF,0xD75FD7,0xD75FFF,0xD78700,0xD7875F,0xD78787,0xD787AF,
0xD787D7,0xD787FF,0xD7AF00,0xD7AF5F,0xD7AF87,0xD7AFAF,0xD7AFD7,0xD7AFFF,0xD7D700,0xD7D75F,0xD7D787,0xD7D7AF,0xD7D7D7,0xD7D7FF,0xD7FF00,0xD7FF5F,
0xD7FF87,0xD7FFAF,0xD7FFD7,0xD7FFFF,0xFF0000,0xFF005F,0xFF0087,0xFF00AF,0xFF00D7,0xFF00FF,0xFF5F00,0xFF5F5F,0xFF5F87,0xFF5FAF,0xFF5FD7,0xFF5FFF,
0xFF8700,0xFF875F,0xFF8787,0xFF87AF,0xFF87D7,0xFF87FF,0xFFAF00,0xFFAF5F,0xFFAF87,0xFFAFAF,0xFFAFD7,0xFFAFFF,0xFFD700,0xFFD75F,0xFFD787,0xFFD7AF,
0xFFD7D7,0xFFD7FF,0xFFFF00,0xFFFF5F,0xFFFF87,0xFFFFAF,0xFFFFD7,0xFFFFFF,0x080808,0x121212,0x1C1C1C,0x262626,0x303030,0x3A3A3A,0x444444,0x4E4E4E,
0x585858,0x626262,0x6C6C6C,0x767676,0x808080,0x8A8A8A,0x949494,0x9E9E9E,0xA8A8A8,0xB2B2B2,0xBCBCBC,0xC6C6C6,0xD0D0D0,0xDADADA,0xE4E4E4,0xEEEEEE,
};	//ANSI_PAL256_COLOR

/**
* @brief	ANSI color 256.\n
* RGBQUAD : #00RRGGBB{bb,gg,rr,00}
*/
const RGBQUAD ANSI_PAL256_RGB[NUM_ANSI_PAL] = {
{0x00,0x00,0x00,0},{0x80,0x00,0x00,0},{0x00,0x80,0x00,0},{0x80,0x80,0x00,0},{0x00,0x00,0x80,0},{0x80,0x00,0x80,0},{0x00,0x80,0x80,0},{0xC0,0xC0,0xC0,0},{0x80,0x80,0x80,0},{0xFF,0x00,0x00,0},{0x00,0xFF,0x00,0},{0xFF,0xFF,0x00,0},{0x00,0x00,0xFF,0},{0xFF,0x00,0xFF,0},{0x00,0xFF,0xFF,0},{0xFF,0xFF,0xFF,0},
{0x00,0x00,0x00,0},{0x00,0x00,0x5F,0},{0x00,0x00,0x87,0},{0x00,0x00,0xAF,0},{0x00,0x00,0xD7,0},{0x00,0x00,0xFF,0},{0x00,0x5F,0x00,0},{0x00,0x5F,0x5F,0},{0x00,0x5F,0x87,0},{0x00,0x5F,0xAF,0},{0x00,0x5F,0xD7,0},{0x00,0x5F,0xFF,0},{0x00,0x87,0x00,0},{0x00,0x87,0x5F,0},{0x00,0x87,0x87,0},{0x00,0x87,0xAF,0},
{0x00,0x87,0xD7,0},{0x00,0x87,0xFF,0},{0x00,0xAF,0x00,0},{0x00,0xAF,0x5F,0},{0x00,0xAF,0x87,0},{0x00,0xAF,0xAF,0},{0x00,0xAF,0xD7,0},{0x00,0xAF,0xFF,0},{0x00,0xD7,0x00,0},{0x00,0xD7,0x5F,0},{0x00,0xD7,0x87,0},{0x00,0xD7,0xAF,0},{0x00,0xD7,0xD7,0},{0x00,0xD7,0xFF,0},{0x00,0xFF,0x00,0},{0x00,0xFF,0x5F,0},
{0x00,0xFF,0x87,0},{0x00,0xFF,0xAF,0},{0x00,0xFF,0xD7,0},{0x00,0xFF,0xFF,0},{0x5F,0x00,0x00,0},{0x5F,0x00,0x5F,0},{0x5F,0x00,0x87,0},{0x5F,0x00,0xAF,0},{0x5F,0x00,0xD7,0},{0x5F,0x00,0xFF,0},{0x5F,0x5F,0x00,0},{0x5F,0x5F,0x5F,0},{0x5F,0x5F,0x87,0},{0x5F,0x5F,0xAF,0},{0x5F,0x5F,0xD7,0},{0x5F,0x5F,0xFF,0},
{0x5F,0x87,0x00,0},{0x5F,0x87,0x5F,0},{0x5F,0x87,0x87,0},{0x5F,0x87,0xAF,0},{0x5F,0x87,0xD7,0},{0x5F,0x87,0xFF,0},{0x5F,0xAF,0x00,0},{0x5F,0xAF,0x5F,0},{0x5F,0xAF,0x87,0},{0x5F,0xAF,0xAF,0},{0x5F,0xAF,0xD7,0},{0x5F,0xAF,0xFF,0},{0x5F,0xD7,0x00,0},{0x5F,0xD7,0x5F,0},{0x5F,0xD7,0x87,0},{0x5F,0xD7,0xAF,0},
{0x5F,0xD7,0xD7,0},{0x5F,0xD7,0xFF,0},{0x5F,0xFF,0x00,0},{0x5F,0xFF,0x5F,0},{0x5F,0xFF,0x87,0},{0x5F,0xFF,0xAF,0},{0x5F,0xFF,0xD7,0},{0x5F,0xFF,0xFF,0},{0x87,0x00,0x00,0},{0x87,0x00,0x5F,0},{0x87,0x00,0x87,0},{0x87,0x00,0xAF,0},{0x87,0x00,0xD7,0},{0x87,0x00,0xFF,0},{0x87,0x5F,0x00,0},{0x87,0x5F,0x5F,0},
{0x87,0x5F,0x87,0},{0x87,0x5F,0xAF,0},{0x87,0x5F,0xD7,0},{0x87,0x5F,0xFF,0},{0x87,0x87,0x00,0},{0x87,0x87,0x5F,0},{0x87,0x87,0x87,0},{0x87,0x87,0xAF,0},{0x87,0x87,0xD7,0},{0x87,0x87,0xFF,0},{0x87,0xAF,0x00,0},{0x87,0xAF,0x5F,0},{0x87,0xAF,0x87,0},{0x87,0xAF,0xAF,0},{0x87,0xAF,0xD7,0},{0x87,0xAF,0xFF,0},
{0x87,0xD7,0x00,0},{0x87,0xD7,0x5F,0},{0x87,0xD7,0x87,0},{0x87,0xD7,0xAF,0},{0x87,0xD7,0xD7,0},{0x87,0xD7,0xFF,0},{0x87,0xFF,0x00,0},{0x87,0xFF,0x5F,0},{0x87,0xFF,0x87,0},{0x87,0xFF,0xAF,0},{0x87,0xFF,0xD7,0},{0x87,0xFF,0xFF,0},{0xAF,0x00,0x00,0},{0xAF,0x00,0x5F,0},{0xAF,0x00,0x87,0},{0xAF,0x00,0xAF,0},
{0xAF,0x00,0xD7,0},{0xAF,0x00,0xFF,0},{0xAF,0x5F,0x00,0},{0xAF,0x5F,0x5F,0},{0xAF,0x5F,0x87,0},{0xAF,0x5F,0xAF,0},{0xAF,0x5F,0xD7,0},{0xAF,0x5F,0xFF,0},{0xAF,0x87,0x00,0},{0xAF,0x87,0x5F,0},{0xAF,0x87,0x87,0},{0xAF,0x87,0xAF,0},{0xAF,0x87,0xD7,0},{0xAF,0x87,0xFF,0},{0xAF,0xAF,0x00,0},{0xAF,0xAF,0x5F,0},
{0xAF,0xAF,0x87,0},{0xAF,0xAF,0xAF,0},{0xAF,0xAF,0xD7,0},{0xAF,0xAF,0xFF,0},{0xAF,0xD7,0x00,0},{0xAF,0xD7,0x5F,0},{0xAF,0xD7,0x87,0},{0xAF,0xD7,0xAF,0},{0xAF,0xD7,0xD7,0},{0xAF,0xD7,0xFF,0},{0xAF,0xFF,0x00,0},{0xAF,0xFF,0x5F,0},{0xAF,0xFF,0x87,0},{0xAF,0xFF,0xAF,0},{0xAF,0xFF,0xD7,0},{0xAF,0xFF,0xFF,0},
{0xD7,0x00,0x00,0},{0xD7,0x00,0x5F,0},{0xD7,0x00,0x87,0},{0xD7,0x00,0xAF,0},{0xD7,0x00,0xD7,0},{0xD7,0x00,0xFF,0},{0xD7,0x5F,0x00,0},{0xD7,0x5F,0x5F,0},{0xD7,0x5F,0x87,0},{0xD7,0x5F,0xAF,0},{0xD7,0x5F,0xD7,0},{0xD7,0x5F,0xFF,0},{0xD7,0x87,0x00,0},{0xD7,0x87,0x5F,0},{0xD7,0x87,0x87,0},{0xD7,0x87,0xAF,0},
{0xD7,0x87,0xD7,0},{0xD7,0x87,0xFF,0},{0xD7,0xAF,0x00,0},{0xD7,0xAF,0x5F,0},{0xD7,0xAF,0x87,0},{0xD7,0xAF,0xAF,0},{0xD7,0xAF,0xD7,0},{0xD7,0xAF,0xFF,0},{0xD7,0xD7,0x00,0},{0xD7,0xD7,0x5F,0},{0xD7,0xD7,0x87,0},{0xD7,0xD7,0xAF,0},{0xD7,0xD7,0xD7,0},{0xD7,0xD7,0xFF,0},{0xD7,0xFF,0x00,0},{0xD7,0xFF,0x5F,0},
{0xD7,0xFF,0x87,0},{0xD7,0xFF,0xAF,0},{0xD7,0xFF,0xD7,0},{0xD7,0xFF,0xFF,0},{0xFF,0x00,0x00,0},{0xFF,0x00,0x5F,0},{0xFF,0x00,0x87,0},{0xFF,0x00,0xAF,0},{0xFF,0x00,0xD7,0},{0xFF,0x00,0xFF,0},{0xFF,0x5F,0x00,0},{0xFF,0x5F,0x5F,0},{0xFF,0x5F,0x87,0},{0xFF,0x5F,0xAF,0},{0xFF,0x5F,0xD7,0},{0xFF,0x5F,0xFF,0},
{0xFF,0x87,0x00,0},{0xFF,0x87,0x5F,0},{0xFF,0x87,0x87,0},{0xFF,0x87,0xAF,0},{0xFF,0x87,0xD7,0},{0xFF,0x87,0xFF,0},{0xFF,0xAF,0x00,0},{0xFF,0xAF,0x5F,0},{0xFF,0xAF,0x87,0},{0xFF,0xAF,0xAF,0},{0xFF,0xAF,0xD7,0},{0xFF,0xAF,0xFF,0},{0xFF,0xD7,0x00,0},{0xFF,0xD7,0x5F,0},{0xFF,0xD7,0x87,0},{0xFF,0xD7,0xAF,0},
{0xFF,0xD7,0xD7,0},{0xFF,0xD7,0xFF,0},{0xFF,0xFF,0x00,0},{0xFF,0xFF,0x5F,0},{0xFF,0xFF,0x87,0},{0xFF,0xFF,0xAF,0},{0xFF,0xFF,0xD7,0},{0xFF,0xFF,0xFF,0},{0x08,0x08,0x08,0},{0x12,0x12,0x12,0},{0x1C,0x1C,0x1C,0},{0x26,0x26,0x26,0},{0x30,0x30,0x30,0},{0x3A,0x3A,0x3A,0},{0x44,0x44,0x44,0},{0x4E,0x4E,0x4E,0},
{0x58,0x58,0x58,0},{0x62,0x62,0x62,0},{0x6C,0x6C,0x6C,0},{0x76,0x76,0x76,0},{0x80,0x80,0x80,0},{0x8A,0x8A,0x8A,0},{0x94,0x94,0x94,0},{0x9E,0x9E,0x9E,0},{0xA8,0xA8,0xA8,0},{0xB2,0xB2,0xB2,0},{0xBC,0xBC,0xBC,0},{0xC6,0xC6,0xC6,0},{0xD0,0xD0,0xD0,0},{0xDA,0xDA,0xDA,0},{0xE4,0xE4,0xE4,0},{0xEE,0xEE,0xEE,0},
};

#ifndef USED2D
/**
* @brief	256�F�o�b�t�@�֘A�̏�����
*
* @note	�y�H�����z
*/
static void init_256color_image(void)
{
	//256�F�s�N�Z���o�b�t�@�̊m��
	int screen_area_pixel_size = g_ScreenBufferSize.X * g_ScreenBufferSize.Y;	//�ʐ�
	if (g_FrameBuffer256 == NULL) {
		g_FrameBuffer256 = (BYTE*)calloc(screen_area_pixel_size, sizeof(BYTE));
		_ASSERT(g_FrameBuffer256);
	}
	if (g_ScreenBuffer256 == NULL) {
		g_ScreenBuffer256 = (BYTE*)calloc(screen_area_pixel_size, sizeof(pixel256Esc));	//�s�����s�K�v�H�s�v�H
		_ASSERT(g_ScreenBuffer256);
		int idx = 0;
		for (int n = 0; n < screen_area_pixel_size; n++) {
			memcpy(&g_ScreenBuffer256[idx], pixel256Esc, sizeof(pixel256Esc));
			idx += sizeof(pixel256Esc);
		}
	}
}	//init_256color_image

/**
* @brief	���݂̃V�X�e���p���b�g�ɂQ�T�U�F�ݒ�
*
* @param	HANDLE _hCon	�R���\�[���̃n���h��
* @param	const COLORREF* _p256
* @param	int _num_pal
*
* @note	�y�H�����z
*/
static void set_palette256(HANDLE _hCon, const COLORREF* _p256, int _num_pal)
{
	if (_hCon == 0) {
		_hCon = g_DisplayHandle[g_SwapFlg];
	}
	if (_num_pal > 256) {
		_num_pal = 256;
	}
	DWORD wrn;
	const char palsetEsc[] = "\x1b]4;%03d;rgb:%02x/%02x/%02x\x07";
	char str[sizeof(palsetEsc) + 1] = { 0 };
	for (int pn = 0; pn < _num_pal; pn++) {
		//"ESC]4;<pal#>;rgb:RR/GG/BBBEL" RRGGBB:hex
		//sprintf_s(str, 23, "\x1b]4;%03d;rgb:%02x/%02x/%02x\x1b\\", pn, _p256[pn].rgbRed, _p256[pn].rgbGreen, _p256[pn].rgbBlue);
		sprintf_s(str, sizeof(str), palsetEsc,
			pn,
			((DWORD)_p256[pn]) & 0x0000FF, //B
			(((DWORD)_p256[pn]) & 0x00FF00) >> 8,	//G
			(((DWORD)_p256[pn]) & 0xFF0000) >> 16	//R
		);
		WriteConsoleA(_hCon, str, sizeof(palette256Esc), &wrn, NULL);
	}
}	//set_palette256

/**
* @brief	���݂̃R���\�[���̐F��ANSI-256�F�p���b�g�ɏ�����
*
* @note	�y�H�����z
*/
void InitPaletteANSI256(void)
{
	if (g_DisplayHandle[0] != NULL) {
		set_palette256(g_DisplayHandle[0], ANSI_PAL256_COLOR, NUM_ANSI_PAL);
	}
	if (g_DisplayHandle[1] != NULL) {
		set_palette256(g_DisplayHandle[1], ANSI_PAL256_COLOR, NUM_ANSI_PAL);
	}
	set_palette256(GetStdHandle(STD_OUTPUT_HANDLE), ANSI_PAL256_COLOR, NUM_ANSI_PAL);	//���݂̃R���\�[���n���h�������������Ă���
}	//InitPaletteANSI256

/**
* @brief	�w��ʒu�ւ̂P�s�N�Z���i256�F�j�`��
*
* @param	HANDLE _hCon	�R���\�[���̃n���h��
* @param	int _x,_y�F���W
* @param	int _palidx�F�Q�T�U�p���b�g�ԍ�
*
* @note		8�r�b�g/�s�N�Z���i256�F�j���w��̍��W�ւP�s�N�Z���`�悷��B\n
*			�F
*
* @note	�y�H�����z
*/
void SetPixel256(int _x, int _y, int _palidx)
{
	DWORD wrn;
	char str[23] = {};
	//�����w��ŃG�X�P�[�v�V�[�P���X������̎w��l�Ƀp���b�g�ԍ�����������
	sprintf_s(str, 23, pixel256Esc, _palidx & 0xFF);
	//�����ʒu�w��
	SetConsoleCursorPosition(g_DisplayHandle[g_SwapFlg], COORD{ (SHORT)_x,(SHORT)_y });
	//"\x1b[48;5;000m "
	WriteConsoleA(g_DisplayHandle[g_SwapFlg], str, sizeof(pixel256Esc), &wrn, NULL);
}	//SetPixel256

/**
*
* @note	�y�H�����z
*/
void SetPixelBuffer256(int _x, int _y, int _palidx)
{
	if (_x >= 0 && _x < g_ScreenBufferSize.X && _y >= 0 && _y < g_ScreenBufferSize.Y) {
		g_FrameBuffer256[_y * g_ScreenBufferSize.X + _x] = _palidx;
	}
}	//SetPixel256

/**
*
* @note	�y�H�����z
*/
void DrawPixelBuffer256(int _xp, int _yp, int _w, int _h, BYTE* _buf)
{
	for (int y = 0; y < _h; y++) {
		for (int x = 0; x < _w; x++) {
			int xx = x + _xp;
			int yy = y + _yp;
			if (xx >= 0 && xx < g_ScreenBufferSize.X && yy >= 0 && yy < g_ScreenBufferSize.Y) {
				g_FrameBuffer256[yy * g_ScreenBufferSize.X + xx] = *_buf;
			}
			_buf++;
		}
	}
	//
	int screen_area_pixel_size = g_ScreenBufferSize.X * g_ScreenBufferSize.Y;	//�ʐ�
	BYTE* dp = &g_ScreenBuffer256[0];
	BYTE* sp = &g_FrameBuffer256[0];
	for (int i = 0; i < screen_area_pixel_size; i++) {
		dp[7] = CharRGBconvTBL[0][*sp];	//'0' + (g_FrameBuffer256[i] / 100 % 10);
		dp[8] = CharRGBconvTBL[1][*sp];	//'0' + (g_FrameBuffer256[i] / 10 % 10);
		dp[9] = CharRGBconvTBL[2][*sp];	//'0' + (g_FrameBuffer256[i] % 10);
		dp += sizeof(pixel256Esc);
		sp++;
	}
	DWORD wrn;
	WriteConsoleA(g_DisplayHandle[g_SwapFlg], g_ScreenBuffer256, screen_area_pixel_size * sizeof(pixel256Esc), &wrn, NULL);
}	//SetPixel256
#endif // !USED2D

#ifdef CONIOEX_DDA_SHAPE
//================================================================
//	�}�`�̕`��iDDA�ŕ`��j
//================================================================
template<typename T>
inline void swap(T& _a, T& _b) { T tmp = _a; _a = _b; _b = tmp; }
//=== ������ ===
/*
* @brief	��������`�悷��
* @param	int _x1,_y1	�J�n���W
* @param	int _x2		�I���w���W�i�x���W��_y1�Ɠ����j
* @param	RGBQUAD _rgb	�`��F��RGB�l
*/
void DrawLineH(int _x1, int _y1, int _x2, RGBQUAD _rgb)
{
	if (_x2 < _x1) {
		//left<Right�ɂ���
		swap(_x1, _x2);
	}
	for (; _x1 <= _x2; _x1++) {
		DrawPixel(_x1, _y1, _rgb);
	}
}
/*
* @brief	��������`�悷��
* @param	int _x1,_y1	�J�n���W
* @param	int _x2		�I���w���W�i�x���W��_y1�Ɠ����j
* @param	int _cc		�J���[�R�[�h�i�p���b�g�̔ԍ��j
*/
void DrawLineH(int _x1, int _y1, int _x2, int _cc)
{
	DrawLineH(_x1, _y1, _x2, g_PaletteD2D[_cc % NUM_D2D_PAL]);
}	//DrawLineH
//=== ������ ===
/*
* @brief	��������`�悷��
* @param	int _x1,_y1	�J�n���W
* @param	int _y2		�I���x���W�i�w���W��_x1�Ɠ����j
* @param	RGBQUAD _rgb	�`��F��RGB�l
*/
void DrawLineV(int _x1, int _y1, int _y2, RGBQUAD _rgb)
{
	if (_y2 < _y1) {
		//Top<Bottom�ɂ���
		swap(_y1, _y2);
	}
	for (; _y1 <= _y2; _y1++) {
		DrawPixel(_x1, _y1, _rgb);
	}
}	//DrawLineV
/*
* @brief	��������`�悷��
* @param	int _x1,_y1	�J�n���W
* @param	int _y2		�I���x���W�i�w���W��_x1�Ɠ����j
* @param	int _cc		�J���[�R�[�h�i�p���b�g�̔ԍ��j
*/
void DrawLineV(int _x1, int _y1, int _y2, int _cc)
{
	DrawLineV(_x1, _y1, _y2, g_PaletteD2D[_cc % NUM_D2D_PAL]);
}	//DrawLineV
//=== 45�x�X�ΐ� ===
/*
* @brief	45�x�̒����̕`��
* @param	int _x1,_y2	�J�n���W
* @param	int _len	����
* @param	int _dir	����
* @param	RGBQUAD _rgb	�`��F��RGB�l
*/
void DrawLine45(int _x1, int _y1, int _len, int _dir, RGBQUAD _rgb)
{
	//45�x
	switch (_dir) {
	case 0:	//�E��45�x�_
		_len = _x1 + _len;
		for (; _x1 < _len; _x1++, _y1++) {
			DrawPixel(_x1, _y1, _rgb);
		}
		break;
	case 1:	//����45�x�^
		_len = _y1 + _len;
		for (; _y1 < _len; _x1--, _y1++) {
			DrawPixel(_x1, _y1, _rgb);
		}
		break;
	case 2:	//�E��45�x�^
		_len = _x1 + _len;
		for (; _x1 < _len; _x1++, _y1--) {
			DrawPixel(_x1, _y1, _rgb);
		}
		break;
	case 3:	//����45�x�_
		_len = _x1 - _len;
		for (; _x1 > _len; _x1--, _y1--) {
			DrawPixel(_x1, _y1, _rgb);
		}
		break;
	}
}
/*
* @brief	45�x�̒����̕`��
* @param	int _x1,_y2	�J�n���W
* @param	int _len	����
* @param	int _dir	����
* @param	int _cc		�J���[�R�[�h�i�p���b�g�̔ԍ��j
*/
void DrawLine45(int _x1, int _y1, int _len, int _dir, int _cc)
{
	DrawLine45(_x1, _y1, _len, _dir, g_PaletteD2D[_cc % NUM_D2D_PAL]);
}
//=== ����(����) ===
/*
* @brief	�����̕`��
* @param	int _x1,_y2	�J�n���W
* @param	int _x2,_y2	�I�����W
* @param	RGBQUAD _rgb	�`��F��RGB�l
*/
void DrawLine(int _x1, int _y1, int _x2, int _y2, RGBQUAD _rgb)
{
	if (_x1 == _x2 && _y1 == _y2) {
		DrawPixel(_x1, _y1, _rgb);	//�_
		return;
	}
	else if (_y1 == _y2) {
		DrawLineH(_x1, _y1, _x2, _rgb);	//������
		return;
	}
	else if (_x1 == _x2) {
		DrawLineV(_x1, _y1, _y2, _rgb);	//������
		return;
	}
	//DDA-line
	int dx = abs(_x2 - _x1);	//��
	int dy = abs(_y2 - _y1);	//��
	int err = dx - dy;	//���ƍ��̍���(+)�Ȃ牡��(-)�Ȃ�c��
	if (err == 0) {
		int area = 0;
		if (_x2 < _x1)area |= 1;
		if (_y2 < _y1)area |= 2;
		DrawLine45(_x1, _y1, dx, area, _rgb);	//45�x
		return;
	}
	int sx = (_x1 < _x2) ? (1) : (-1);	//X�����̕���
	int sy = (_y1 < _y2) ? (1) : (-1);	//Y�����̕���
	do {
		DrawPixel(_x1, _y1, _rgb);
		int e2 = (err << 1);	//
		if (e2 > -dy) {
			err -= dy;
			_x1 += sx;
		}
		if (e2 < dx) {
			err += dx;
			_y1 += sy;
		}
	} while ((_x1 != _x2) || (_y1 != _y2));
	return;
}	//DrawLine
/*
* @brief	�����̕`��
* @param	int _x1,_y2	�J�n���W
* @param	int _x2,_y2	�I�����W
* @param	int _cc		�J���[�R�[�h�i�p���b�g�̔ԍ��j
*/
void DrawLine(int _x1, int _y1, int _x2, int _y2, int _cc)
{
	DrawLine(_x1, _y1, _x2, _y2, g_PaletteD2D[_cc % NUM_D2D_PAL]);
}	//DrawLine
//=== ��` ===
/*
* @brief	��`��`�悷��
* @param	int _x1,_y1	������W
* @param	int _x2,_y2	�E�����W
* @param	RGBQUAD _rgb	�`��F��RGB�l
*/
void DrawRect(int _x1, int _y1, int _x2, int _y2, RGBQUAD _rgb, bool _fill)
{
	if (_fill) {
		if (_y2 < _y1)swap(_y1, _y2);
		for (; _y1 <= _y2; _y1++) {
			DrawLineH(_x1, _y1, _x2, _rgb);	//Top-line
		}
	}
	else {
		DrawLineH(_x1, _y1, _x2, _rgb);	//Top-line
		DrawLineH(_x1, _y2, _x2, _rgb);	//Bottom-line
		DrawLineV(_x1, _y1, _y2, _rgb);	//Left-line
		DrawLineV(_x2, _y1, _y2, _rgb);	//Right-line
	}
}	//DrawRect
/*
* @brief	��`��`�悷��
* @param	int _x1,_y1	������W
* @param	int _x2,_y2	�E�����W
* @param	int _cc		�J���[�R�[�h�i�p���b�g�̔ԍ��j
*/
void DrawRect(int _x1, int _y1, int _x2, int _y2, int _cc, bool _fill) {
	DrawRect(_x1, _y1, _x2, _y2, g_PaletteD2D[_cc % NUM_D2D_PAL], _fill);
}
//=== �~�` ===
/*
* @brief	�~��`�悷��
* @param	int _cx,_cy	���S���W
* @param	int _r		���a
* @param	RGBQUAD _rgb	�`��F��RGB�l
*/
void DrawCircle(int _cx, int _cy, int _r, RGBQUAD _rgb, bool _fill)
{
	int D = _r;
	int x = (D - 1);
	int y = 0;
	if (_fill) {
		while (x >= y) {
			//�E���Ő������O�x�F�P��łW�h�b�g�i�W�ی����j��`��
			DrawLineH(_cx, _cy + y, _cx + x,/* _cy + y,*/ _rgb);	//��P�ی��F�@�@�O���`
			DrawLineH(_cx, _cy + x, _cx + y,/* _cy + x,*/ _rgb);	//��Q�ی��F�@�S�T���`
			DrawLineH(_cx, _cy + x, _cx - y,/* _cy + x,*/ _rgb);	//��R�ی��F�@�X�O���`
			DrawLineH(_cx, _cy + y, _cx - x,/* _cy + y,*/ _rgb);	//��R�ی��F�P�R�T���`
			DrawLineH(_cx, _cy - y, _cx - x,/* _cy - y,*/ _rgb);	//��R�ی��F�P�W�O���`
			DrawLineH(_cx, _cy - x, _cx - y,/* _cy - x,*/ _rgb);	//��R�ی��F�Q�Q�T���`
			DrawLineH(_cx, _cy - x, _cx + y,/* _cy - x,*/ _rgb);	//��R�ی��F�Q�V�O���`
			DrawLineH(_cx, _cy - y, _cx + x,/* _cy - y,*/ _rgb);	//��R�ی��F�R�P�T���`�R�U�O��
			D -= (y << 1);
			if (D <= 0) {
				x--;
				D += (x << 1);
			}
			y++;
		}
	}
	else {
		while (x >= y) {
			//�E���Ő������O�x�F�P��łW�h�b�g�i�W�ی����j��`��
			DrawPixel(_cx + x, _cy + y, _rgb);	//��P�ی��F�@�@�O���`
			DrawPixel(_cx + y, _cy + x, _rgb);	//��Q�ی��F�@�S�T���`
			DrawPixel(_cx - y, _cy + x, _rgb);	//��R�ی��F�@�X�O���`
			DrawPixel(_cx - x, _cy + y, _rgb);	//��R�ی��F�P�R�T���`
			DrawPixel(_cx - x, _cy - y, _rgb);	//��R�ی��F�P�W�O���`
			DrawPixel(_cx - y, _cy - x, _rgb);	//��R�ی��F�Q�Q�T���`
			DrawPixel(_cx + y, _cy - x, _rgb);	//��R�ی��F�Q�V�O���`
			DrawPixel(_cx + x, _cy - y, _rgb);	//��R�ی��F�R�P�T���`�R�U�O��
			D -= (y << 1);
			if (D <= 0) {
				x--;
				D += (x << 1);
			}
			y++;
		}
	}
}	//DDA_Circle
/*
* @brief	�~��`�悷��
* @param	int _cx,_cy	���S���W
* @param	int _r		���a
* @param	int _cc		�J���[�R�[�h�i�p���b�g�̔ԍ��j
*/
void DrawCircle(int _cx, int _cy, int _r, int _cc, bool _fill) {
	DrawCircle(_cx, _cy, _r, g_PaletteD2D[_cc % NUM_D2D_PAL], _fill);
}

#endif // !CONIOEX_DDA_SHAPE

//================================================================
//	�t���[���������v���p�֐�
#define	USE_NONE	0
#define	USE_MMSEC	1	//mm(�~��)�b���x"timeGetTime()"���g��
#define	USE_QPC		2	//��(�}�C�N��)�b���x"QueryPerformanceCounter"���g��
#define	USE_RDTSC	3	//"ReaD Time Stamp Counter"���g���iCPU�̃N���b�N�J�E���^�j
#define	FRAME_SYNC	USE_RDTSC	//USE_NONE

#if (FRAME_SYNC==USE_RDTSC)
#include	"intrin.h"
#endif // USE_RDTSC

//�t���[���X�s�[�h�v���p
#ifdef _DEBUG
static char dbg_str[4096] = {};	//�f�o�b�O�p������
static int dbg_frame_count = 0;	//�t���[����
#endif // _DEBUG
static double FPS = 60.0;

#if (FRAME_SYNC==USE_RDTSC)
// ���ڍ׌v���p
static __int64	i64_frequency = 0;
static __int64	i64_t1 = 0;
static __int64	i64_t2 = 0;
static double f_1sec = 0.0;		//�P�b�̃J�E���g��
static double f_tpf = 0.0;		//�P�t���[���̃J�E���g���i����\�j
#ifdef _DEBUG
static double f_total = 0.0;
#endif // _DEBUG
/*
* �t���[������
* ���𑜓x�^�C�� �X�^���v���g���Čv��
*/
//������
void InitFrameSync(double _FPS)
{
	if (_FPS <= 0) {
		//�͈͊O�͂PFPS�Ƃ���
		_FPS = 1;
	}
	FPS = _FPS;
	timeBeginPeriod(1);	//�v�����x��1�~���b�ɐݒ�
	i64_t1 = __rdtsc();
	Sleep(1000 / 10);	//�v����Ƃ��Ĉ�莞�ԑ҂i1/10�b�j
	i64_t2 = __rdtsc();
	//1/10�b�Ԃ�t1�`t2�Ԃ̃J�E���g����P�b�Ԃ̃J�E���g���Z�o
	i64_frequency = (i64_t2 - i64_t1) * (__int64)10;	//1513233427/1681604920/1813127620
	f_1sec = (double)i64_frequency;	//�P�b�Ԃ̃J�E���g���i����\�j
	f_tpf = (f_1sec / FPS);	//�P�t���[���̃J�E���g���i����\�j
}
//����
void FrameSync(void)
{
	//�t���[���҂����Ԍv��
	i64_t2 = __rdtsc();	//���ݎ��Ԏ擾
	double f_frame_interval = (double)(i64_t2 - i64_t1);	//�t���[���Ԋu(����)�Z�o
	double f_wait = (f_tpf - f_frame_interval);	//�҂����Ԃ��Z�o�i�҂����ԁ��P�t���[���ɕK�v�Ȏ��ԁ|���ۂɊ|���������ԁj
	if (f_wait > 0) {
		__int64 t2 = 0;
		do {
			//Sleep(1);
			t2 = __rdtsc();	//���ݎ��Ԏ擾
			//���̃��[�v�̌o�ߎ��Ԃ𑪂�A�҂����Ԉȉ��Ȃ烋�[�v�p������B
		} while ((double)(t2 - i64_t2) < f_wait);
#ifdef _DEBUG
		//�P�t���[�����Ԃ�ώZ�i�P�t���[�����ԁ��O�̃t���[�����獡�̃t���[���܂ł̌o�ߎ��ԁ{����Ȃ��������̑҂����ԁj
		f_total += (f_frame_interval + f_wait);
	}
	else {
		//�҂����Ԃ����������̂łP�t���[�����Ԃ�����ώZ
		f_total += f_frame_interval;
	}
	dbg_frame_count++;	//�P�b�Ԃ̃t���[�����J�E���g(�f�o�b�O�p)
	if ((f_total >= f_1sec) && (dbg_frame_count > 0)) {
		double fps = f_1sec / (f_total / (double)dbg_frame_count);
		sprintf_s(dbg_str, sizeof(dbg_str),
			"[fps:%7.2f][count:%3d][1sec:%7.2f][tpf:%7.2f][wait:%7.2f][interval:%7.2f][total:%7.2f]\n",
			(double)fps, (int)dbg_frame_count, (double)f_1sec, (double)f_tpf, (double)f_wait, (double)f_frame_interval, (double)f_total);
		OutputDebugStringA(dbg_str);
		SetCaption(dbg_str);
		dbg_frame_count = 0;
		f_total = 0.0;
#endif // _DEBUG
	}
	i64_t1 = __rdtsc();	//�v���J�n���Ԏ擾
}
#elif (FRAME_SYNC==USE_QPC)
// ���ڍ׌v���p
LARGE_INTEGER	li_frequency = {};
LARGE_INTEGER	li_t1 = {};
LARGE_INTEGER	li_t2 = {};
double f_1sec = 0.0;	//�P�b�̃J�E���g��
double f_tpf = 0.0;		//�P�t���[���̃J�E���g���i����\�j
#ifdef _DEBUG
double f_total = 0.0;
#endif // _DEBUG
/*
* �t���[������
* ���𑜓x�^�C�� �X�^���v���g���Čv��
*/
//������
void InitFrameSync(double _FPS)
{
	if (_FPS <= 0) {
		//�͈͊O�͂PFPS�Ƃ���
		_FPS = 1;
	}
	FPS = _FPS;
	timeBeginPeriod(1);	//�v�����x��1�~���b�ɐݒ�
	//����\�F�J�E���^�̎��g���i�P�b�Ԃɉ��J�E���g�s�����j���擾
	if (!QueryPerformanceFrequency(&li_frequency)) {
		//���s�����ꍇ�́A10,000,000Hz(10MHz)�ɂ���B
		li_frequency.QuadPart = 1000 * 1000 * 10;	//����\��(1/10,000,000)
	}
	f_1sec = (double)li_frequency.QuadPart;	//�P�b�Ԃ̃J�E���g���i����\�j
	f_tpf = (f_1sec / FPS);	//�P�t���[���̃J�E���g���i����\�j
}
//����
void FrameSync(void)
{
	//�t���[���҂����Ԍv��
	QueryPerformanceCounter(&li_t2);	//���ݎ��Ԏ擾
	double f_frame_interval = (double)(li_t2.QuadPart - li_t1.QuadPart);	//�t���[���Ԋu(����)�Z�o
	double f_wait = (f_tpf - f_frame_interval);	//�҂����Ԃ��Z�o�i�҂����ԁ��P�t���[���ɕK�v�Ȏ��ԁ|���ۂɊ|���������ԁj
	if (f_wait > 0) {
		LARGE_INTEGER t2 = {};
		do {
			//Sleep(1);
			//std::this_thread::yield();
			QueryPerformanceCounter(&t2);	//���ݎ��Ԏ擾
			//���̃��[�v�̌o�ߎ��Ԃ𑪂�A�҂����Ԉȉ��Ȃ烋�[�v�p������B
		} while ((double)(t2.QuadPart - li_t2.QuadPart) < f_wait);
#ifdef _DEBUG
		//�P�t���[�����Ԃ�ώZ�i�P�t���[�����ԁ��O�̃t���[�����獡�̃t���[���܂ł̌o�ߎ��ԁ{����Ȃ��������̑҂����ԁj
		f_total += (f_frame_interval + f_wait);
	}
	else {
		//�҂����Ԃ����������̂łP�t���[�����Ԃ�����ώZ
		f_total += f_frame_interval;
	}
	dbg_frame_count++;	//�P�b�Ԃ̃t���[�����J�E���g(�f�o�b�O�p)
	if ((f_total >= f_1sec) && (dbg_frame_count > 0)) {
		double fps = f_1sec / (f_total / (double)dbg_frame_count);
		sprintf_s(dbg_str, sizeof(dbg_str),
			"[fps:%7.2f][count:%3d][1sec:%7.2f][tpf:%7.2f][wait:%7.2f][interval:%7.2f][total:%7.2f]\n",
			(double)fps, (int)dbg_frame_count, (double)f_1sec, (double)f_tpf, (double)f_wait, (double)f_frame_interval, (double)f_total);
		OutputDebugStringA(dbg_str);
		dbg_frame_count = 0;
		f_total = 0.0;
#endif // _DEBUG
	}
	QueryPerformanceCounter(&li_t1);	//�v���J�n���Ԏ擾
}
#elif (FRAME_SYNC==USE_MMSEC)
DWORD	dd_frequency = 0;
DWORD	dd_t1 = 0;
DWORD	dd_t2 = 0;
double f_1sec = 0.0;	//�P�b�̃J�E���g��
double f_tpf = 0.0;		//�P�t���[���̎��ԁi�~���b�j
#ifdef _DEBUG
double f_total = 0.0;
#endif // _DEBUG
/*
* �t���[������
* ���𑜓x�^�C�� �X�^���v���g���Čv��
*/
//������
void InitFrameSync(double _FPS)
{
	if (_FPS <= 0) {
		//�͈͊O�͂PFPS�Ƃ���
		_FPS = 1;
	}
	FPS = _FPS;
	timeBeginPeriod(1);				//����\���P�~���b�ɐݒ�
	dd_frequency = 1000;			//�P�b�̃J�E���g���i�P�O�O�O�~���b�j
	f_1sec = (double)dd_frequency;	//�P�b�̃J�E���g���i�P�O�O�O�~���b�j
	f_tpf = (f_1sec / FPS);	//�P�t���[���̎��ԁi�~���b�j
}
//����
void FrameSync(void)
{
	//�t���[���҂����Ԍv��
	dd_t2 = timeGetTime();	//���ݎ��Ԏ擾
	double f_frame_interval = (double)(dd_t2 - dd_t1);	//�t���[���Ԋu(����)�Z�o
	double f_wait = (f_tpf - f_frame_interval);	//�҂����Ԃ��Z�o�i�҂����ԁ��P�t���[���ɕK�v�Ȏ��ԁ|���ۂɊ|���������ԁj
	if (f_wait > 0) {
		DWORD t2 = 0;
		do {
			Sleep(1);
			//std::this_thread::yield();
			t2 = timeGetTime();	//���ݎ��Ԏ擾
			//���̃��[�v�̌o�ߎ��Ԃ𑪂�A�҂����Ԉȉ��Ȃ烋�[�v�p������B
		} while ((double)(t2 - dd_t2) < f_wait);
#ifdef _DEBUG
		//�P�t���[�����Ԃ�ώZ�i�P�t���[�����ԁ��O�̃t���[�����獡�̃t���[���܂ł̌o�ߎ��ԁ{����Ȃ��������̑҂����ԁj
		f_total += (f_frame_interval + f_wait);
	}
	else {
		//�҂����Ԃ����������̂łP�t���[�����Ԃ�����ώZ
		f_total += f_frame_interval;
	}
	dbg_frame_count++;	//�P�b�Ԃ̃t���[�����J�E���g(�f�o�b�O�p)
	if ((f_total >= f_1sec) && (dbg_frame_count > 0)) {
		double fps = f_1sec / (f_total / (double)dbg_frame_count);
		sprintf_s(dbg_str, sizeof(dbg_str),
			"[fps:%7.2f][count:%3d][1sec:%7.2f][tpf:%7.2f][wait:%7.2f][interval:%7.2f][total:%7.2f]\n",
			(double)fps, (int)dbg_frame_count, (double)f_1sec, (double)f_tpf, (double)f_wait, (double)f_frame_interval, (double)f_total);
		OutputDebugStringA(dbg_str);
		dbg_frame_count = 0;
		f_total = 0.0;
#endif // _DEBUG
	}
	dd_t1 = timeGetTime();	//���ݎ��Ԏ擾
}
#else	// USE_RDTSC
//DWORD	dd_frequency = 0;
int	dd_t1 = 0;
int	dd_t2 = 0;
int mmpf = 0;
void InitFrameSync(double _fps)
{
	if (_fps <= 0) {
		_fps = 1;
	}
	FPS = _fps;
	timeBeginPeriod(1);				//����\���P�~���b�ɐݒ�
	dd_t1 = timeGetTime();	//���ݎ��Ԏ擾
	mmpf = (int)(1000.0 / FPS);
}
void FrameSync(void)
{
	//�t���[���҂����Ԍv��
	dd_t2 = timeGetTime();	//���ݎ��Ԏ擾
	int t_wait = (mmpf - (dd_t2 - dd_t1));
	if (t_wait > 0) {
		Sleep(t_wait);
	}
	dd_t1 = timeGetTime();	//���ݎ��Ԏ擾
}
#endif // USE_RDTSC

/**
* @copyright (c) 2018-2019 HAL Osaka College of Technology & Design (Ihara, H.)
*/
