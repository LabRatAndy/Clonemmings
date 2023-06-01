#include "Core/Application/Utills.h"
#include "Core/Application/Assert.h"

#include <fstream>
namespace Clonemmings
{
	Buffer FileSystem::ReadBinaryFile(const std::filesystem::path& filename)
	{
		std::ifstream file(filename, std::ios::binary | std::ios::in);
		if (!file.is_open())
		{
			LOGERROR("Unable to open {} for reading", filename.string());
			return {};
		}
		file.seekg(0, std::ios::end);
		uint64_t filesize = file.tellg();
		if (filesize == -1)
		{
			LOGERROR("invalid filesize for {}", filename.string());
			return {};
		}
		file.seekg(0, std::ios::beg);
		Buffer buffer(filesize);
		file.read(buffer.As<char>(), filesize);
		file.close();
		return buffer;
	}
}