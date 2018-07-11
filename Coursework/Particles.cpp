#include "pch.h"
#include "Particles.h"

Particles::Particles(ComPtr<ID3D11Device> const& device)
	: _vertex_buffer(nullptr), _index_buffer(nullptr), _time(100)
{
	_time = rand() % 256;
	Initialize(device);
}

Particles::~Particles()
{

}

HRESULT Particles::Initialize(ComPtr<ID3D11Device> const& device)
{
	auto hr = static_cast<HRESULT>(0L);

	// Define quad
	auto const vertices = std::array<vertex, 6>
	{ {
		{ { -1.f, -1.f, 0.f }, { 1.f, 1.f }, {} },	// bottom left
		{ { -1.f, 1.f, 0.f }, { 1.f, 0.f }, {} },	// top left
		{ { 1.f, -1.f, 0.0f }, { 0.f, 1.f }, {} },	// bottom right
		{ { 1.f, 1.f, 0.0f }, { 0.f, 0.f }, {} },	// top right
		{ { 1.f, -1.f, 0.0f }, { 0.f, 1.f }, {} },	// bottom right
		{ { -1.f, 1.f, 0.0f }, { 1.f, 0.f }, {} }	// top left
	} };

	auto const vertex_buffer_desc = D3D11_BUFFER_DESC
	{
		static_cast<UINT>(sizeof(vertex) * vertices.size()),
		D3D11_USAGE_DEFAULT, 
		D3D11_BIND_VERTEX_BUFFER, 0U, 0U,
		0U
	};

	auto const vertex_data = D3D11_SUBRESOURCE_DATA
	{
		vertices.data(),
		0U, 0U
	};

	V_RETURN(device->CreateBuffer(&vertex_buffer_desc, &vertex_data,
		_vertex_buffer.GetAddressOf()));

	auto const indices = std::array<unsigned long, 6>
	{ { 0UL, 1UL, 2UL, 3UL, 4UL, 5UL } };

	auto const index_buffer_desc = D3D11_BUFFER_DESC
	{
		static_cast<UINT>(sizeof(unsigned long) * indices.size()),
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_INDEX_BUFFER, 0U, 0U,
		0U
	};

	auto const index_data = D3D11_SUBRESOURCE_DATA
	{
		indices.data(),
		0U, 0U
	};

	V_RETURN(device->CreateBuffer(&index_buffer_desc, &index_data,
		_index_buffer.GetAddressOf()));

	_world = XMMatrixIdentity();

	// Scale
	_world *= XMMatrixScaling(0.01f, 0.01f, 0.01f);

	// Rotation
	_world *= XMMatrixRotationY(0.0f);

	// Create position around the circumference of a 1 unit radius 
	auto const a =  XMConvertToRadians(static_cast<float>(rand() % 360));

	auto const r = rand() / static_cast<float>(RAND_MAX);

	auto const rx = (r * cos(a));
	auto const ry = (r * sin(a));

	// Translate
	_world *= XMMatrixTranslation(rx, 0.01f, ry);

	return hr;
}

void Particles::Update(void)
{
	--_time;

	_world *= XMMatrixTranslation(0.0f, (_time - 50) / 100000.0f, 0.0f);
}

void Particles::Render(ComPtr<ID3D11DeviceContext> const& context)
{
	// Render quad
	auto const stride = static_cast<UINT>(sizeof(vertex));
	auto const offset = 0U;
	context->IASetVertexBuffers(0U, 1U, _vertex_buffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(_index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0U);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}