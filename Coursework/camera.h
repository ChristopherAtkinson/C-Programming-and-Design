#pragma once
class camera
{
public:
	camera(XMFLOAT3 const& position, XMFLOAT3 const& look_at, XMFLOAT3 const& up, XMFLOAT3 const& rotation);
	~camera();

	void Rotate(XMFLOAT3 const& delta);
	void Move(XMFLOAT3 const& delta);
	void Render(void);

	inline void Rotation(XMFLOAT3 const& rotation)
	{ _rotation = rotation; }

	inline XMMATRIX const& View(void) const
	{ return _view; }
	inline XMFLOAT3 const& Position(void) const
	{ return _position; }
	inline XMFLOAT3 const& Lookat(void) const
	{ return _look_at; }
	inline XMFLOAT3 const& Up(void) const
	{ return _up; }
	inline XMFLOAT3 const& Rotation(void) const
	{ return _rotation; }
	
	inline void Lookat(XMFLOAT3 const& lookat)
	{ _look_at = lookat; }

private: 
	XMMATRIX _view;
	XMFLOAT3 _position, _look_at, _up;
	XMFLOAT3 _rotation;
};