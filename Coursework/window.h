#pragma once
#include "message.h"
#include "input.h"
#include "Config.h"

class window : public message
{
public:
	window(void);
	~window(void);

	HRESULT Initialize(std::shared_ptr<Config> const& settings);
	
	inline void Alive(bool const& alive)
	{ _alive = alive; }
	inline bool const& Alive() const 
	{ return _alive; }
	inline std::shared_ptr<input> const& Input(void) const
	{ return _input; }

private:
	bool _alive = true; 
	void Message(UINT const uMsg, WPARAM const wParam, LPARAM const lParam) override;

	std::shared_ptr<input> _input;
};