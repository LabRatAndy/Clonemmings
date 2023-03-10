#pragma once
#include <cinttypes>

namespace Clonemmings
{
	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;

		operator uint64_t() const { return m_UUID; }

	private:
		uint64_t m_UUID;
	};
}

//hash function template specialsiation

namespace std
{
	template<typename T> struct hash;
	template<>
	struct hash<Clonemmings::UUID>
	{
		size_t operator()(const Clonemmings::UUID& uuid) const
		{
			return (uint64_t)uuid;
		}
	};
}