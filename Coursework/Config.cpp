#include "pch.h"
#include "Config.h"

Config::Config(std::wstring const& filename) : _filename(filename)
{

}

Config::~Config(void)
{

}

std::wstring Config::Read(std::wstring const& app, std::wstring const& key) const
{
	auto text = std::array<TCHAR, 64U>{};
	GetPrivateProfileString(app.c_str(), key.c_str(), L"0", text.data(),
		static_cast<DWORD>(text.size()), _filename.c_str()); 
	return{ text.data() };
}
