#include "_gltf.hpp"

#include "camera.hpp"

namespace Alpha::glTF
{
	Cameras GetCameras(const simdjson::dom::element& element)
	{
		Cameras cameras;

		const auto& items = Get<simdjson::dom::array>(element);
		cameras.reserve(items.size());

		for (const auto item : items)
		{
			const auto object = Get<simdjson::dom::object>(item);
			auto& camera = cameras.emplace_back();

			std::string type;
			std::optional<Camera::Perspective> perspective;
			std::optional<Camera::Orthographic> orthographic;

			for (const auto [key, value] : object)
			{
				if (key == "type")
				{
					type = Get<std::string_view>(value);
					continue;
				}

				if (key == "perspective")
				{
					const auto object = Get<simdjson::dom::object>(value);
					perspective = Camera::Perspective();

					for (const auto [key, value] : object)
					{
						if (key == "aspectRatio")
						{
							perspective->aspect_ratio_ = Get<float>(value);
							continue;
						}

						if (key == "yfov")
						{
							perspective->fov_ = Get<float>(value);
							continue;
						}

						if (key == "zfar")
						{
							perspective->z_far_ = Get<float>(value);
							continue;
						}

						if (key == "znear")
						{
							perspective->z_near_ = Get<float>(value);
							continue;
						}

						if (key == "extensions")
						{
							continue;
						}

						if (key == "extras")
						{
							perspective->extras_ = std::move(GetExtras(value));
							continue;
						}

						assert(false);
					}

					continue;
				}

				if (key == "orthographic")
				{
					const auto object = Get<simdjson::dom::object>(value);
					orthographic = Camera::Orthographic();

					for (const auto [key, value] : object)
					{
						if (key == "xmag")
						{
							orthographic->x_mag_ = Get<float>(value);
							continue;
						}

						if (key == "ymag")
						{
							orthographic->y_mag_ = Get<float>(value);
							continue;
						}

						if (key == "zfar")
						{
							orthographic->z_far_ = Get<float>(value);
							continue;
						}

						if (key == "znear")
						{
							orthographic->z_near_ = Get<float>(value);
							continue;
						}

						if (key == "extensions")
						{
							continue;
						}

						if (key == "extras")
						{
							orthographic->extras_ = std::move(GetExtras(value));
							continue;
						}

						assert(false);
					}

					continue;
				}

				////////////////////////////////////////////////////////////////////////////////

				if (key == "name")
				{
					camera.name_ = Get<std::string_view>(value);
					continue;
				}

				if (key == "extensions")
				{
					continue;
				}

				if (key == "extras")
				{
					camera.extras_ = std::move(GetExtras(value));
					continue;
				}

				assert(false);
			}
		
			////////////////////////////////////////////////////////////////////////////////

			if (type == "perspective")
			{
				assert(perspective.has_value());
				camera.projection_ = std::move(*perspective);
				continue;
			}

			if (type == "orthographic")
			{
				assert(orthographic.has_value());
				camera.projection_ = std::move(*orthographic);
				continue;
			}

			assert(false);
		}

		return cameras;
	}
}
