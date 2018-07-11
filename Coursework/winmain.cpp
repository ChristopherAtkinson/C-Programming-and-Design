#include "pch.h"
#include <Windows.h>

#include "Application.h"

int WINAPI wWinMain(_In_ HINSTANCE const hInstance, _In_opt_ HINSTANCE const hPrevInstance, _In_ LPWSTR const lpCmdLine, _In_ int const nCmdShow)
{
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	auto hr = static_cast<HRESULT>(0L);
	Application app{};
	V_RETURN(app.Initialize());

	while (app.Alive())
	{
		app.Update();
		app.Render();
	}

	return hr;
}