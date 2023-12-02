#pragma once

namespace Alpha::glTF
{
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

	using Null = std::monostate;

	struct Array;
	struct Object;

	using Value = std::variant
		<
		Null,
		std::string,
		std::int64_t,
		std::uint64_t,
		double,
		bool,
		std::unique_ptr<Array>,
		std::unique_ptr<Object>
		>;

	struct Array : public std::vector<Value>
	{
	};

	struct Object : public std::unordered_map<std::string, Value>
	{
	};

	Value GetValue(const simdjson::dom::element& element);
}