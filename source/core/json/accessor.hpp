#pragma once

namespace Alpha::Core::JSON
{
	//TODO: remove float handling
	template <typename Type>
	Type Get(const simdjson::dom::element& element)
	{
		if constexpr (std::is_same_v<Type, float>)
		{
			double result;
		
			if (auto error = element.get<double>(result); error)
			{
				assert(false);
				return {};
			}
		
			return float(result);
		}
		else
		{
			Type result;
			if (auto error = element.get<Type>(result); error)
			{
				assert(false);
				return {};
			}
		
			return result;
		}
	}

	//TODO: optimize this, make it at once
	template <typename Type>
	std::vector<Type> GetVector(const simdjson::dom::element& element)
	{
		std::vector<Type> vector;

		const auto items = Get<simdjson::dom::array>(element);
		vector.reserve(items.size());

		for (const auto item : items)
		{
			vector.emplace_back(Get<Type>(item));
		}

		return vector;
	}

	////////////////////////////////////////////////////////////////////////////////

	//TODO: make it happen - noexcept
	Value GetValue(const simdjson::dom::element& element) noexcept;
}



//	template <class Type>
//	void GetValue(Config::Object& result, Type&& element) noexcept
//	{
//		bool add_comma;
//		switch (element.type())
//		{
//		case simdjson::ondemand::json_type::array:
//			std::cout << "[";
//			add_comma = false;
//			for (auto child : element.get_array())
//			{
//				if (add_comma) {
//					std::cout << ",";
//				}
//				// We need the call to value() to get
//				// an ondemand::value type.
//				GetValue(result, child.value());
//				add_comma = true;
//			}
//			std::cout << "]";
//			break;
//		case simdjson::ondemand::json_type::object:
//			std::cout << "{";
//			add_comma = false;
//			for (auto field : element.get_object())
//			{
//				if (add_comma) {
//					std::cout << ",";
//				}
//				// key() returns the key as it appears in the raw
//				// JSON document, if we want the unescaped key,
//				// we should do field.unescaped_key().
//				std::cout << "\"" << field.key() << "\": ";
//				GetValue(result, field.value());
//				add_comma = true;
//			}
//			std::cout << "}";
//			break;
//		case simdjson::ondemand::json_type::number:
//			// assume it fits in a double
//			std::cout << element.get_double();
//			break;
//		case simdjson::ondemand::json_type::string:
//			// get_string() would return escaped string, but
//			// we are happy with unescaped string.
//			std::cout << "\"" << element.get_raw_json_string() << "\"";
//			break;
//		case simdjson::ondemand::json_type::boolean:
//			std::cout << element.get_bool();
//			break;
//		case simdjson::ondemand::json_type::null:
//			std::cout << "null";
//			break;
//		}
//	}
//	}

//	template <class Type>
//	void GetValue(Config::Object& result, Type&& element)
//	{
//		switch (element.type())
//		{
//		case simdjson::ondemand::json_type::string:
//		{
//			if (const auto result = element.get_string(); false == result.error())
//			{
//				//return std::string(result.value_unsafe());
//			}
//	
//			//return std::string();
//			break;
//		}
//		case simdjson::ondemand::json_type::number:
//		{
//			//return Config::Null{};
//			break;
//		}
//		case simdjson::ondemand::json_type::array:
//		{
//			Config::Value value = std::make_unique<Config::Array>();
//			//	auto& array = *std::get<std::unique_ptr<Config::Array>>(value);
//			//	
//			//	for (const auto& item : simdjson::dom::array(element))
//			//	{
//			//		array.emplace_back(GetValue(item));
//			//	}	
//			// 
//			//return value;
//			break;
//		}
//		case simdjson::ondemand::json_type::object:
//		{
//			Config::Value value = std::make_unique<Config::Object>();
//			auto& object = *std::get<std::unique_ptr<Config::Object>>(value);
//	
//			for (auto field : element.get_object())
//			{
//				auto key = field.key();
//				GetValue(object, field.value());
//			}
//	
//			//	for (auto [field, error] : element.get_object())
//			//	{
//			//		continue;
//			//	}
//			//	for (const auto& item : simdjson::dom::object(element))
//			//	{
//			//		object.emplace(item.key, GetValue(item.value));
//			//	}
//	
//			//return value;
//			break;
//		}
//		case simdjson::ondemand::json_type::boolean:
//		{
//			//return element.get_bool();
//			break;
//		}
//		case simdjson::ondemand::json_type::null:
//		{
//			break;
//		}
//		default:
//		{
//			break;
//		}
//		}
//	}