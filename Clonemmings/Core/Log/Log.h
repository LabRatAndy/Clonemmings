#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#pragma warning(push,0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)
#include <memory>
namespace Clonemmings
{
	class Log
	{
	public: 
		static void Init();

		static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }

	private:
		static std::shared_ptr<spdlog::logger> s_Logger;
	};
}

//glm to string functions 
template<typename OStream , glm::length_t L,typename  T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::qua<T, Q> quaternion)
{
	return os << glm::to_string(quaternion);
}

//logging macro functions 
#ifdef DIST
#define TRACE(...)
#define INFO(...)
#define LOGERROR(...)
#define WARN(...)
#define CRITICAL(...)
#else
#define TRACE(...) ::Clonemmings::Log::GetLogger()->trace(__VA_ARGS__)
#define WARN(...) ::Clonemmings::Log::GetLogger()->warn(__VA_ARGS__)
#define INFO(...) ::Clonemmings::Log::GetLogger()->info(__VA_ARGS__)
#define LOGERROR(...) ::Clonemmings::Log::GetLogger()->error(__VA_ARGS__)
#define CRITICAL(...) ::Clonemmings::Log::GetLogger()->critical(__VA_ARGS__)
#endif