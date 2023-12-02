#pragma once

namespace Alpha::Core::Config
{
	namespace CISTA = cista::offset;

	constexpr auto const MODE =
		cista::mode::WITH_VERSION |
		cista::mode::WITH_INTEGRITY |
		cista::mode::DEEP_CHECK;

	////////////////////////////////////////////////////////////////////////////////

	struct MetaArray;
	struct MetaObject;

	using Null = std::monostate;

	using MetaValue = CISTA::variant
		<
		Null,
		CISTA::string,
		std::int64_t,
		std::uint64_t,
		double,
		bool,
		CISTA::unique_ptr<MetaArray>,
		CISTA::unique_ptr<MetaObject>
		>;

	struct MetaArray : public CISTA::vector<MetaValue>
	{
	};

	struct MetaObject : public CISTA::hash_map<CISTA::string, MetaValue>
	{
	};

	////////////////////////////////////////////////////////////////////////////////

	template <typename Ctx>
	inline void serialize(Ctx& c, const MetaArray* origin, const cista::offset_t offset)
	{
		cista::serialize(c, static_cast<const CISTA::vector<MetaValue>*>(origin), offset);
	}
	
	template <typename Ctx>
	inline void deserialize(const Ctx& c, MetaArray* el)
	{
		cista::deserialize(c, static_cast<CISTA::vector<MetaValue>*>(el));
	}

	template <typename Ctx>
	inline void serialize(Ctx& c, const MetaObject* origin, const cista::offset_t offset)
	{
		cista::serialize(c, static_cast<const CISTA::hash_map<CISTA::string, MetaValue>*>(origin), offset);
	}
	
	template <typename Ctx>
	inline void deserialize(const Ctx& c, MetaObject* el)
	{
		cista::deserialize(c, static_cast<CISTA::hash_map<CISTA::string, MetaValue>*>(el));
	}
}