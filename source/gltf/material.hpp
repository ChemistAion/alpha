#pragma once

namespace Alpha::glTF
{
	struct Material
	{
		struct Texture
		{
			Index index_;
			std::uint64_t coord_ = 0;
			
			Extras extras_;
		};

		struct Normal
		{
			Index index_;
			std::uint64_t coord_ = 0;
			float scale_ = 1.0f;

			Extras extras_;
		};

		struct Occlusion
		{
			Index index_;
			std::uint64_t coord_ = 0;
			float strength_ = 1.0f;

			Extras extras_;
		};

		struct PBR
		{
		};

		enum class AlphaMode
		{
			None,
			Opaque,
			Mask,
			Blend
		};

		////////////////////////////////////////////////////////////////////////////////

		std::optional<PBR> pbr_;
		std::optional<Normal> normal_;
		std::optional<Occlusion> occlusion_;
		std::optional<Texture> emissive_;
		XMFLOAT3 emissive_factor_;
		AlphaMode alpha_mode_ = AlphaMode::None;
		float alpha_cutoff_ = 0.5f;
		bool double_sided = false;

		Name name_;
		Extras extras_;
	};

	using Materials = std::vector<Material>;
	Materials GetMaterials(const simdjson::dom::element& element);
}