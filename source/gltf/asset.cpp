#include "_gltf.hpp"

#include "asset.hpp"

namespace Alpha::glTF
{
	Asset GetAsset(const simdjson::dom::element& element)
	{
		Asset asset;

		for (const auto [key, value] : Get<simdjson::dom::object>(element))
		{
			if (key == "copyright")
			{
				asset.copyright_ = Get<std::string_view>(value);
				continue;
			}
		
			if (key == "generator")
			{
				asset.generator_ = Get<std::string_view>(value);
				continue;
			}
			
			if (key == "version")
			{
				asset.version_ = Get<std::string_view>(value);
				continue;
			}
			
			if (key == "minVersion")
			{
				asset.compatibility_ = Get<std::string_view>(value);
				continue;
			}
			
			if (key == "extensions")
			{
				continue;
			}
			
			if (key == "extras")
			{
				asset.extras_ = std::move(GetExtras(value));
				continue;
			}
			
			assert(false);
		}

		return asset;
	}
}
