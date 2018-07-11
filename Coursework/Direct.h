#pragma once
#include "Config.h"

class Direct
{
public:
	Direct(void);
	~Direct(void);

	HRESULT Initialize(HWND const& handle, std::shared_ptr<Config> const& settings);

	void Start(float const r, float const g, float const b, float const a);
	void End(void);

	inline void Toggle_Raster_State(void)
	{
		if (is_solid)
		{
			_context->RSSetState(_raster_state_wireframe.Get());
			is_solid = false;
		}
		else
		{
			_context->RSSetState(_raster_state_solid.Get());
			is_solid = true;
		}
	}

	inline ComPtr<ID3D11Device> const& Device(void) const
	{ return _device; }
	inline ComPtr<ID3D11DeviceContext> const& Context(void) const
	{ return _context; }
	inline XMMATRIX const& Projection(void) const 
	{ return _projection; }

private:
	bool _vsync;

	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11DeviceContext> _context;
	ComPtr<IDXGISwapChain> _swap_chain; 
	ComPtr<ID3D11RenderTargetView> _render_target_view; 
	ComPtr<ID3D11Texture2D> _depth_stencil_buffer;
	ComPtr<ID3D11DepthStencilState> _depth_stencil_state;
	ComPtr<ID3D11DepthStencilView> _depth_stencil_view;

	bool is_solid;
	ComPtr<ID3D11RasterizerState> _raster_state_solid;
	ComPtr<ID3D11RasterizerState> _raster_state_wireframe;

	XMMATRIX _projection;
};