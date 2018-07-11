#include "pch.h"
#include "message.h"

HINSTANCE const message::_hInstance = GetModuleHandle(NULL);

message::message(std::wstring const& name) 
	: _handle(nullptr), _name(name)
{

}

message::~message(void)
{
	UnregisterClass(_name.c_str(), _hInstance);
}

void message::Update(void)
{
	MSG msg{};
	while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LRESULT CALLBACK message::Hook(int const uMsg, WPARAM const wParam, LPARAM const lParam)
{
	if (HCBT_CREATEWND == uMsg)
	{
		auto const data = reinterpret_cast<LPCBT_CREATEWND>(lParam);

		SetWindowLongPtr(reinterpret_cast<HWND>(wParam), GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(data->lpcs->lpCreateParams));
	}

	return CallNextHookEx(nullptr, uMsg, wParam, lParam);
}

LRESULT CALLBACK message::MsgProc(HWND const hWnd, UINT const uMsg, WPARAM const wParam, LPARAM const lParam)
{
	auto const pointer = GetWindowLongPtr(hWnd, GWLP_USERDATA);
	auto const target = reinterpret_cast<message*>(pointer); 

	if (TwEventWin(hWnd, uMsg, wParam, lParam))
		return 0;

	target->Message(uMsg, wParam, lParam);

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void message::Message(UINT const uMsg, WPARAM const wParam, LPARAM const lParam)
{

}