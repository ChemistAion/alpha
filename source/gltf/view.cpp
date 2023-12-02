#include "_gltf.hpp"

#include "view.hpp"

namespace Alpha::glTF
{
	Views GetViews(const simdjson::dom::element& element)
	{
		Views views;

		const auto& items = Get<simdjson::dom::array>(element);
		views.reserve(items.size());

		for (const auto item : items)
		{
			const auto object = Get<simdjson::dom::object>(item);
			auto& view = views.emplace_back();

			for (const auto [key, value] : object)
			{
				if (key == "buffer")
				{
					view.buffer_= Get<std::uint64_t>(value);
					continue;
				}

				if (key == "byteOffset")
				{
					view.offset_ = Get<std::uint64_t>(value);
					continue;
				}

				if (key == "byteLength")
				{
					view.size_ = Get<std::uint64_t>(value);
					continue;
				}

				if (key == "byteStride")
				{
					view.stride_ = Get<std::uint64_t>(value);
					continue;
				}

				if (key == "target")
				{
					view.target_ = static_cast<View::Target>(Get<std::uint64_t>(value));
					continue;
				}

				////////////////////////////////////////////////////////////////////////////////

				if (key == "name")
				{
					view.name_ = Get<std::string_view>(value);
					continue;
				}

				if (key == "extensions")
				{
					continue;
				}

				if (key == "extras")
				{
					view.extras_ = std::move(GetExtras(value));
					continue;
				}

				assert(false);
			}
		}

		return views;
	}
}
