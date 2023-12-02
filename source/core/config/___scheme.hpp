#pragma once

namespace Alpha::Core::Config
{
	class Scheme final
	{	
		Config::Object config_;
	public:
		//TODO: noexcept
		explicit Scheme(const std::string_view stream)
		{
			if (stream.empty())
			{
				return;
			}

			////////////////////////////////////////////////////////////////////////////////

			simdjson::padded_string json(stream);
			simdjson::dom::parser parser;

			auto document = parser.parse(json);

			if (document.error())
			{
				//TODO: error handling
				return;
			}

			auto root = document.get_object();

			if (root.error())
			{
				//TODO: error handling
				return;
			}

			////////////////////////////////////////////////////////////////////////////////		

			for (auto [key, value] : root)
			{
				auto element = config_.try_emplace
				(
					std::string(key),
					JSON::GetValue(value)
				);

				if (false == element.second)
				{
					//TODO: error handling
					continue;
				}
			}


			////////////////////////////////////////////////////////////////////////////////

			return;
		}
	};
}