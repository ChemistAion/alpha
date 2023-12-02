#include "_root.hpp"

//	#include "imgui/backends/imgui_impl_win32.h"
//	extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Alpha::Root::System
{
	LRESULT Engine::Pump(HWND handle, UINT message, WPARAM w_param, LPARAM l_param)
	{
		switch (message)
		{
		case WM_KEYDOWN:
		{
			if (w_param == VK_ESCAPE)
			{
				PostQuitMessage(0);
			}

			break;
		}
		case WM_SIZE:
		{
			RECT client = {};
			GetClientRect(handle, &client);

			auto registry = ECS::Registry::GetInstance();
			registry->Process(Root::Event::Resize { std::uint32_t(client.right - client.left), std::uint32_t(client.bottom - client.top) });
		}
		}

		return 0;
	}

	Engine::Engine()
	{
		renderer_ = std::make_shared<Alpha::Engine::Renderer>();

		auto window = Alpha::Core::Window::GetInstance();

		Alpha::Engine::SwapChainDesc swapchain_desc;
		swapchain_desc.name_ = "SwapChain";
		swapchain_desc.window_ = window;
		swapchain_ = renderer_->CreateSwapChain(swapchain_desc);

		////////////////////////////////////////////////////////////////////////////////

		window->RegisterPump(Engine::Pump);
	}

	void Engine::Process(ECS::Registry& registry, Event::Resize&& resize)
	{
		renderer_->Flush(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		renderer_->Flush(D3D12_COMMAND_LIST_TYPE_DIRECT);

		swapchain_->Resize(resize.width_, resize.height_);
		return;
	}

	void Engine::Process(ECS::Registry& registry, Event::Create&&)
	{
		registry.GetView<Component::Editor>().Each
		(
			[this](const auto, Component::Editor& editor)
			{ 
				editor.Create(renderer_);
			}
		);
	}

	void Engine::Process(ECS::Registry& registry, Event::Process&&)
	{
	}

	void Engine::Process(ECS::Registry& registry, Event::Frame::Begin&&)
	{
		renderer_->BeginFrame();

		auto& context = renderer_->GetContext();

		//TODO: new command list should be moved to Renderer::BeginFrame
		auto cmd_list = renderer_->GetCommands().GetNewCommandList();
		context.SetCommandList(cmd_list);

		//TODO: improve me
		{
			auto bb_barier = CD3DX12_RESOURCE_BARRIER::Transition(swapchain_->GetBackBufferResource(), D3D12_RESOURCE_STATE_PRESENT,
			  D3D12_RESOURCE_STATE_RENDER_TARGET);
			cmd_list->ResourceBarrier(1, &bb_barier);

			cmd_list->OMSetRenderTargets(1, swapchain_->GetBackBufferHandle(), TRUE, NULL);

			float clear_color[] = { 0.1f, 0.1f, 0.1f, 1.0f };
			cmd_list->ClearRenderTargetView(*swapchain_->GetBackBufferHandle(), clear_color, 0, NULL);

			//TODO:
			//RSSetViewports
			//RSSetScissorRects

		}

		registry.GetView<Component::Editor>().Each
		(
			[this](const auto, Component::Editor& editor) 
			{ 
				editor.Begin(renderer_);
			}
		);
	}

	void Engine::Process(ECS::Registry& registry, Event::Frame::Render&&)
	{
		////////////////////////////////////////////////////////////////////////////////

		ImGui::ShowMetricsWindow();

		registry.GetView<Component::Editor>().Each
		(
			[this](const auto, Component::Editor& editor) 
			{ 
				editor.Render(renderer_); 
			}
		);
	}

	void Engine::Process(ECS::Registry& registry, Event::Frame::End&&)
	{
		auto& context = renderer_->GetContext();
		context.EndFrame();

		auto cmd_list = renderer_->GetCommands().GetNewCommandList();

		auto bb_barier = CD3DX12_RESOURCE_BARRIER::Transition(swapchain_->GetBackBufferResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

		cmd_list->ResourceBarrier(1, &bb_barier);

		auto result = cmd_list->Close();
		Alpha::Engine::ThrowIfFailed(result, "[Context::Flush] ID3D12GraphicsCommandList::Close failed");

		ID3D12CommandList* const lists[] = { cmd_list };
		renderer_->GetGraphicsQueue()->ExecuteCommandLists(1, lists);

		////////////////////////////////////////////////////////////////////////////////

		swapchain_->Present();
		swapchain_->Wait(renderer_->GetFrame());

		//TODO: flush every frame for GPU Validation
		//renderer_->Flush(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		//renderer_->Flush(D3D12_COMMAND_LIST_TYPE_DIRECT);

		renderer_->EndFrame();
	}

	void Engine::Process(ECS::Registry& registry, Event::Terminate&& terminate)
	{
		//TODO: Kill 'Em All
		//TODO: improve orphans

		registry.GetView<Root::Component::Editor>().Each
		(
			[this](const auto, Root::Component::Editor& editor) 
			{
				editor.Destroy();
			}
		);

		swapchain_.reset();
		renderer_.reset();

		if constexpr (Alpha::Engine::cEngineMode_Debug && !Alpha::Engine::cEngineMode_PIX)
		{
			HMODULE library = ::LoadLibraryEx(L"dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

			//TODO: error handling
			if (library)
			{
				typedef HRESULT(WINAPI * LPDXGIGETDEBUGINTERFACE)(REFIID, void**);
				auto debug = reinterpret_cast<LPDXGIGETDEBUGINTERFACE>(reinterpret_cast<void*>(::GetProcAddress(library, "DXGIGetDebugInterface")));

				IDXGIDebug* debug_controller;
				if (SUCCEEDED(debug(IID_PPV_ARGS(&debug_controller))))
				{
					auto result = debug_controller->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
					debug_controller->Release();
				}
			}
		}

	}
}