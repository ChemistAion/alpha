#pragma once

#include "engine/renderer.hpp"
#include "engine/swapchain.hpp"

//#include "singularity/object.hpp"

#include "imgui/imgui.h"

namespace Alpha::Editor
{
	class Interface final
	{
		std::shared_ptr<Core::Window> window_;
		std::shared_ptr<Engine::Renderer> renderer_;
		std::shared_ptr<Engine::Pipeline> pipeline_;

		std::shared_ptr<Engine::Texture> fonts_;
		Engine::TextureView fonts_srv_;

		const DXGI_FORMAT buffer_color_format_ = DXGI_FORMAT_UNKNOWN;
		const DXGI_FORMAT buffer_depth_format_ = DXGI_FORMAT_UNKNOWN;

		//Singularity::Object object_;

		void CreateInternalResources();
	public:
		Interface
		(
			std::shared_ptr<Core::Window>& window,
			std::shared_ptr<Engine::Renderer>& renderer,
			DXGI_FORMAT buffer_color_format,
			DXGI_FORMAT buffer_depth_format
		);

		Interface(const Interface&) = delete;
		Interface(Interface&&) = delete;
		Interface& operator=(const Interface&) = delete;
		Interface& operator=(Interface&&) = delete;

		~Interface();
		
		////////////////////////////////////////////////////////////////////////////////
		
		void BeginFrame(std::shared_ptr<Engine::SwapChain>& swapchain);
		void RenderFrame();
		void EndFrame(std::shared_ptr<Engine::SwapChain>& swapchain);
		
		//LRESULT Pump(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
	};
}