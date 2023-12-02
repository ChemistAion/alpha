#pragma once

namespace Alpha::glTF
{
	struct Camera
	{
		struct Perspective
		{
			float aspect_ratio_;
			float fov_;
			float z_far_;
			float z_near_;

			Extras extras_;
		};

		struct Orthographic
		{
			float x_mag_;
			float y_mag_;
			float z_far_;
			float z_near_;

			Extras extras_;
		};

		std::variant<Orthographic, Perspective> projection_;

		Name name_;
		Extras extras_;
	};

	using Cameras = std::vector<Camera>;
	Cameras GetCameras(const simdjson::dom::element& element);
}