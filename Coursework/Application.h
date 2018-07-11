#pragma once
#include "input.h"
#include "window.h"
#include "Direct.h"
#include "config.h"
#include "camera.h"
#include "texture_shader.h"
#include "Particle_Shader.h"
#include "Emitter.h"

class Application
{
private: 
	struct Object
	{
		XMMATRIX world; 
		std::shared_ptr<CDXUTSDKMesh> mesh; 
		XMFLOAT3 translation; 
		XMFLOAT3 rotation; 
		XMFLOAT3 scale; 
		unsigned key_number;
		std::vector<XMFLOAT3> key_frames;
	};
	
public:
	Application(void);
	Application(Application const&) 
		: _camera_index(0U), _bar(nullptr), 
		_animated(false), _AntTweakBar_visable(false) {};
	Application& operator=(Application const&) = default;
	~Application(void);

	HRESULT Initialize(void);
	void Update(void); 
	void Render(void);

	inline bool const Alive(void) const 
	{ return _window->Alive(); }

private: 
	// Window
	std::unique_ptr<window> _window;
	std::unique_ptr<Direct> _directX3D;

	// Shaders
	std::shared_ptr<texture_shader> _texture_shader;
	std::shared_ptr<Particle_Shader> _particle_shader;

	// Camera
	unsigned _camera_index; 
	std::vector<std::shared_ptr<camera>> _cameras;
	
	// SDK meshes
	std::vector<std::shared_ptr<Object>> _objects;

	// Lights
	std::vector<std::shared_ptr<spotlight>> _spotlights;
	
	// Particles
	std::shared_ptr<Emitter> _emitter;

	// Curretn render state
	unsigned render_state = 0;
	bool textured = true;

	// Timer 
	using high_resolution_clock = std::chrono::high_resolution_clock;
	high_resolution_clock::time_point _time = high_resolution_clock::now();
	std::chrono::microseconds _delay = static_cast<std::chrono::milliseconds>(15);

	TwBar* _bar;
	bool _animated;
	bool _AntTweakBar_visable;
};