#pragma once

#include "engine/renderer.hpp"
#include "imgui/imgui.h"

namespace Alpha::Root::Component
{
	class Editor
	{
		struct Fonts
		{
			std::shared_ptr<Alpha::Engine::Texture> texture_;
			Alpha::Engine::TextureView srv_;
		};

		std::shared_ptr<Fonts> fonts_;

		std::shared_ptr<Alpha::Engine::Pipeline> pipeline_;
	public:
		void Create(const std::shared_ptr<Alpha::Engine::Renderer>& renderer);
		void Destroy();

		void Begin(const std::shared_ptr<Alpha::Engine::Renderer>& renderer);
		void Render(const std::shared_ptr<Alpha::Engine::Renderer>& renderer);
		void End(const std::shared_ptr<Alpha::Engine::Renderer>& renderer);
	};
}