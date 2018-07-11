#include "pch.h"
#include "window.h"

window::window(void) : message(L"ACW REALTIME GRAPGICS"), _alive(true)
{
	
}

HRESULT window::Initialize(std::shared_ptr<Config> const& settings)
{
	auto hr = static_cast<HRESULT>(0L);

	auto const window_class = WNDCLASS
	{
		CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
		message::MsgProc,
		0, 0,
		Instance(),
		nullptr, nullptr, nullptr,
		nullptr, Name().c_str()
	};

	V_RETURN(!RegisterClass(&window_class));

	auto const hook = SetWindowsHookEx(WH_CBT, message::Hook, nullptr, GetCurrentThreadId());

	auto const width = settings->Read<unsigned>(L"general", L"width");
	auto const height = settings->Read<unsigned>(L"general", L"height");

	Handle(CreateWindow
	(
		window_class.lpszClassName, window_class.lpszClassName,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
		0x80000000, 0x80000000,
		width, height,
		nullptr, nullptr,
		Instance(),
		this
	));

	UnhookWindowsHookEx(hook);

	if (!Handle())
		return static_cast<HRESULT>(0x80004005);

	_input = std::make_shared<input>();

	return hr; 
}

window::~window(void)
{

}

void window::Message(UINT const uMsg, WPARAM const wParam, LPARAM const lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		_alive = false;
		return; 
	case WM_KEYDOWN:
		_input->Key_Down(wParam);
		break;
	case WM_KEYUP:
		_input->Key_Up(wParam);
		break;
	}
}