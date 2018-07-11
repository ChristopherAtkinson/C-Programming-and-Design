#pragma once

class message
{
public:
	explicit message(std::wstring const& name);
	message(message const&) {};
	message& operator=(message const&) = default;
	virtual ~message(void);

	virtual void Update(void);

	inline HWND const Handle(void) const
	{ return _handle.get(); }
	inline std::wstring const& Name(void) const 
	{ return _name; }
	inline HINSTANCE const& Instance(void) const 
	{ return _hInstance; }

	inline void Handle(HWND const& instance)
	{ _handle.reset(instance); }

protected: 
	virtual void Message(UINT const uMsg, WPARAM const wParam, LPARAM const lParam);

	static LRESULT CALLBACK Hook(int const uMsg, WPARAM const wParam, LPARAM const lParam);
	static LRESULT CALLBACK MsgProc(HWND const hWnd, UINT const uMsg, WPARAM const wParam, LPARAM const lParam);

private: 
	struct HWMD_DELETER
	{
		inline void operator()(HWND const p) const 
		{ DestroyWindow(p); }
	};

	std::unique_ptr<HWND__, HWMD_DELETER> _handle; 
	std::wstring _name = {};

	static HINSTANCE const _hInstance; 
};

