#include "_core.hpp"

namespace Alpha::Core::Config::JSON
{
	MetaValue Detial::GetMetaFromDOM(const simdjson::dom::element& element) noexcept
	{
		switch (element.type())
		{
		case simdjson::dom::element_type::NULL_VALUE:
			return MetaValue { Null() };
		case simdjson::dom::element_type::STRING:
			return MetaValue { CISTA::string(element) };
		case simdjson::dom::element_type::INT64:
			return MetaValue { std::int64_t(element) };
		case simdjson::dom::element_type::UINT64:
			return MetaValue { std::uint64_t(element) };
		case simdjson::dom::element_type::DOUBLE:
			return MetaValue { double(element) };
		case simdjson::dom::element_type::BOOL:
			return MetaValue { bool(element) };
		case simdjson::dom::element_type::ARRAY:
		{
			auto unique = CISTA::make_unique<MetaArray>();

			for (const auto& item : simdjson::dom::array(element))
			{
				unique->emplace_back(Detial::GetMetaFromDOM(item));
			}

			return MetaValue { std::move(unique) };
		}
		case simdjson::dom::element_type::OBJECT:
		{
			auto unique = CISTA::make_unique<MetaObject>();

			for (const auto& item : simdjson::dom::object(element))
			{
				unique->emplace(CISTA::string(item.key), Detial::GetMetaFromDOM(item.value));
			}

			return MetaValue { std::move(unique) };
		}
		default:
		{
			assert(false);
			break;
		}
		}

		return MetaValue { Null() };
	}

	////////////////////////////////////////////////////////////////////////////////

	MetaObject GetMetaFromJSON(const Core::Filesystem::Stream& stream) noexcept
	{
		simdjson::dom::parser parser;
		simdjson::padded_string json(stream.Get());

		auto document = parser.parse(json);

		if (document.error())
		{
			//TODO: error handling
			return {};
		}

		auto root = document.get_object();

		if (root.error())
		{
			//TODO: error handling
			return {};
		}

		////////////////////////////////////////////////////////////////////////////////

		MetaObject object;

		for (auto [key, value] : root)
		{
			auto element = object.emplace(CISTA::string(key), Detial::GetMetaFromDOM(value));

			if (false == element.second)
			{
				//TODO: error handling
				continue;
			}
		}

		return object;
	}
}
