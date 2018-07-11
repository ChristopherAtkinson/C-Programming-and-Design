#pragma once

class Config
{
public:
	explicit Config(std::wstring const& filename);
	~Config(void);

	std::wstring Read(std::wstring const& app, std::wstring const& key) const;
	template <typename T>
	T Read(std::wstring const& app, std::wstring const& key) const
	{
		T value;
		std::wistringstream{ Read(app, key) } >> std::boolalpha >> value;
		return value;
	}

private: 
	std::wstring _filename = {};
};