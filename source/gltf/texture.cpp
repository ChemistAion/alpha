#include "_gltf.hpp"

#include "texture.hpp"

namespace Alpha::glTF
{
	Textures GetTextures(const simdjson::dom::element& element)
	{
		Textures textures;

		const auto& items = Get<simdjson::dom::array>(element);
		textures.reserve(items.size());

		for (const auto item : items)
		{
			const auto object = Get<simdjson::dom::object>(item);
			auto& texture = textures.emplace_back();

			for (const auto [key, value] : object)
			{
				if (key == "sampler")
				{
					texture.sampler_ = std::move(Get<std::uint64_t>(value));
					continue;
				}

				if (key == "source")
				{
					texture.source_ = std::move(Get<std::uint64_t>(value));
					continue;
				}

				////////////////////////////////////////////////////////////////////////////////

				if (key == "name")
				{
					texture.name_ = Get<std::string_view>(value);
					continue;
				}

				if (key == "extensions")
				{
					continue;
				}

				if (key == "extras")
				{
					texture.extras_ = std::move(GetExtras(value));
					continue;
				}

				assert(false);
			}
		}

		return textures;
	}
}