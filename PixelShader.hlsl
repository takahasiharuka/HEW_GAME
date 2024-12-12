//--------------------------------------------------------------------------------------
// ピクセルシェーダー
//--------------------------------------------------------------------------------------

// ピクセルの情報の構造体（受け取り用）
struct PS_IN
{
    // float4型　→　float型が４つの構造体
    float4 pos : SV_POSITION; // ピクセルの画面上の座標
    float4 col : COLOR0;
    float2 tex : TEXCOORD;
};
Texture2D myTexture : register(t0);
SamplerState mySampler : register(s0);
// ピクセルシェーダーのエントリポイント
float4 ps_main(PS_IN input) : SV_Target
{
    float4 color = myTexture.Sample(mySampler, input.tex);
    color = color * input.col;
    
    return color;
    

}
