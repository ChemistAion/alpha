#pragma once

namespace Alpha::Core::JSON
{
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
}


