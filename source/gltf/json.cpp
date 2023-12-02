#include "_gltf.hpp"

#include "json.hpp"

namespace Alpha::glTF
{
	Value GetValue(const simdjson::dom::element& element)
	{
		switch (element.type())
		{
		case simdjson::dom::element_type::NULL_VALUE:
			return Null {};
		case simdjson::dom::element_type::STRING:
			return std::string(std::string_view(element));
		case simdjson::dom::element_type::INT64:
			return std::int64_t(element);
		case simdjson::dom::element_type::UINT64:
			return std::uint64_t(element);
		case simdjson::dom::element_type::DOUBLE:
			return double(element);
		case simdjson::dom::element_type::BOOL:
			return bool(element);
		case simdjson::dom::element_type::ARRAY:
		{
			Value value = std::make_unique<Array>();
			auto& array = *std::get<std::unique_ptr<Array>>(value);

			for (const auto& item : simdjson::dom::array(element))
			{
				array.emplace_back(GetValue(item));
			}

			return value;
		} 
		case simdjson::dom::element_type::OBJECT:
		{
			Value value = std::make_unique<Object>();
			auto& object = *std::get<std::unique_ptr<Object>>(value);

			for (const auto& item : simdjson::dom::object(element))
			{
				object.emplace(item.key, GetValue(item.value));
			}

			return value;
		}
		default:
		{
			break;
		}		
		}

		assert(false);
		return Null{};
	}
}
