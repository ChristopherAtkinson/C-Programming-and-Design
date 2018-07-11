#include "pch.h"
#include "input.h"

input::input(void)
{
	for (auto& it : _keys)
		it = false;
}

input::~input(void)
{

}