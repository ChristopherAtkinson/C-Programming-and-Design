#pragma once
#include "Particles.h"
#include "Particle_Shader.h"
#include "camera.h"

class Emitter
{
public:
	explicit Emitter(XMFLOAT3 const& position);
	~Emitter(void);

	HRESULT Initialize(ComPtr<ID3D11Device> const& device, std::wstring const& path);

	XMFLOAT3 const& Position(void) const
	{ return _position; }

	void Update(void);
	void Emit(ComPtr<ID3D11Device> const& device, unsigned const& amount);
	void Render(ComPtr<ID3D11DeviceContext> const& context, std::shared_ptr<Particle_Shader> const& shader, std::shared_ptr<camera> const& camera, XMMATRIX const& projection, bool const& textured);

private:
	ComPtr<ID3D11ShaderResourceView> _texture; 
	std::vector<std::shared_ptr<Particles>> _particles;

	XMFLOAT3 _position; 
};