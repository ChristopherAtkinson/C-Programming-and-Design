#include "pch.h"
#include "Particle_Shader.h"

Particle_Shader::Particle_Shader()
{

}

Particle_Shader::~Particle_Shader()
{

}

HRESULT Particle_Shader::initialize(ComPtr<ID3D11Device> const& device, LPCWSTR const& filename)
{
	auto hr = static_cast<HRESULT>(0L);

	auto shader_flags = static_cast<DWORD>(D3DCOMPILE_ENABLE_STRICTNESS);
#ifdef _DEBUG
	shader_flags |= D3DCOMPILE_DEBUG;
	shader_flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	// Compile vertex shader
	ComPtr<ID3DBlob> vertex_blob = nullptr;
	V_RETURN(DXUTCompileFromFile(filename, nullptr, "VS", "vs_4_0", shader_flags, 0, vertex_blob.GetAddressOf()));

	// Create the vertex shader
	V_RETURN(device->CreateVertexShader(vertex_blob->GetBufferPointer(), vertex_blob->GetBufferSize(), nullptr, _vertex_shader.GetAddressOf()));

	// Compile pixel shader
	ComPtr<ID3DBlob> pixel_blob = nullptr;
	V_RETURN(DXUTCompileFromFile(filename, nullptr, "PS", "ps_4_0", shader_flags, 0, pixel_blob.GetAddressOf()));

	// Create the pixel shader
	V_RETURN(device->CreatePixelShader(pixel_blob->GetBufferPointer(), pixel_blob->GetBufferSize(), nullptr, _pixel_shader.GetAddressOf()));

	// Define layout
	auto const layout = std::array<D3D11_INPUT_ELEMENT_DESC, 3U>
	{{
		{ "POSITION", 0U, DXGI_FORMAT_R32G32B32_FLOAT, 0U, 0U, D3D11_INPUT_PER_VERTEX_DATA, 0U },
		{ "TEXCOORD", 0U, DXGI_FORMAT_R32G32_FLOAT, 0U, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0U },
		{ "COLOR", 0U, DXGI_FORMAT_R32G32B32A32_FLOAT, 0U, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0U }
	}};

	// Create input layout
	V_RETURN(device->CreateInputLayout(layout.data(), static_cast<UINT>(layout.size()), vertex_blob->GetBufferPointer(), vertex_blob->GetBufferSize(), _input_layout.GetAddressOf()));

	// Assign buffers
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = static_cast<UINT>(sizeof(SCENE));
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	V_RETURN(device->CreateBuffer(&bd, nullptr, _scene.GetAddressOf()));

	auto const sampler_description = D3D11_SAMPLER_DESC
	{
		D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP,
		0.0f, 1U, D3D11_COMPARISON_NEVER,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, D3D11_FLOAT32_MAX
	};

	V_RETURN(device->CreateSamplerState(&sampler_description, _sampler_state.GetAddressOf()));

	return hr;
}

void Particle_Shader::set_shader_resource(ComPtr<ID3D11DeviceContext> const& context, XMMATRIX const& world, XMMATRIX const& view, XMMATRIX const& projection) const
{
	auto hr = static_cast<HRESULT>(0L);
	auto mapped_resource = D3D11_MAPPED_SUBRESOURCE{};
	V(context->Map(_scene.Get(), 0U, D3D11_MAP_WRITE_DISCARD, 0U, &mapped_resource));
	auto const changes_every_frame_buffer = reinterpret_cast<SCENE*>(mapped_resource.pData);
	changes_every_frame_buffer->world = XMMatrixTranspose(world);
	changes_every_frame_buffer->view = XMMatrixTranspose(view);
	changes_every_frame_buffer->projection = XMMatrixTranspose(projection);
	context->Unmap(_scene.Get(), 0U);
}

void Particle_Shader::Render(ComPtr<ID3D11DeviceContext> const& context)
{
	context->VSSetShader(_vertex_shader.Get(), nullptr, 0U );
	context->PSSetShader( _pixel_shader.Get(), nullptr, 0U );

	context->VSSetConstantBuffers( 0U, 1U, _scene.GetAddressOf());
	context->PSSetSamplers( 0U, 1U, _sampler_state.GetAddressOf() );

	context->Draw(6U, 0U);
}