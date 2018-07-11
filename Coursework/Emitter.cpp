#include "pch.h"
#include "Emitter.h"

Emitter::Emitter(XMFLOAT3 const& position) : _position(position)
{

}

Emitter::~Emitter(void)
{

}

HRESULT Emitter::Initialize(ComPtr<ID3D11Device> const& device, std::wstring const& path)
{
	auto hr = static_cast<HRESULT>(0L);

	// Load texture  
	V_RETURN(CreateDDSTextureFromFile(device.Get(), path.c_str(),
		nullptr, _texture.GetAddressOf()));

	for (auto const& p : _particles)
	{
		p->Initialize(device);
	}

	return hr;
}

void Emitter::Update()
{
	std::vector<std::vector<std::shared_ptr<Particles>>::iterator> dead;
	for (auto it = _particles.begin(); it != _particles.end(); ++it)
	{
		auto const particle = (*it);
		if (particle->Alive())
		{
			particle->Update();
		}
		else
		{
			dead.emplace_back(it);
		}
	}

	for (auto it = dead.crbegin(); it != dead.crend(); ++it)
	{
		_particles.erase(*it);
	}
}

void Emitter::Emit(ComPtr<ID3D11Device> const& device, unsigned const& amount)
{
	if (_particles.size() > 5000)
		return; 

	for (auto i = 0U; i < amount; ++i)
	{
		_particles.emplace_back(std::make_shared<Particles>(device));
	}
}

void Emitter::Render(ComPtr<ID3D11DeviceContext> const& context, std::shared_ptr<Particle_Shader> const& shader, std::shared_ptr<camera> const& camera, XMMATRIX const& projection, bool const& textured)
{
	if (textured)
		context->PSSetShaderResources( 0U, 1U, _texture.GetAddressOf());

	for (auto const& p : _particles)
	{
		if (p->Alive())
		{
			XMMATRIX world = p->World() * XMMatrixTranslation(_position.x, _position.y, _position.z);
			
			auto const x = _position.x - camera->Position().x;
			auto const z = _position.z - camera->Position().z;
			auto const angle = static_cast<float>(std::atan2(x, z));
			world *= XMMatrixRotationY(angle);

			shader->set_shader_resource(context, world, camera->View(), projection);
			p->Render(context);
			shader->Render(context);
		}
	}
}