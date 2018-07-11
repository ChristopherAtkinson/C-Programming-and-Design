#pragma once

class spotlight
{
public:
	spotlight(XMFLOAT3 const& position, float const& range, 
		XMFLOAT3 const& direction, float const& cone, XMFLOAT3 const& attenuation, 
		XMFLOAT4 const& ambient, XMFLOAT4 const& diffuse);
	~spotlight(void);

	inline XMFLOAT3 const& Position(void) const
	{ return _position; }
	inline float const& Range(void) const
	{ return _range; }
	inline XMFLOAT3 const& Direction(void) const
	{ return _direction; }
	inline float const& Cone(void) const
	{ return _cone; }
	inline XMFLOAT3 const& Attenuation(void) const
	{ return _attenuation; }
	inline XMFLOAT4 const& Ambient(void) const
	{ return _ambient; }
	inline XMFLOAT4 const& Diffuse(void) const
	{ return _diffuse; }

	inline void Set(XMFLOAT3 const& p, XMFLOAT3 const&d)
	{
		_position = p;
		_direction = d;
	}

private: 
	XMFLOAT3 _position;
	float _range; 
	XMFLOAT3 _direction; 
	float _cone; 
	XMFLOAT3 _attenuation; 
	float _pad;
	XMFLOAT4 _ambient; 
	XMFLOAT4 _diffuse;
};