//--------------------------------------------------------------------------------------
// 頂点シェーダー
//--------------------------------------------------------------------------------------

// 頂点のデータを表す構造体（受け取り用）
struct VS_IN
{
    float4 pos : POSITION;
    float4 col : COLOR0;
    float2 tex : TEX;
};

// 頂点のデータを表す構造体（送信用） 
struct VS_OUT
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
    float2 tex : TEXCOORD;
};
cbuffer ConstBuffer : register(b0)
{
    float4 vertexColor;
    matrix matrixTex;
    matrix matrixProj;
    matrix matrixWorld;
}
// 頂点シェーダーのエントリポイント 
VS_OUT vs_main(VS_IN input)
{
    VS_OUT output;
    output.pos = mul(input.pos, matrixWorld);
    output.pos = mul(output.pos, matrixProj);
    float4 uv;
    uv.xy = input.tex;
    uv.z = 0.0f;
    uv.w = 1.0f;
    uv = mul(uv, matrixTex);
    output.tex = uv.xy;
    output.col = input.col * vertexColor;
    return output;
}