#include "pch.h"
#include "Application.h"

Application::Application(void) 
	: _window(std::make_unique<window>()), _directX3D(std::make_unique<Direct>()), 
	_camera_index(0U), _bar(nullptr), _animated(false), _AntTweakBar_visable(false)
{
	auto const settings = std::make_shared<Config>(L"./config.ini");
	_window->Initialize(settings);
	_directX3D->Initialize(_window->Handle(), settings);
}

Application::~Application(void)
{

}

HRESULT Application::Initialize(void)
{
	HRESULT hr = static_cast<HRESULT>(0L);
	auto const settings = std::make_shared<Config>(L"./config.ini");

	auto const width = settings->Read<unsigned>(L"general", L"width");
	auto const height = settings->Read<unsigned>(L"general", L"height");

	TwInit(TW_DIRECT3D11, _directX3D->Device().Get());
	TwWindowSize(width, height);
	_bar = TwNewBar("AntTweakBar");
	TwDefine("AntTweakBar visible=false");

	// Shaders
	_texture_shader = std::make_shared<texture_shader>();
	_texture_shader->initialize(_directX3D->Device(), L"texture_shader.fx");

	_particle_shader = std::make_shared<Particle_Shader>();
	_particle_shader->initialize(_directX3D->Device(), L"Particles.fx");

	// Names of camera in config file 
	auto const camera_names =
	{
		L"camera0",
		L"camera1",
		L"camera2",
		L"camera3"
	};

	// Load each camera into vector list 
	for (auto const name : camera_names)
	{
		auto const position = XMFLOAT3
		{
			settings->Read<float>(name, L"positionx"),
			settings->Read<float>(name, L"positiony"),
			settings->Read<float>(name, L"positionz")
		};

		auto const lookat = XMFLOAT3
		{
			settings->Read<float>(name, L"lookatx"),
			settings->Read<float>(name, L"lookaty"),
			settings->Read<float>(name, L"lookatz")
		};

		auto const up = XMFLOAT3
		{
			settings->Read<float>(name, L"upx"),
			settings->Read<float>(name, L"upy"),
			settings->Read<float>(name, L"upz")
		};

		auto const rotation = XMFLOAT3
		{
			settings->Read<float>(name, L"rotationx"),
			settings->Read<float>(name, L"rotationy"),
			settings->Read<float>(name, L"rotationz")
		};

		_cameras.push_back(std::make_shared<camera>(position, lookat, up, rotation));
	}

	// AntTweakBar
	auto bar_camera = std::array<TwStructMember, 5U>
	{{
		{ "View", TW_TYPE_QUAT4F, offsetof(camera, View()), "" },
		{ "Position", TW_TYPE_DIR3F, offsetof(camera, Position()), "" },
		{ "Lookat", TW_TYPE_DIR3F, offsetof(camera, Lookat()), "" },
		{ "Up", TW_TYPE_DIR3F, offsetof(camera, Up()), "" },
		{ "Rotation", TW_TYPE_DIR3F, offsetof(camera, Rotation()), "" }
	}};

	TwType const camera_type = TwDefineStruct("Camera", 
		bar_camera.data(), static_cast<unsigned>(bar_camera.size()), 
		sizeof(camera), nullptr, nullptr);

	for (auto i = 0U; i < _cameras.size(); ++i)
	{
		auto const name = std::string{ "Camera" } +std::to_string(i);
		TwAddVarRW(_bar, name.c_str(), camera_type, static_cast<void*>(_cameras[i].get()), nullptr);
	}

	// Spotlights
	auto const spotlight_quantity = settings->Read<unsigned>(L"spotlight", L"quantity");
	for (auto i = 0U; i < spotlight_quantity; ++i)
	{
		auto const key = std::wstring{ L"spotlight" } + std::to_wstring(i);

		auto const spotlight_position = XMFLOAT3
		{
			settings->Read<float>(key, L"positionx"),
			settings->Read<float>(key, L"positiony"),
			settings->Read<float>(key, L"positionz")
		};

		auto const spotlight_range = settings->Read<float>(key, L"range");

		auto const spotlight_direction = XMFLOAT3
		{
			settings->Read<float>(key, L"directionx"),
			settings->Read<float>(key, L"directiony"),
			settings->Read<float>(key, L"directionz")
		};

		auto const spotlight_cone = settings->Read<float>(key, L"cone");

		auto const spotlight_attenuation = XMFLOAT3
		{
			settings->Read<float>(key, L"attenuationx"),
			settings->Read<float>(key, L"attenuationy"),
			settings->Read<float>(key, L"attenuationz")
		};

		auto const spotlight_ambient = XMFLOAT4
		{
			settings->Read<float>(key, L"ambientr"),
			settings->Read<float>(key, L"ambientg"),
			settings->Read<float>(key, L"ambientb"),
			settings->Read<float>(key, L"ambienta")
		};

		auto const spotlight_diffuse = XMFLOAT4
		{
			settings->Read<float>(key, L"diffuser"),
			settings->Read<float>(key, L"diffuseg"),
			settings->Read<float>(key, L"diffuseb"),
			settings->Read<float>(key, L"diffusea")
		};

		_spotlights.push_back(std::make_shared<spotlight>(
			spotlight_position, spotlight_range,
			spotlight_direction, spotlight_cone,
			spotlight_attenuation,
			spotlight_ambient, spotlight_diffuse
			));
	}

	// AntTweakBar
	auto bar_spotlight = std::array<TwStructMember, 7U>
	{{
		{ "Position", TW_TYPE_DIR3F, offsetof(spotlight, Position()), "" },
		{ "Range", TW_TYPE_FLOAT, offsetof(spotlight, Range()), "" },
		{ "Direction", TW_TYPE_DIR3F, offsetof(spotlight, Direction()), "" },
		{ "Cone", TW_TYPE_FLOAT, offsetof(spotlight, Cone()), "" },
		{ "Attenuation", TW_TYPE_DIR3F, offsetof(spotlight, Attenuation()), "" },
		{ "Ambient", TW_TYPE_COLOR4F, offsetof(spotlight, Ambient()), "" },
		{ "Diffuse", TW_TYPE_COLOR4F, offsetof(spotlight, Diffuse()), "" }
	}};

	TwType const spotlight_type = TwDefineStruct("SpotLight", 
		bar_spotlight.data(), static_cast<unsigned>(bar_spotlight.size()),
		sizeof(spotlight), nullptr, nullptr);

	for (auto i = 0U; i < _spotlights.size(); ++i)
	{
		auto const name = std::string{ "Spotlight" } +std::to_string(i);
		TwAddVarRW(_bar, name.c_str(), spotlight_type, static_cast<void*>(_spotlights[i].get()), nullptr);
	}

	// Load Assests
	auto const mesh_quantity = settings->Read<unsigned>(L"mesh", L"quantity");

	for (auto i = 0U; i < mesh_quantity; ++i)
	{
		auto const object = std::make_shared<Object>();
		object->world = XMMatrixIdentity();

		object->mesh = std::make_shared<CDXUTSDKMesh>();
		auto key = std::wstring{ L"object" } +std::to_wstring(i);
		auto const path = settings->Read<std::wstring>(key, L"path");
		V_RETURN(object->mesh->Create(_directX3D->Device().Get(),
			path.c_str()));

		object->translation = XMFLOAT3{
			settings->Read<float>(key, L"translationx"),
			settings->Read<float>(key, L"translationy"),
			settings->Read<float>(key, L"translationz")
		};

		object->rotation = XMFLOAT3{
			XMConvertToRadians(settings->Read<float>(key, L"rotationx")),
			XMConvertToRadians(settings->Read<float>(key, L"rotationy")),
			XMConvertToRadians(settings->Read<float>(key, L"rotationz"))
		};

		object->scale = XMFLOAT3{
			XMConvertToRadians(settings->Read<float>(key, L"scalex")),
			XMConvertToRadians(settings->Read<float>(key, L"scaley")),
			XMConvertToRadians(settings->Read<float>(key, L"scalez"))
		};

		_objects.push_back(object);
	}

	// AntTweakBar
	auto bar_mesh = std::array<TwStructMember, 3U>
	{{
		{ "Translation", TW_TYPE_DIR3F, offsetof(Object, translation), "" },
		{ "Rotation", TW_TYPE_DIR3F, offsetof(Object, rotation), "" },
		{ "Scale", TW_TYPE_DIR3F, offsetof(Object, scale), "" }
	}};

	TwType const mesh_type = TwDefineStruct("Mesh",
		bar_mesh.data(), static_cast<unsigned>(bar_mesh.size()),
		sizeof(Object), nullptr, nullptr);

	for (auto i = 0U; i < _objects.size(); ++i)
	{
		auto const name = std::string{ "Mesh" } +std::to_string(i);
		TwAddVarRW(_bar, name.c_str(), mesh_type, static_cast<void*>(_objects[i].get()), nullptr);
	}

	_emitter = std::make_shared<Emitter>(XMFLOAT3{ 0.0f, 0.0f, 0.0f });
	_emitter->Initialize(_directX3D->Device(), L"particle.dds");

	// AntTweakBar
	auto bar_emitter = std::array<TwStructMember, 1U>
	{{
		{ "Position", TW_TYPE_DIR3F, offsetof(Emitter, Position()), "" },
	}};

	TwType const Emitter_type = TwDefineStruct("Emitter",
		bar_emitter.data(), static_cast<unsigned>(bar_emitter.size()),
		sizeof(Emitter), nullptr, nullptr);

	TwAddVarRW(_bar, "Emitter", Emitter_type, static_cast<void*>(_emitter.get()), nullptr);

	auto const index = settings->Read<unsigned>(L"animation", L"model");
	auto const key_frames = settings->Read<unsigned>(L"animation", L"key");
	_objects[index]->key_number = 0U;
	for (auto i = 0U; i < key_frames; ++i)
	{
		auto const key = std::wstring{ L"pos" + std::to_wstring(i) };
		auto const target = XMFLOAT3{ settings->Read<float>(L"animation", key + L"x"),
			settings->Read<float>(L"animation", key + L"y") ,
			settings->Read<float>(L"animation", key + L"z") };

		_objects[index]->key_frames.push_back(target);
	}

	return hr;
}

void Application::Update(void)
{
	_window->Update();

	// Close
	if (_window->Input()->Key_Pressed(0x1B)) // ESC
		_window->Alive(false);

	// Reset
	if (_window->Input()->Key_Pressed(0x52)) // R
	{
		_animated = false;
		// Reset delay 
		_delay = static_cast<std::chrono::milliseconds>(15);
		// Reset render state
		if (render_state == 1) { _directX3D->Toggle_Raster_State(); }
		render_state = 0;
		textured = true;

		// Clear objects
		_cameras.clear();
		_objects.clear();
		_spotlights.clear();

		// Reset default camera
		_camera_index = 0;

		//Reset AntTweakBar
		TwRemoveAllVars(_bar);
		TwDeleteAllBars();
		TwTerminate();

		// Reload Scene from config
		Initialize();
	}

	// Panning
	if (_window->Input()->Key_Pressed(0x57)) // W
		_cameras[_camera_index]->Move({.0f, .0f, .1f});

	if (_window->Input()->Key_Pressed(0x41)) // A
		_cameras[_camera_index]->Move({-.1f, .0f, .0f});

	if (_window->Input()->Key_Pressed(0x53)) // S
		_cameras[_camera_index]->Move({.0f, .0f, -.1f});

	if (_window->Input()->Key_Pressed(0x44)) // D
		_cameras[_camera_index]->Move({.1f, .0f, .0f});

	if (_window->Input()->Key_Pressed(VK_PRIOR)) // PAGE UP
		_cameras[_camera_index]->Move({.0f, .1f, .0f});

	if (_window->Input()->Key_Pressed(VK_NEXT)) // PAGE DOWN
		_cameras[_camera_index]->Move({.0f, -.1f, .0f});

	// Rotation
	if (_window->Input()->Key_Pressed(VK_UP) || _window->Input()->Key_Pressed(0x49)) // UP or I
		_cameras[_camera_index]->Rotate({0.5f, .0f, .0f});

	if (_window->Input()->Key_Pressed(VK_LEFT) || _window->Input()->Key_Pressed(0x4A)) // LEFT or J
		_cameras[_camera_index]->Rotate({.0f, 0.5f, .0f});

	if (_window->Input()->Key_Pressed(VK_DOWN) || _window->Input()->Key_Pressed(0x4B)) // DOWN or K
		_cameras[_camera_index]->Rotate({-0.5f, .0f, .0f});

	if (_window->Input()->Key_Pressed(VK_RIGHT) || _window->Input()->Key_Pressed(0x4C)) // RIGHT or L
		_cameras[_camera_index]->Rotate({.0f, -0.5f, .0f});

	// Function keys
	if (_window->Input()->Key_Pressed(VK_F1)) // F1
		_camera_index = 0;

	if (_window->Input()->Key_Pressed(VK_F2)) // F2
		_camera_index = 1;

	if (_window->Input()->Key_Pressed(VK_F3)) // F3
		_camera_index = 2;

	if (_window->Input()->Key_Pressed(VK_F4)) // F4
		_camera_index = 3;

	if (_window->Input()->Key_Pressed(VK_F5)) // F5
	{
		switch (render_state)
		{
		case 0:
			// Wireframe state
			_directX3D->Toggle_Raster_State();
			// Update itterator
			render_state++;
			// Delay next input
			Sleep(100);
			break;
		case 1:
			// Solid state
			_directX3D->Toggle_Raster_State();
			// diffuse non-textured
			textured = false;
			// Update itterator
			render_state++;
			// Delay next input
			Sleep(100);
			break;
		case 2:
			// diffused textured
			textured = true;
			// Update itterator
			render_state = 0;
			// Delay next input
			Sleep(100);
			break;
		}
	}

	if (_window->Input()->Key_Pressed(0x09)) // TAB
	{
		if (_AntTweakBar_visable)
		{
			TwDefine("AntTweakBar visible=false");
			_AntTweakBar_visable = false;
			Sleep(100);
		}
		else
		{
			TwDefine("AntTweakBar visible=true");
			_AntTweakBar_visable = true;
			Sleep(100);
		}
	}

	if (_window->Input()->Key_Pressed(VK_F12) || _window->Input()->Key_Pressed(VK_F6)) // f12
	{ _animated = true;  }

	if (_animated)
	{
		_emitter->Emit(_directX3D->Device(), 10);

		for (auto const& i : _objects)
		{
			if (i->key_frames.size() > 0)
			{
				auto dir = XMFLOAT3{
					i->key_frames[i->key_number].x - i->translation.x,
					i->key_frames[i->key_number].y - i->translation.y,
					i->key_frames[i->key_number].z - i->translation.z
				};

				auto const mag = sqrtf((dir.x*dir.x) + (dir.y*dir.y) + (dir.z*dir.z));

				dir.x = XMConvertToRadians(dir.x / mag);
				dir.y = XMConvertToRadians(dir.y / mag);
				dir.z = XMConvertToRadians(dir.z / mag);

				/*i->rotation.x += dir.x * 0.1f;
				i->rotation.y += dir.y * 0.1f;
				i->rotation.z += dir.z * 0.1f;*/

				XMFLOAT3 c_center = XMFLOAT3{};
				XMStoreFloat3(&c_center, i->mesh->GetMeshBBoxCenter(0));
				XMFLOAT3 c_extents = XMFLOAT3{};
				XMStoreFloat3(&c_extents, i->mesh->GetMeshBBoxExtents(0));
				BoundingBox const box1 = { c_center, c_extents };

				for (auto const i2 : _objects)
				{
					XMFLOAT3 center = XMFLOAT3{};
					XMStoreFloat3(&center, i2->mesh->GetMeshBBoxCenter(0));
					XMFLOAT3 extents = XMFLOAT3{};
					XMStoreFloat3(&extents, i2->mesh->GetMeshBBoxExtents(0));
					BoundingBox const box2 = { center, extents };

					if ((box1.Center.x - box2.Center.x) < 0.00001f && (box1.Center.y - box2.Center.y) < 0.00001f && (box1.Center.z - box2.Center.z) < 0.00001f)
					{
						continue;
					}

					if (!box1.Intersects(box2))
					{
						i->translation.x += dir.x * 0.1f;
						i->translation.y += dir.y * 0.1f;
						i->translation.z += dir.z * 0.1f;
					}
				}

				/*i->translation.x += dir.x * 0.1f;
				i->translation.y += dir.y * 0.1f;
				i->translation.z += dir.z * 0.1f;*/
				

				auto const cam_dir = XMFLOAT3{
					i->translation.x - _cameras[2]->Position().x,
					i->translation.y -_cameras[2]->Position().y,
					i->translation.z - _cameras[2]->Position().z
				};

				_cameras[2]->Lookat(cam_dir);

				if (mag < 0.01f && i->key_number < i->key_frames.size() -1)
				{
					i->key_number++;
				}
			}
		}
	}

	_emitter->Update();

	for (auto const& object : _objects)
	{
		object->world = 
			XMMatrixTranslation(object->translation.x, object->translation.y, object->translation.z) *
			XMMatrixRotationX(object->rotation.x) *
			XMMatrixRotationY(object->rotation.y) *
			XMMatrixRotationZ(object->rotation.z) *
			XMMatrixScaling(object->scale.x, object->scale.y, object->scale.z);
	}

	// Global delay update 
	if (_window->Input()->Key_Pressed(0x54)) // T
	{
		if (_window->Input()->Key_Pressed(0x10) && 
			_delay > std::chrono::microseconds(100)) // LSHIFT
		{
			// Increase global timer
			_delay -= static_cast<std::chrono::microseconds>(100);
		}
		else
		{
			// Decrease global timer
			_delay += static_cast<std::chrono::microseconds>(100);
		}
	}
}

void Application::Render(void)
{	
	auto const time = std::chrono::high_resolution_clock::now();
	auto difference = time - _time;
	while (difference < _delay)
	{
		_directX3D->Start(1.0f, 1.0f, 1.0f, 1.0f);

		_cameras[_camera_index]->Render();

		for (auto object : _objects)
		{
			_texture_shader->set_shader_resource(_directX3D->Context(), object->world, _cameras[_camera_index]->View(), _directX3D->Projection());
			_texture_shader->set_shader_resource(_directX3D->Context(), _cameras[_camera_index]);
			_texture_shader->set_shader_resource(_directX3D->Context(), _spotlights);
			_texture_shader->set_shader_resource(_directX3D->Context(), _animated, static_cast<float>(difference.count()));
			_texture_shader->render(_directX3D->Context(), object->mesh, textured);
		}

		_emitter->Render(_directX3D->Context(), _particle_shader, _cameras[_camera_index], _directX3D->Projection(), textured);

		_directX3D->End();

		difference = std::chrono::high_resolution_clock::now() - _time;
	}

	_time = time;
}