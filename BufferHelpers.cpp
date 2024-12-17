#include "BufferHelpers.h"

#include <cassert>
#include <cstddef>

#if defined(_XBOX_ONE) && defined(_TITLE)
#include <d3d11_x.h>
#include "GraphicsMemory.h"
#else
#include <d3d11_1.h>
#endif

#include <wrl\client.h>

HRESULT __cdecl DirectX::CreateStaticBuffer(ID3D11Device* device, const void* ptr, size_t count, size_t stride, unsigned int bindFlags, ID3D11Buffer** pBuffer) noexcept
{
    return E_NOTIMPL;
}

HRESULT __cdecl DirectX::CreateTextureFromMemory(ID3D11Device* device, size_t width, DXGI_FORMAT format, const D3D11_SUBRESOURCE_DATA& initData, ID3D11Texture1D** texture, ID3D11ShaderResourceView** textureView, unsigned int bindFlags) noexcept
{
    return E_NOTIMPL;
}

HRESULT __cdecl DirectX::CreateTextureFromMemory(ID3D11Device* device, size_t width, size_t height, DXGI_FORMAT format, const D3D11_SUBRESOURCE_DATA& initData, ID3D11Texture2D** texture, ID3D11ShaderResourceView** textureView, unsigned int bindFlags) noexcept
{
    return E_NOTIMPL;
}

//HRESULT __cdecl DirectX::CreateTextureFromMemory(ID3D11DeviceX* d3dDeviceX, ID3D11DeviceContextX* d3dContextX, ID3D11Device* device, ID3D11DeviceContext* d3dContext, size_t width, size_t height, DXGI_FORMAT format, const D3D11_SUBRESOURCE_DATA& initData, ID3D11Texture2D** texture, ID3D11ShaderResourceView** textureView) noexcept
//{
//    return E_NOTIMPL;
//}

HRESULT __cdecl DirectX::CreateTextureFromMemory(ID3D11Device* device, size_t width, size_t height, size_t depth, DXGI_FORMAT format, const D3D11_SUBRESOURCE_DATA& initData, ID3D11Texture3D** texture, ID3D11ShaderResourceView** textureView, unsigned int bindFlags) noexcept
{
    return E_NOTIMPL;
}

void __cdecl DirectX::Internal::ConstantBufferBase::CreateBuffer(ID3D11Device* device, size_t bytes, ID3D11Buffer** pBuffer)
{
}
