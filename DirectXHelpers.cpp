#include "DirectXHelpers.h"

//#if defined(_XBOX_ONE) && defined(_TITLE)
//#include <d3d11_x.h>
//#else
//#include <d3d11_1.h>
//#endif
//
//#if !defined(NO_D3D11_DEBUG_NAME) && ( defined(_DEBUG) || defined(PROFILE) )
//#if !defined(_XBOX_ONE) || !defined(_TITLE)
//#pragma comment(lib,"dxguid.lib")
//#endif
//#endif
//
//#ifndef IID_GRAPHICS_PPV_ARGS
//#define IID_GRAPHICS_PPV_ARGS(x) IID_PPV_ARGS(x)
//#endif
//
//#include <cassert>
//#include <cstddef>
//#include <cstdint>
//#include <exception>

HRESULT __cdecl DirectX::CreateInputLayoutFromEffect(ID3D11Device* device, IEffect* effect, const D3D11_INPUT_ELEMENT_DESC* desc, size_t count, ID3D11InputLayout** pInputLayout) noexcept
{
    return E_NOTIMPL;
}
