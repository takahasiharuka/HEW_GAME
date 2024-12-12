#pragma once

#include"direct3d.h"
#include "WICTextureLoader.h"
// ※ID3D11で始まるポインタ型の変数は、解放する必要がある
class Object {

private:
	int velocity;
	Vertex vertexList[4] =
	{
		// x      y      z   r    g    b     a   u    v
	  {  -0.5f,  0.5f, 0.5f,1.0f,1.0f,1.0f,1.0f,0.0f,0.0f },  // ０番目の頂点座標　{ x, y, z }
	  {   0.5f,  0.5f, 0.5f,1.0f,1.0f,1.0f,1.0f,1.0f,0.0f },  // １番目の頂点座標
	  { -0.5f,  -0.5f, 0.5f,1.0f,1.0f,1.0f,1.0f,0.0f,1.0f },  // ２番目の頂点座標
	  {  0.5f,  -0.5f, 0.5f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f },  // 3番目の頂点座標　{ x, y, z }
	};
	//座標
	DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 size = { 100.0f,100.0f,0.0f };
	float angle = 0.0f;
	DirectX::XMFLOAT3 kyarapos = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 kyarasize = { 100.0f,100.0f,0.0f };
	float kyaraangle = 0.0f;
	DirectX::XMFLOAT3 lovepos = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 lovesize = { 100.0f,100.0f,0.0f };
	float loveangle = 0.0f;
	DirectX::XMFLOAT3 taraipos = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 taraisize = { 100.0f,100.0f,0.0f };
	float taraiangle = 0.0f;
	DirectX::XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f };
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11ShaderResourceView* m_pTextureView;
	int splitX = 1;
	int splitY = 1;
public:
	float numU = 0;
	float numV = 0;
	void Init(const wchar_t* imgname, int sx = 1, int sy = 1); //初期化​
	void Draw();//描画​
	void Uninit();//終了​
	void SetPos(float x, float y, float z);
	void SetSize(float x, float y, float z);
	void SetAngle(float a);
	void SetColor(float r, float g, float b, float a);
	DirectX::XMFLOAT3 GetPos(void);
	DirectX::XMFLOAT3 GetSize(void);
	float GetAngle(void);

	struct State {
		float x;
	};


};
