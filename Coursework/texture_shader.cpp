#include "pch.h"
#include "texture_shader.h"

texture_shader::texture_shader(void)
{

}

texture_shader::~texture_shader(void)
{

}

HRESULT texture_shader::initialize(ComPtr<ID3D11Device> const& device, LPCWSTR const& filename)
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
		{ "NORMAL", 0U, DXGI_FORMAT_R32G32B32_FLOAT, 0U, 12U, D3D11_INPUT_PER_VERTEX_DATA, 0U },
		{ "TEXCOORD", 0U, DXGI_FORMAT_R32G32_FLOAT, 0U, 24U, D3D11_INPUT_PER_VERTEX_DATA, 0U },
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

	bd.ByteWidth = static_cast<UINT>(sizeof(CAMERA));
	V_RETURN(device->CreateBuffer(&bd, nullptr, _camera.GetAddressOf()));

	bd.ByteWidth = static_cast<UINT>(sizeof(LIGHT));
	V_RETURN(device->CreateBuffer(&bd, nullptr, _light.GetAddressOf()));

	bd.ByteWidth = static_cast<UINT>(sizeof(MATERIAL));
	V_RETURN(device->CreateBuffer(&bd, nullptr, _material.GetAddressOf()));

	bd.ByteWidth = static_cast<UINT>(sizeof(ANIMATION));
	V_RETURN(device->CreateBuffer(&bd, nullptr, _animation.GetAddressOf()));

	auto const sampler_description = D3D11_SAMPLER_DESC
	{
		D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP,
		0.0f, 1U, D3D11_COMPARISON_ALWAYS,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, D3D11_FLOAT32_MAX
	};

	V_RETURN(device->CreateSamplerState(&sampler_description, _sampler_state.GetAddressOf()));

	return hr;
}

void texture_shader::set_shader_resource(ComPtr<ID3D11DeviceContext> const& context, XMMATRIX const& world, XMMATRIX const& view, XMMATRIX const& projection) const
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

void texture_shader::set_shader_resource(ComPtr<ID3D11DeviceContext> const& context, std::shared_ptr<camera> const& camera) const
{
	auto hr = static_cast<HRESULT>(0L);
	auto mapped_resource = D3D11_MAPPED_SUBRESOURCE{};
	V(context->Map(_camera.Get(), 0U, D3D11_MAP_WRITE_DISCARD, 0U, &mapped_resource));
	auto const camera_ps_buffer = reinterpret_cast<CAMERA*>(mapped_resource.pData);
	camera_ps_buffer->position = XMLoadFloat3(&camera->Position());
	context->Unmap(_camera.Get(), 0U);
}

void texture_shader::set_shader_resource(ComPtr<ID3D11DeviceContext> const& context, std::vector<std::shared_ptr<spotlight>> const& spot_lights) const
{ 
	auto hr = static_cast<HRESULT>(0L);
	auto mapped_resource = D3D11_MAPPED_SUBRESOURCE{};
	V(context->Map(_light.Get(), 0U, D3D11_MAP_WRITE_DISCARD, 0U, &mapped_resource));
	auto const light_ps_buffer = reinterpret_cast<LIGHT*>(mapped_resource.pData);
	
	for (auto i = 0U; i < spot_lights.size(); i++)
	{
		auto const light = spotlight{
			spot_lights[i]->Position(), 
			spot_lights[i]->Range(),
			spot_lights[i]->Direction(),
			spot_lights[i]->Cone(),
			spot_lights[i]->Attenuation(),
			spot_lights[i]->Ambient(),
			spot_lights[i]->Diffuse()
		};

		light_ps_buffer->lights[i] = light;
	}

	light_ps_buffer->quantity = static_cast<int>(spot_lights.size());

	context->Unmap(_light.Get(), 0U);
}

void texture_shader::set_shader_resource(ComPtr<ID3D11DeviceContext> const& context, bool const& animation, float const& time) const
{
	auto hr = static_cast<HRESULT>(0L);
	auto mapped_resource = D3D11_MAPPED_SUBRESOURCE{};
	V(context->Map(_animation.Get(), 0U, D3D11_MAP_WRITE_DISCARD, 0U, &mapped_resource));
	auto const animation_buffer = reinterpret_cast<ANIMATION*>(mapped_resource.pData);
	animation_buffer->aniamtion = animation;
	animation_buffer->time = time;
	context->Unmap(_animation.Get(), 0U);
}

void texture_shader::render(ComPtr<ID3D11DeviceContext> const& context, std::shared_ptr<CDXUTSDKMesh>& mesh, bool const& textured)
{
	context->IASetInputLayout(_input_layout.Get());

	auto const buffer = mesh->GetVB11(0U, 0U);
	auto const strides = static_cast<UINT>(mesh->GetVertexStride( 0, 0 ));
	auto const offset = static_cast<UINT>(0U);
	context->IASetVertexBuffers(0U, 1U, &buffer, &strides, &offset );
	context->IASetIndexBuffer(mesh->GetIB11(0U), mesh->GetIBFormat11(0U), 0U);
	context->VSSetShader(_vertex_shader.Get(), nullptr, 0U);
	context->VSSetConstantBuffers(0U, 1U, _scene.GetAddressOf());
	context->PSSetShader( _pixel_shader.Get(), nullptr, 0 );
	context->PSSetSamplers(0U, 1U, _sampler_state.GetAddressOf());
	context->VSSetConstantBuffers(1U, 1U, _camera.GetAddressOf());
	context->PSSetConstantBuffers(1U, 1U, _camera.GetAddressOf());
	context->VSSetConstantBuffers(2U, 1U, _light.GetAddressOf());
	context->PSSetConstantBuffers(2U, 1U, _light.GetAddressOf());
	context->VSSetConstantBuffers(4U, 1U, _animation.GetAddressOf());

	for( auto i = 0U; i < mesh->GetNumSubsets(0U); ++i)
	{
		auto const pSubset = mesh->GetSubset(0U, i);
		auto const PrimType = mesh->GetPrimitiveType11(static_cast<SDKMESH_PRIMITIVE_TYPE>(pSubset->PrimitiveType));
		context->IASetPrimitiveTopology( PrimType );

		if (textured)
		{
			auto const texture = mesh->GetMaterial(pSubset->MaterialID)->pDiffuseRV11;
			context->PSSetShaderResources(0U, 1U, &texture);
		}

		auto hr = static_cast<HRESULT>(0L);
		auto mapped_resource = D3D11_MAPPED_SUBRESOURCE{};
		V(context->Map(_material.Get(), 0U, D3D11_MAP_WRITE_DISCARD, 0U, &mapped_resource));
		auto const material_buffer = reinterpret_cast<MATERIAL*>(mapped_resource.pData);
		material_buffer->ambient = mesh->GetMaterial( pSubset->MaterialID )->Ambient;
		material_buffer->diffuse = mesh->GetMaterial( pSubset->MaterialID )->Diffuse;
		material_buffer->specular = mesh->GetMaterial( pSubset->MaterialID )->Specular;
		material_buffer->power = mesh->GetMaterial( pSubset->MaterialID )->Power;
		context->Unmap(_material.Get(), 0U);

		context->PSSetConstantBuffers( 3U, 1U, _material.GetAddressOf());

		context->DrawIndexed( static_cast<UINT>(pSubset->IndexCount), 0U, 
			static_cast<UINT>(pSubset->VertexStart));
	}
}