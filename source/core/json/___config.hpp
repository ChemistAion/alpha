#pragma once

namespace Alpha
{
	class Config final
	{
		Core::Filesystem::Entry entry_;
		simdjson::dom::parser parser_;
	public:
		explicit Config(const std::filesystem::path& path) noexcept
			: entry_(path)
		{

		}


	};
}