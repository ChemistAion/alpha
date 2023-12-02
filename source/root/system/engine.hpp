#pragma once

#include "engine/renderer.hpp"

namespace Alpha::Root::System
{
	class Engine final : public ECS::System<Event::Create, Event::Resize, Event::Process, Event::Frame::Begin, Event::Frame::Render, Event::Frame::End, Event::Terminate>
	{
		std::shared_ptr<Alpha::Engine::Renderer> renderer_;
		std::shared_ptr<Alpha::Engine::SwapChain> swapchain_;

		void CreateInternalResources();

		static LRESULT Pump(HWND handle, UINT message, WPARAM w_param, LPARAM l_param);
	public:
		Engine();

		void Process(ECS::Registry& registry, Event::Create&&) override;
		void Process(ECS::Registry& registry, Event::Resize&&) override;
		void Process(ECS::Registry& registry, Event::Process&&) override;

		void Process(ECS::Registry& registry, Event::Frame::Begin&&) override;
		void Process(ECS::Registry& registry, Event::Frame::Render&&) override;
		void Process(ECS::Registry& registry, Event::Frame::End&&) override;

		void Process(ECS::Registry& registry, Event::Terminate&&) override;
	};
}