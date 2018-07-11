#pragma once
#include "camera.h"
#include "spotlight.h"

static const unsigned MAX_LIGHTS = 10U;
class texture_shader
{
private:
	struct SCENE
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct CAMERA
	{
		XMVECTOR position; 
		float pad0; 
	};

	struct LIGHT
	{
		std::array<spotlight, MAX_LIGHTS> lights;
		int quantity;
		XMFLOAT3 pad;
	};

	struct MATERIAL
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;
		float power;
		XMFLOAT3 pad;
	};

	struct ANIMATION
	{
		BOOL aniamtion;
		float time;
		XMFLOAT2 pad; 
	};

public:
	texture_shader(void);
	~texture_shader(void);

	HRESULT initialize(ComPtr<ID3D11Device> const& device, LPCWSTR const& filename);
	void set_shader_resource(ComPtr<ID3D11DeviceContext> const& context, XMMATRIX const& world, XMMATRIX const& view, XMMATRIX const& projection) const;
	void set_shader_resource(ComPtr<ID3D11DeviceContext> const& context, std::shared_ptr<camera> const& camera) const;
	void set_shader_resource(ComPtr<ID3D11DeviceContext> const& context, std::vector<std::shared_ptr<spotlight>> const& spot_lights) const;
	void set_shader_resource(ComPtr<ID3D11DeviceContext> const& context, bool const& animation, float const& time) const;

	void render(ComPtr<ID3D11DeviceContext> const& context, std::shared_ptr<CDXUTSDKMesh>& mesh, bool const& textured);

private: 
	ComPtr<ID3D11VertexShader> _vertex_shader;
	ComPtr<ID3D11PixelShader> _pixel_shader;
	ComPtr<ID3D11InputLayout> _input_layout;
	ComPtr<ID3D11SamplerState> _sampler_state;

	ComPtr<ID3D11Buffer> _scene;
	ComPtr<ID3D11Buffer> _camera;
	ComPtr<ID3D11Buffer> _light;
	ComPtr<ID3D11Buffer> _material;
	ComPtr<ID3D11Buffer> _animation;
};

