#include "_gltf.hpp"

#include "material.hpp"

namespace Alpha::glTF
{
	Materials GetMaterials(const simdjson::dom::element& element)
	{
		Materials materials;

		const auto& items = Get<simdjson::dom::array>(element);
		materials.reserve(items.size());

		for (const auto item : items)
		{
			const auto object = Get<simdjson::dom::object>(item);
			auto& material = materials.emplace_back();

			for (const auto [key, value] : object)
			{
				if (key == "normalTexture")
				{
					material.normal_ = Material::Normal();
					continue;
				}

				if (key == "occlusionTexture")
				{
					material.occlusion_ = Material::Occlusion();
					continue;
				}

				if (key == "emissiveTexture")
				{
					material.emissive_ = Material::Texture();
					continue;
				}

				if (key == "pbrMetallicRoughness")
				{
					material.pbr_ = Material::PBR();
					continue;
				}

				if (key == "emissiveFactor")
				{
					auto vector = GetVector<float>(value);
					assert(vector.size() == 3);

					material.emissive_factor_ = XMFLOAT3(vector.data());
					continue;
				}

				if (key == "alphaMode")
				{
					const auto mode = Get<std::string_view>(value);

					if (mode == "OPAQUE")
					{
						material.alpha_mode_ = Material::AlphaMode::Opaque;
						continue;
					}

					if (mode == "MASK")
					{
						material.alpha_mode_ = Material::AlphaMode::Mask;
						continue;
					}

					if (mode == "BLEND")
					{
						material.alpha_mode_ = Material::AlphaMode::Blend;
						continue;
					}

					assert(false);
				}

				if (key == "alphaCutoff")
				{
					material.alpha_cutoff_ = Get<float>(value);
					continue;
				}

				if (key == "doubleSided")
				{
					material.double_sided = Get<bool>(value);
					continue;
				}

				////////////////////////////////////////////////////////////////////////////////

				if (key == "name")
				{
					material.name_ = Get<std::string_view>(value);
					continue;
				}

				if (key == "extensions")
				{
					continue;
				}

				if (key == "extras")
				{
					material.extras_ = std::move(GetExtras(value));
					continue;
				}

				assert(false);
			}
		}

		return materials;
	}
}
