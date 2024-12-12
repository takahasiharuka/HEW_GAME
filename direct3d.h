#pragma once
// ■■■■■■■■■■■■■■■■■■■■■■■■■
//↓direcgt3d.hへ移動
#include <d3d11.h>  // DirectX11を使うためのヘッダーファイル
#include <DirectXMath.h>
#define SCREEN_WIDTH (640)	// ウインドウの幅
#define SCREEN_HEIGHT (480)	// ウインドウの高さ

// 関数のプロトタイプ宣言
HRESULT D3D_Create(HWND hwnd);
void    D3D_Release();
//void    D3D_Render();
void D3D_StartRender();
void D3D_FinishRender();
// 構造体の定義
// 頂点データを表す構造体
struct Vertex
{
	// 頂点の位置座標
	float x, y, z;
	//色
	float r, g, b, a;
	//テクスチャ座標
	float u, v;
};
struct ConstBuffer
{
	DirectX::XMFLOAT4 color;
	DirectX::XMMATRIX matrixTex;
	DirectX::XMMATRIX matrixProj;
	DirectX::XMMATRIX matrixWorld;
};
extern ID3D11Device* g_pDevice;
extern ID3D11DeviceContext* g_pDeviceContext;
extern ID3D11Buffer* g_pConstantBuffer;
#define SAFE_RELEASE(p){if(NULL !=p){p->Release();p = NULL;}}
//↑direcgt3d.hへ移動
//■■■■■■■■■■■■■■■■■■■■■■■■■


