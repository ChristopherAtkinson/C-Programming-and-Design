#include "pch.h"
#include "camera.h"

camera::camera(XMFLOAT3 const& position, XMFLOAT3 const& look_at, XMFLOAT3 const& up, XMFLOAT3 const& rotation)
	: _position(position), _look_at(look_at), _up(up), _rotation(rotation)
{

}

camera::~camera()
{

}

void camera::Rotate(XMFLOAT3 const& delta)
{
	_rotation.x += delta.x;
	_rotation.y += delta.y;
	_rotation.z += delta.z;
}

void camera::Move(XMFLOAT3 const& delta)
{
	auto const pitch = XMConvertToRadians(_rotation.x);
	auto const yaw = XMConvertToRadians(_rotation.y);
	auto const roll = XMConvertToRadians(_rotation.z);
	auto const rotation_matrix = XMMatrixRotationRollPitchYaw(-pitch, -yaw, -roll);

	auto vec = XMLoadFloat3(&delta);
	vec = XMVector3TransformCoord(vec, rotation_matrix);
	auto result = XMFLOAT3{};
	XMStoreFloat3(&result, vec);

	_position.x += result.x;
	_position.y += result.y;
	_position.z += result.z;
}

void camera::Render(void)
{
	auto const pitch = XMConvertToRadians(_rotation.x);
	auto const yaw = XMConvertToRadians(_rotation.y);
	auto const roll = XMConvertToRadians(_rotation.z);
	auto const rotation_matrix = XMMatrixRotationRollPitchYaw(-pitch, -yaw, -roll);

	auto at = XMLoadFloat3(&_look_at);
	at = XMVector3TransformCoord(at, rotation_matrix);

	auto up = XMLoadFloat3(&_up);
	up = XMVector3TransformCoord(up, rotation_matrix);

	auto const position = XMLoadFloat3(&_position);
	auto const direction = XMVectorAdd(position, at);

	_view = XMMatrixLookAtLH(position, direction, up);
}