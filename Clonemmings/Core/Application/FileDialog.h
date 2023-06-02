#pragma once
#include <string>

namespace Clonemmings
{
	class FileDialog
	{
	public: 
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};
}