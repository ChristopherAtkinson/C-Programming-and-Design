#include "pch.h"
#include "Direct.h"

Direct::Direct(void) : _vsync(false), is_solid(true)
{
	
}

Direct::~Direct(void)
{

}

HRESULT Direct::Initialize(HWND const& handle, std::shared_ptr<Config> const& settings)
{
	auto const width = settings->Read<unsigned>(L"general", L"width");
	auto const height = settings->Read<unsigned>(L"general", L"height");
	_vsync = settings->Read<bool>(L"general", L"vsync");

	auto hr = static_cast<HRESULT>(0L);
	// Create Factory from interface
	auto factory = ComPtr<IDXGIFactory>{};
	V_RETURN(CreateDXGIFactory(__uuidof(IDXGIFactory),
		reinterpret_cast<void**>(factory.GetAddressOf())))

	// Create Adapter using previous Factory
	auto adapter = ComPtr<IDXGIAdapter>{};
	V_RETURN(factory->EnumAdapters(0U, &adapter))

	// Enumerate the primary adapter output (monitor)
	auto output = ComPtr<IDXGIOutput>{};
	V_RETURN(adapter->EnumOutputs(0U, &output))

	// Count the number of modes supported by the display
	auto count = unsigned{};
	V_RETURN(output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED, &count, nullptr))

	// Populate list with mode list
	auto display_modes = std::vector<DXGI_MODE_DESC>(static_cast<size_t>(count)); 
	V_RETURN(output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED, &count, display_modes.data()))

	// Loop through each mode to determain a match for width and height,
	// When match is found, store the numerator and denominator of refresh rate
	auto refresh_rate = DXGI_RATIONAL{ 0U, 1U };
	for (auto const& it : display_modes)
	{
		if (it.Width == width && it.Height == height)
			refresh_rate = it.RefreshRate;
	}

	auto const windowed = settings->Read<bool>(L"general", L"windowed"); ; 

	// Create description for swap chain 
	auto const swap_chain_desc = DXGI_SWAP_CHAIN_DESC
	{
		{
			width, height,
			refresh_rate,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
			DXGI_MODE_SCALING_UNSPECIFIED
		},
		{ 1U, 0U },
		DXGI_USAGE_RENDER_TARGET_OUTPUT,
		1U,
		handle,
		windowed,
		DXGI_SWAP_EFFECT_DISCARD,
		0U
	};

	// Create device, context and swap chain
	auto constexpr feature_level = D3D_FEATURE_LEVEL_11_0;
	V_RETURN(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
		nullptr, 0U, &feature_level, 1U, D3D11_SDK_VERSION, &swap_chain_desc,
		_swap_chain.GetAddressOf(), _device.GetAddressOf(), nullptr,
		_context.GetAddressOf()))

	// Retrieve pointer to the back buffer from swap chain 
	auto back_buffer = ComPtr<ID3D11Texture2D>{};
	V_RETURN(_swap_chain->GetBuffer(0U, __uuidof(ID3D11Texture2D),
		reinterpret_cast<LPVOID*>(back_buffer.GetAddressOf())))

	// Create render target view using back buffer 
	V_RETURN(_device->CreateRenderTargetView(back_buffer.Get(), nullptr,
		&_render_target_view))

	// Create description for depth buffer
	auto const depth_buffer_desc = D3D11_TEXTURE2D_DESC
	{
		width, height, 
		1U, 1U,
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		{ 1U, 0U },
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_DEPTH_STENCIL,
		0U, 0U
	};

	// Create depth buffer texture using the previous description
	V_RETURN(_device->CreateTexture2D(&depth_buffer_desc, nullptr, 
		_depth_stencil_buffer.GetAddressOf()))

	// Create description for depth stencil 
	auto const depth_stencil_desc = D3D11_DEPTH_STENCIL_DESC
	{
		TRUE,
		D3D11_DEPTH_WRITE_MASK_ALL,
		D3D11_COMPARISON_LESS,
		TRUE,
		0xFF, 0xFF,
		{
			D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_INCR, D3D11_STENCIL_OP_KEEP,
			D3D11_COMPARISON_ALWAYS
		},
		{
			D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_INCR, D3D11_STENCIL_OP_KEEP,
			D3D11_COMPARISON_ALWAYS
		}
	};

	// Create depth stencil state
	V_RETURN(_device->CreateDepthStencilState(&depth_stencil_desc,
		_depth_stencil_state.GetAddressOf()))

	// Set the depth stencil view
	_context->OMSetDepthStencilState(_depth_stencil_state.Get(), 1);

	// Create description for depth stencil view 
	auto constexpr depth_stencil_view_desc = D3D11_DEPTH_STENCIL_VIEW_DESC
	{
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		D3D11_DSV_DIMENSION_TEXTURE2D,
		0U,
		0U
	};

	// Create depth stencil view
	V_RETURN(_device->CreateDepthStencilView(_depth_stencil_buffer.Get(),
		&depth_stencil_view_desc, _depth_stencil_view.GetAddressOf()))

	// Bind target view and depth stencil buffer to the pipeline
	_context->OMSetRenderTargets(1U, _render_target_view.GetAddressOf(),
		_depth_stencil_view.Get());

	// Solid
	// Create description for rasterizer 
	auto constexpr raster_desc_solid = D3D11_RASTERIZER_DESC
	{

		D3D11_FILL_SOLID, D3D11_CULL_FRONT,
		FALSE,
		0, .0f, .0f, TRUE,
		FALSE, FALSE, FALSE
	};

	// Create rasterizer state for previous description 
	V_RETURN(_device->CreateRasterizerState(&raster_desc_solid,
		_raster_state_solid.GetAddressOf()))

	// Wireframe
	// Create description for rasterizer 
	auto constexpr raster_desc_wireframe = D3D11_RASTERIZER_DESC
	{
		D3D11_FILL_WIREFRAME, D3D11_CULL_NONE,
		FALSE,
		0, .0f, .0f, TRUE,
		FALSE, FALSE, FALSE
	};

	// Create rasterizer state for previous description 
	V_RETURN(_device->CreateRasterizerState(&raster_desc_wireframe,
		_raster_state_wireframe.GetAddressOf()))

	// Set rasterizer state
	_context->RSSetState(_raster_state_solid.Get());

	// Create description for viewport
	auto const viewport = D3D11_VIEWPORT
	{
		.0f, .0f,
		static_cast<float>(width), static_cast<float>(height),
		.0f, 1.0f
	};

	// Create viewport
	_context->RSSetViewports(1U, &viewport);

	auto const faspect = static_cast<float>(width / height);
	_projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), faspect, 0.1f, 1000.0f);

	TwInit(TW_DIRECT3D11, _device.Get());
	TwWindowSize(width, height);

	return hr;
}

void Direct::Start(float const r, float const g, float const b, float const a)
{
	auto const color = std::array<float, 4> { r, g, b, a };
	_context->ClearRenderTargetView(_render_target_view.Get(), color.data());
	_context->ClearDepthStencilView( _depth_stencil_view.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0U );

	auto constexpr blend_desc = D3D11_BLEND_DESC
	{
		FALSE, FALSE,
		TRUE,
		D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE, D3D11_BLEND_ZERO, D3D11_BLEND_OP_ADD,
		0x0F
	};

	static ID3D11BlendState *blend_state = nullptr;
	if (nullptr == blend_state)
	{
		_device->CreateBlendState(&blend_desc, &blend_state);
	}

	_context->OMSetBlendState(blend_state, nullptr, 0xffffffff);

	TwDraw();
}

void Direct::End(void)
{
	TwDraw();

	if (_vsync)
		_swap_chain->Present(1U, 0U);
	else
		_swap_chain->Present(0U, 0U);

	auto constexpr blend_desc = D3D11_BLEND_DESC
	{
		FALSE, FALSE,
		FALSE,
		D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE, D3D11_BLEND_ZERO, D3D11_BLEND_OP_ADD,
		0x0F
	};

	static ID3D11BlendState *blend_state = nullptr;
	if (nullptr == blend_state)
	{
		_device->CreateBlendState(&blend_desc, &blend_state);
	}

	_context->OMSetBlendState(blend_state, nullptr, 0xffffffff);
}