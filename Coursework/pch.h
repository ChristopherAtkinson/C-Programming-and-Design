#pragma once

// Windows 
#define NOMINMAX
#define WINDOW_LEAN_AND_MEAN

#include <Windows.h>
#include <wrl/client.h>

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

// DirectX
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
using namespace DirectX;

// DXUT
#include <DXUT.h>
#include <SDKmesh.h>
#include <SDKmisc.h>
#include <DDSTextureLoader.h>

// AntTweakBar
#include "AntTweakBar.h"

// STL
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <array>
#include <chrono>