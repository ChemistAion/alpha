#pragma once

// https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_lights_punctual

namespace Alpha::glTF
{
	struct Light
	{
		struct Directional
		{
		};

		struct Point
		{
			std::optional<float> range_;
		};

		struct Spot
		{
			std::optional<float> range_;
			float inner_angle_ = 0.0f;
			float outer_angle_ = XM_PIDIV4;
		};

		////////////////////////////////////////////////////////////////////////////////

		Name name_;
		XMFLOAT3 color_;
		float intensity_ = 1.0f;
		std::variant<Directional, Point, Spot> parameters_;
	};

	using Lights = std::vector<Light>;
	Lights GetLights(const simdjson::dom::element& element);
}