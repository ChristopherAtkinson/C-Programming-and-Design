#include "pch.h"
#include "spotlight.h"

spotlight::spotlight(XMFLOAT3 const& position, float const& range,
	XMFLOAT3 const& direction, float const& cone, XMFLOAT3 const& attenuation,
	XMFLOAT4 const& ambient, XMFLOAT4 const& diffuse)
	: _position(position), _range(range),
	_direction(direction), _cone(cone), _attenuation(attenuation),
	_pad(0.0f), _ambient(ambient), _diffuse(diffuse)
{

}

spotlight::~spotlight(void)
{

}