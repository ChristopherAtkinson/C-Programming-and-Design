#pragma once
class Particles
{
private:
	struct vertex
	{
		XMFLOAT3 position; 
		XMFLOAT2 texture; 
		XMFLOAT4 color; 
	};

public:
	explicit Particles(ComPtr<ID3D11Device> const& device);
	~Particles();

	HRESULT Initialize(ComPtr<ID3D11Device> const& device);
	void Update(void);
	void Render(ComPtr<ID3D11DeviceContext> const& context);

	inline bool const Alive(void) const
	{ return _time != 0U; }

	XMMATRIX const& World(void) const
	{ return _world; }

private:
	ComPtr<ID3D11Buffer> _vertex_buffer;
	ComPtr<ID3D11Buffer> _index_buffer;
	int _time; 

	XMMATRIX _world; 
};