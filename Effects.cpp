#include "Effects.h"

#if defined(_XBOX_ONE) && defined(_TITLE)
#include <d3d11_x.h>
#else
#include <d3d11_1.h>
#endif

#include <cstddef>
#include <memory>
#include <DirectXMath.h>

#include <Windows.h>

void XM_CALLCONV DirectX::IEffectMatrices::SetMatrices(FXMMATRIX world, CXMMATRIX view, CXMMATRIX projection)
{
    //return void XM_CALLCONV();
}

DirectX::BasicEffect::BasicEffect(ID3D11Device* device)
{
}

DirectX::BasicEffect::BasicEffect(BasicEffect&& moveFrom) noexcept
{
}

//BasicEffect& DirectX::BasicEffect::operator=(BasicEffect&& moveFrom) noexcept
//{
//    // TODO: insert return statement here
//}

DirectX::BasicEffect::~BasicEffect()
{
}

void __cdecl DirectX::BasicEffect::Apply(ID3D11DeviceContext* deviceContext)
{
}

void __cdecl DirectX::BasicEffect::GetVertexShaderBytecode(void const** pShaderByteCode, size_t* pByteCodeLength)
{
}

void XM_CALLCONV DirectX::BasicEffect::SetWorld(FXMMATRIX value)
{
    //return void XM_CALLCONV();
}

void XM_CALLCONV DirectX::BasicEffect::SetView(FXMMATRIX value)
{
    //return void XM_CALLCONV();
}

void XM_CALLCONV DirectX::BasicEffect::SetProjection(FXMMATRIX value)
{
    //return void XM_CALLCONV();
}

void XM_CALLCONV DirectX::BasicEffect::SetMatrices(FXMMATRIX world, CXMMATRIX view, CXMMATRIX projection)
{
   // return void XM_CALLCONV();
}

void XM_CALLCONV DirectX::BasicEffect::SetDiffuseColor(FXMVECTOR value)
{
    //return void XM_CALLCONV();
}

void XM_CALLCONV DirectX::BasicEffect::SetEmissiveColor(FXMVECTOR value)
{
   // return void XM_CALLCONV();
}

void XM_CALLCONV DirectX::BasicEffect::SetSpecularColor(FXMVECTOR value)
{
   // return void XM_CALLCONV();
}

void __cdecl DirectX::BasicEffect::SetSpecularPower(float value)
{
}

void __cdecl DirectX::BasicEffect::DisableSpecular()
{
}

void __cdecl DirectX::BasicEffect::SetAlpha(float value)
{
}

void XM_CALLCONV DirectX::BasicEffect::SetColorAndAlpha(FXMVECTOR value)
{
   // return void XM_CALLCONV();
}

void __cdecl DirectX::BasicEffect::SetLightingEnabled(bool value)
{
}

void __cdecl DirectX::BasicEffect::SetPerPixelLighting(bool value)
{
}

void XM_CALLCONV DirectX::BasicEffect::SetAmbientLightColor(FXMVECTOR value)
{
   // return void XM_CALLCONV();
}

void __cdecl DirectX::BasicEffect::SetLightEnabled(int whichLight, bool value)
{
}

void XM_CALLCONV DirectX::BasicEffect::SetLightDirection(int whichLight, FXMVECTOR value)
{
   // return void XM_CALLCONV();
}

void XM_CALLCONV DirectX::BasicEffect::SetLightDiffuseColor(int whichLight, FXMVECTOR value)
{
   // return void XM_CALLCONV();
}

void XM_CALLCONV DirectX::BasicEffect::SetLightSpecularColor(int whichLight, FXMVECTOR value)
{
   // return void XM_CALLCONV();
}

void __cdecl DirectX::BasicEffect::EnableDefaultLighting()
{
}

void __cdecl DirectX::BasicEffect::SetFogEnabled(bool value)
{
}

void XM_CALLCONV DirectX::BasicEffect::SetFogColor(FXMVECTOR value)
{
   // return void XM_CALLCONV();
}

void __cdecl DirectX::BasicEffect::SetVertexColorEnabled(bool value)
{
}

void __cdecl DirectX::BasicEffect::SetTextureEnabled(bool value)
{
}

void __cdecl DirectX::BasicEffect::SetTexture(ID3D11ShaderResourceView* value)
{
}

void __cdecl DirectX::BasicEffect::SetBiasedVertexNormals(bool value)
{
}

DirectX::AlphaTestEffect::AlphaTestEffect(ID3D11Device* device)
{
}

DirectX::AlphaTestEffect::AlphaTestEffect(AlphaTestEffect&& moveFrom) noexcept
{
}

//AlphaTestEffect& DirectX::AlphaTestEffect::operator=(AlphaTestEffect&& moveFrom) noexcept
//{
//    // TODO: insert return statement here
//}

DirectX::AlphaTestEffect::~AlphaTestEffect()
{
}

void __cdecl DirectX::AlphaTestEffect::GetVertexShaderBytecode(void const** pShaderByteCode, size_t* pByteCodeLength)
{
}

void XM_CALLCONV DirectX::AlphaTestEffect::SetWorld(FXMMATRIX value)
{
   // return void XM_CALLCONV();
}

void XM_CALLCONV DirectX::AlphaTestEffect::SetView(FXMMATRIX value)
{
   // return void XM_CALLCONV();
}

void XM_CALLCONV DirectX::AlphaTestEffect::SetProjection(FXMMATRIX value)
{
   // return void XM_CALLCONV();
}

void XM_CALLCONV DirectX::AlphaTestEffect::SetMatrices(FXMMATRIX world, CXMMATRIX view, CXMMATRIX projection)
{
   // return void XM_CALLCONV();
}

void XM_CALLCONV DirectX::AlphaTestEffect::SetDiffuseColor(FXMVECTOR value)
{
   // return void XM_CALLCONV();
}

void __cdecl DirectX::AlphaTestEffect::SetAlpha(float value)
{
}

void XM_CALLCONV DirectX::AlphaTestEffect::SetColorAndAlpha(FXMVECTOR value)
{
   // return void XM_CALLCONV();
}

void __cdecl DirectX::AlphaTestEffect::SetFogEnabled(bool value)
{
}

void __cdecl DirectX::AlphaTestEffect::SetFogStart(float value)
{
}

void __cdecl DirectX::AlphaTestEffect::SetFogEnd(float value)
{
}

void XM_CALLCONV DirectX::AlphaTestEffect::SetFogColor(FXMVECTOR value)
{
   // return void XM_CALLCONV();
}

void __cdecl DirectX::AlphaTestEffect::SetVertexColorEnabled(bool value)
{
}

void __cdecl DirectX::AlphaTestEffect::SetTexture(ID3D11ShaderResourceView* value)
{
}

void __cdecl DirectX::AlphaTestEffect::SetAlphaFunction(D3D11_COMPARISON_FUNC value)
{
}

void __cdecl DirectX::AlphaTestEffect::SetReferenceAlpha(int value)
{
}

DirectX::DualTextureEffect::DualTextureEffect(ID3D11Device* device)
{
}

DirectX::DualTextureEffect::DualTextureEffect(DualTextureEffect&& moveFrom) noexcept
{
}

//DualTextureEffect& DirectX::DualTextureEffect::operator=(DualTextureEffect&& moveFrom) noexcept
//{
//    // TODO: insert return statement here
//}

DirectX::DualTextureEffect::~DualTextureEffect()
{
}

void __cdecl DirectX::DualTextureEffect::GetVertexShaderBytecode(void const** pShaderByteCode, size_t* pByteCodeLength)
{
}

void XM_CALLCONV DirectX::DualTextureEffect::SetWorld(FXMMATRIX value)
{
   // return void XM_CALLCONV();
}

void XM_CALLCONV DirectX::DualTextureEffect::SetView(FXMMATRIX value)
{
   // return void XM_CALLCONV();
}

void XM_CALLCONV DirectX::DualTextureEffect::SetProjection(FXMMATRIX value)
{
   // return void XM_CALLCONV();
}

void XM_CALLCONV DirectX::DualTextureEffect::SetMatrices(FXMMATRIX world, CXMMATRIX view, CXMMATRIX projection)
{
   // return void XM_CALLCONV();
}

void XM_CALLCONV DirectX::DualTextureEffect::SetDiffuseColor(FXMVECTOR value)
{
   // return void XM_CALLCONV();
}

void __cdecl DirectX::DualTextureEffect::SetAlpha(float value)
{
}

void XM_CALLCONV DirectX::DualTextureEffect::SetColorAndAlpha(FXMVECTOR value)
{
   // return void XM_CALLCONV();
}

void __cdecl DirectX::DualTextureEffect::SetFogEnabled(bool value)
{
}

void __cdecl DirectX::DualTextureEffect::SetFogStart(float value)
{
}

void __cdecl DirectX::DualTextureEffect::SetFogEnd(float value)
{
}

void XM_CALLCONV DirectX::DualTextureEffect::SetFogColor(FXMVECTOR value)
{
   // return void XM_CALLCONV();
}

void __cdecl DirectX::DualTextureEffect::SetVertexColorEnabled(bool value)
{
}

void __cdecl DirectX::DualTextureEffect::SetTexture(ID3D11ShaderResourceView* value)
{
}

void __cdecl DirectX::DualTextureEffect::SetTexture2(ID3D11ShaderResourceView* value)
{
}

DirectX::EnvironmentMapEffect::EnvironmentMapEffect(ID3D11Device* device)
{
}

DirectX::EnvironmentMapEffect::EnvironmentMapEffect(EnvironmentMapEffect&& moveFrom) noexcept
{
}

//EnvironmentMapEffect& DirectX::EnvironmentMapEffect::operator=(EnvironmentMapEffect&& moveFrom) noexcept
//{
//    // TODO: insert return statement here
//}

DirectX::EnvironmentMapEffect::~EnvironmentMapEffect()
{
}

void __cdecl DirectX::EnvironmentMapEffect::GetVertexShaderBytecode(void const** pShaderByteCode, size_t* pByteCodeLength)
{
}

void XM_CALLCONV DirectX::EnvironmentMapEffect::SetWorld(FXMMATRIX value)
{
   // return void XM_CALLCONV();
}

void XM_CALLCONV DirectX::EnvironmentMapEffect::SetView(FXMMATRIX value)
{
  //  return void XM_CALLCONV();
}

void XM_CALLCONV DirectX::EnvironmentMapEffect::SetProjection(FXMMATRIX value)
{
   // return void XM_CALLCONV();
}

void XM_CALLCONV DirectX::EnvironmentMapEffect::SetMatrices(FXMMATRIX world, CXMMATRIX view, CXMMATRIX projection)
{
   // return void XM_CALLCONV();
}

void XM_CALLCONV DirectX::EnvironmentMapEffect::SetDiffuseColor(FXMVECTOR value)
{
   // return void XM_CALLCONV();
}

void XM_CALLCONV DirectX::EnvironmentMapEffect::SetEmissiveColor(FXMVECTOR value)
{
   // return void XM_CALLCONV();
}

void __cdecl DirectX::EnvironmentMapEffect::SetAlpha(float value)
{
}

void XM_CALLCONV DirectX::EnvironmentMapEffect::SetColorAndAlpha(FXMVECTOR value)
{
   // return void XM_CALLCONV();
}

void XM_CALLCONV DirectX::EnvironmentMapEffect::SetAmbientLightColor(FXMVECTOR value)
{
   // return void XM_CALLCONV();
}

void __cdecl DirectX::EnvironmentMapEffect::SetPerPixelLighting(bool value)
{
}

void __cdecl DirectX::EnvironmentMapEffect::SetLightEnabled(int whichLight, bool value)
{
}

void XM_CALLCONV DirectX::EnvironmentMapEffect::SetLightDirection(int whichLight, FXMVECTOR value)
{
   // return void XM_CALLCONV();
}

void XM_CALLCONV DirectX::EnvironmentMapEffect::SetLightDiffuseColor(int whichLight, FXMVECTOR value)
{
   // return void XM_CALLCONV();
}

void __cdecl DirectX::EnvironmentMapEffect::EnableDefaultLighting()
{
}

void __cdecl DirectX::EnvironmentMapEffect::SetFogEnabled(bool value)
{
}

void __cdecl DirectX::EnvironmentMapEffect::SetFogStart(float value)
{
}

void __cdecl DirectX::EnvironmentMapEffect::SetFogEnd(float value)
{
}

void XM_CALLCONV DirectX::EnvironmentMapEffect::SetFogColor(FXMVECTOR value)
{
   // return void XM_CALLCONV();
}

void __cdecl DirectX::EnvironmentMapEffect::SetTexture(ID3D11ShaderResourceView* value)
{
}

void __cdecl DirectX::EnvironmentMapEffect::SetEnvironmentMap(ID3D11ShaderResourceView* value)
{
}

void __cdecl DirectX::EnvironmentMapEffect::SetMode(Mapping mapping)
{
}

void __cdecl DirectX::EnvironmentMapEffect::SetEnvironmentMapAmount(float value)
{
}

void XM_CALLCONV DirectX::EnvironmentMapEffect::SetEnvironmentMapSpecular(FXMVECTOR value)
{
   // return void XM_CALLCONV();
}

void __cdecl DirectX::EnvironmentMapEffect::SetFresnelFactor(float value)
{
}

void __cdecl DirectX::EnvironmentMapEffect::SetBiasedVertexNormals(bool value)
{
}

void __cdecl DirectX::EnvironmentMapEffect::SetLightingEnabled(bool value)
{
}

void XM_CALLCONV DirectX::EnvironmentMapEffect::SetLightSpecularColor(int whichLight, FXMVECTOR value)
{
   // return void XM_CALLCONV();
}
