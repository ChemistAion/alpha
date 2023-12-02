#include "_gltf.hpp"

#include "mesh.hpp"

namespace Alpha::glTF
{
	Meshes GetMeshes(const simdjson::dom::element& element)
	{
		Meshes meshes;

		const auto& items = Get<simdjson::dom::array>(element);
		meshes.reserve(items.size());

		for (const auto item : items)
		{
			const auto object = Get<simdjson::dom::object>(item);
			auto& mesh = meshes.emplace_back();

			for (const auto [key, value] : object)
			{
				if (key == "primitives")
				{
					const auto& items = Get<simdjson::dom::array>(value);
					mesh.primitives_.reserve(items.size());

					for (const auto item : items)
					{
						const auto object = Get<simdjson::dom::object>(item);
						auto& primitive = mesh.primitives_.emplace_back();

						for (const auto [key, value] : object)
						{
							if (key == "attributes")
							{
								const auto object = Get<simdjson::dom::object>(value);
								for (const auto [key, value] : object)
								{
									primitive.attributes_.push_back
									(
										Mesh::Primitive::Attribute
										{
											std::string(key),
											Get<std::uint64_t>(value)
										}
									);
								}

								continue;
							}

							if (key == "indices")
							{
								primitive.indices_ = Get<std::uint64_t>(value);
								continue;
							}

							if (key == "material")
							{
								primitive.material_ = Get<std::uint64_t>(value);
								continue;
							}

							if (key == "mode")
							{
								primitive.mode_ = static_cast<Mesh::Primitive::Mode>(Get<std::uint64_t>(value));
								continue;
							}

							if (key == "targets")
							{
								assert(false);
								continue;
							}

							if (key == "extensions")
							{
								assert(false);
								continue;
							}

							if (key == "extras")
							{
								primitive.extras_ = std::move(GetExtras(value));
								continue;
							}

							assert(false);
						}
					}

					continue;
				}

				if (key == "weights")
				{
					assert(false);
					continue;
				}

				////////////////////////////////////////////////////////////////////////////////

				if (key == "name")
				{
					mesh.name_ = Get<std::string_view>(value);
					continue;
				}

				if (key == "extensions")
				{
					assert(false);
					continue;
				}

				if (key == "extras")
				{
					mesh.extras_ = std::move(GetExtras(value));
					continue;
				}

				assert(false);
			}
		}

		return meshes;
	}
}
