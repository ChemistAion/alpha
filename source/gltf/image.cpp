#include "_gltf.hpp"

#include "image.hpp"

namespace Alpha::glTF
{
	Images GetImages(const simdjson::dom::element& element)
	{
		Images images;

		const auto& items = Get<simdjson::dom::array>(element);
		images.reserve(items.size());

		for (const auto item : items)
		{
			const auto object = Get<simdjson::dom::object>(item);
			auto& image = images.emplace_back();

			std::optional<std::string> uri;
			std::optional<Index> buffer_view;
			std::optional<std::string> mime_type;

			for (const auto [key, value] : object)
			{
				if (key == "uri")
				{
					uri = Get<std::string_view>(value);
					continue;
				}

				if (key == "mimeType")
				{
					mime_type = Get<std::string_view>(value);
					continue;
				}

				if (key == "bufferView")
				{
					buffer_view = Get<std::uint64_t>(value);
					continue;
				}

				////////////////////////////////////////////////////////////////////////////////

				if (key == "name")
				{
					image.name_ = Get<std::string_view>(value);
					continue;
				}

				if (key == "extensions")
				{
					continue;
				}

				if (key == "extras")
				{
					image.extras_ = std::move(GetExtras(value));
					continue;
				}

				assert(false);
			}

			////////////////////////////////////////////////////////////////////////////////

			if (uri.has_value())
			{
				image.data_ = Image::DataURI{ *uri, {} };
				continue;
			}

			if (buffer_view.has_value())
			{
				assert(mime_type.has_value());
				image.data_ = Image::DataBufferView{ *mime_type, *buffer_view };
				continue;
			}

			assert(false);
		}

		return images;
	}
}
