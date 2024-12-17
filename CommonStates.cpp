#include "CommonStates.h"

#if defined(_XBOX_ONE) && defined(_TITLE)
#include <d3d11_x.h>
#else
#include <d3d11_1.h>
#endif

#include <memory>

DirectX::CommonStates::CommonStates(ID3D11Device* device)
{
}

DirectX::CommonStates::CommonStates(CommonStates&& moveFrom) noexcept
{
}

DirectX::CommonStates::~CommonStates()
{
}

ID3D11BlendState* __cdecl DirectX::CommonStates::Opaque() const
{
	return nullptr;
}

ID3D11BlendState* __cdecl DirectX::CommonStates::AlphaBlend() const
{
	return nullptr;
}

ID3D11BlendState* __cdecl DirectX::CommonStates::Additive() const
{
	return nullptr;
}

ID3D11BlendState* __cdecl DirectX::CommonStates::NonPremultiplied() const
{
	return nullptr;
}

ID3D11DepthStencilState* __cdecl DirectX::CommonStates::DepthNone() const
{
	return nullptr;
}

ID3D11DepthStencilState* __cdecl DirectX::CommonStates::DepthDefault() const
{
	return nullptr;
}

ID3D11DepthStencilState* __cdecl DirectX::CommonStates::DepthRead() const
{
	return nullptr;
}

ID3D11RasterizerState* __cdecl DirectX::CommonStates::CullNone() const
{
	return nullptr;
}

ID3D11RasterizerState* __cdecl DirectX::CommonStates::CullClockwise() const
{
	return nullptr;
}

ID3D11RasterizerState* __cdecl DirectX::CommonStates::CullCounterClockwise() const
{
	return nullptr;
}

ID3D11RasterizerState* __cdecl DirectX::CommonStates::Wireframe() const
{
	return nullptr;
}

ID3D11SamplerState* __cdecl DirectX::CommonStates::PointWrap() const
{
	return nullptr;
}

ID3D11SamplerState* __cdecl DirectX::CommonStates::PointClamp() const
{
	return nullptr;
}

ID3D11SamplerState* __cdecl DirectX::CommonStates::LinearWrap() const
{
	return nullptr;
}

ID3D11SamplerState* __cdecl DirectX::CommonStates::LinearClamp() const
{
	return nullptr;
}

ID3D11SamplerState* __cdecl DirectX::CommonStates::AnisotropicWrap() const
{
	return nullptr;
}

ID3D11SamplerState* __cdecl DirectX::CommonStates::AnisotropicClamp() const
{
	return nullptr;
}
