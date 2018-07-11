#pragma once

class Particle_Shader
{
private:
	struct SCENE
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

public:
	explicit Particle_Shader();
	~Particle_Shader();

	HRESULT initialize(ComPtr<ID3D11Device> const& device, LPCWSTR const& filename);
	void set_shader_resource(ComPtr<ID3D11DeviceContext> const& context, XMMATRIX const& world, XMMATRIX const& view, XMMATRIX const& projection) const;
	void Render(ComPtr<ID3D11DeviceContext> const& context);

private:
	ComPtr<ID3D11VertexShader> _vertex_shader;
	ComPtr<ID3D11PixelShader> _pixel_shader;
	ComPtr<ID3D11InputLayout> _input_layout;
	ComPtr<ID3D11SamplerState> _sampler_state;

	ComPtr<ID3D11Buffer> _scene;
};