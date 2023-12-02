#pragma once

namespace Alpha::Core::Config
{
	class Config final
	{
		const MetaObject object_;

		//TODO: noexcept
		MetaObject Build(const std::filesystem::path& path)
		{
			auto stream = Core::Filesystem::Stream(path);
			return Core::Config::JSON::GetMetaFromJSON(stream);
		}

	public:
		Config() noexcept = default;

		explicit Config(const std::filesystem::path& path) noexcept
		  : object_(Build(path))
		{
		}

		////////////////////////////////////////////////////////////////////////////////

		bool IsEnabled(const char* name) const noexcept
		{
			if (auto element = object_.find(CISTA::string(name)); element != std::end(object_))
			{
				if (const auto result = cista::get_if<bool>(element->second); result != nullptr)
				{
					return *result;
				}
			}

			return false;
		}

		//TODO: POD only
		template <typename Type>
		std::pair<Type, bool> GetValue(const char* name) const noexcept
		{
			if (auto element = object_.find(CISTA::string(name)); element != std::end(object_))
			{
				if (const auto result = cista::get_if<Type>(element->second); result != nullptr)
				{
					return { *result, true };
				}
			}

			return { Type(), false };
		}

		std::vector<std::string> GetNamesList(const char* name) const noexcept
		{
			if (auto element = object_.find(CISTA::string(name)); element != std::end(object_))
			{
				if (const auto result = cista::get_if<CISTA::unique_ptr<MetaArray>>(element->second); result != nullptr)
				{
					std::vector<std::string> names;

					for (const auto& element : *result->get())
					{
						if (const auto name = cista::get_if<CISTA::string>(element); name != nullptr)
						{
							names.emplace_back(name->str());
						}
					}

					return names;
				}
			}

			return {};
		}
	};
}