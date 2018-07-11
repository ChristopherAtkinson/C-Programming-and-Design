#pragma once
#include "message.h"

class input
{
public:
	input(void);
	~input(void);

	inline void Key_Down(LPARAM  const& key)
	{ _keys[key] = true; }
	inline void Key_Up(LPARAM  const& key)
	{ _keys[key] = false; }
	inline bool const& Key_Pressed(LPARAM  const& key) const 
	{ return _keys[key]; }

private:
	std::array<bool, 256U> _keys = {};
};

