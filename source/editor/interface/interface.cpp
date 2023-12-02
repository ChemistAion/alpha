#include "_editor.hpp"

#include "interface.hpp"
#include "imgui/backends/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Alpha::Editor
{
	Interface::Interface
	(
		std::shared_ptr<Core::Window>& window,
		std::shared_ptr<Engine::Renderer>& renderer,
		DXGI_FORMAT buffer_color_format,
		DXGI_FORMAT buffer_depth_format
	)
		: window_ { window }
		, renderer_ { renderer }
		, buffer_color_format_ { buffer_color_format }
		, buffer_depth_format_ { buffer_depth_format }
	{
		::IMGUI_CHECKVERSION();
		window->RegisterPump(::ImGui_ImplWin32_WndProcHandler);
		::ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendRendererName = "Editor";
		io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
		//io.IniFilename = nullptr;

		CreateInternalResources();

		::ImGui_ImplWin32_Init(window->GetHandle());

		::ImGui::StyleColorsDark();

		////////////////////////////////////////////////////////////////////////////////

		//object_.RegisterFeature<Engine::Renderer>();
	}

	Interface::~Interface()
	{
		::ImGui::DestroyContext();
		::ImGui_ImplWin32_Shutdown();
	}

	void Interface::CreateInternalResources()
	{
		Engine::PipelineDesc pipeline_desc;

		pipeline_desc.name_ = "EditorInterface";
		
		//TODO: artificial, handle SwapChain RTV
		pipeline_desc.RT_count_ = 1;
		pipeline_desc.RTV_format[0] = buffer_color_format_;
		pipeline_desc.DSV_format = buffer_depth_format_;
		pipeline_desc.rasterizer_state_.CullMode = D3D12_CULL_MODE_NONE;
		pipeline_desc.depth_stencil_state_.DepthEnable = FALSE;

		////////////////////////////////////////////////////////////////////////////////

		{
			Engine::ShaderDesc shader_desc;

			shader_desc.name_ = "SignatureTest";
			shader_desc.type_ = Engine::ShaderType::Vertex;
			shader_desc.file_ = "shaders\\interface.vsh";
			shader_desc.entry_ = L"main";
			//TODO: template for automatic creation based on <shader_desc>
			pipeline_desc.shaders_[shader_desc.type_] = renderer_->CreateShader(shader_desc);

			shader_desc.name_ = "EditorInterfacePS";
			shader_desc.type_ = Engine::ShaderType::Pixel;
			shader_desc.file_ = "shaders\\interface.psh";
			shader_desc.entry_ = L"main";
			//TODO: template for automatic creation based on <shader_desc>
			pipeline_desc.shaders_[shader_desc.type_] = renderer_->CreateShader(shader_desc);
		}

		////////////////////////////////////////////////////////////////////////////////

		{
			auto& blend_state = pipeline_desc.blend_state_.RenderTarget[0];

			blend_state.BlendEnable = TRUE;
			blend_state.LogicOpEnable = FALSE;
			blend_state.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blend_state.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			blend_state.BlendOp = D3D12_BLEND_OP_ADD;
			blend_state.SrcBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
			blend_state.DestBlendAlpha = D3D12_BLEND_ZERO;
			blend_state.BlendOpAlpha = D3D12_BLEND_OP_ADD;
			blend_state.LogicOp = D3D12_LOGIC_OP_NOOP;
			blend_state.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		}

		////////////////////////////////////////////////////////////////////////////////

		pipeline_desc.input_layout_.push_back
		(
			{
				"ATTRIB",
				0,
				DXGI_FORMAT_R32G32_FLOAT,
				0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				0
			}
		);

		pipeline_desc.input_layout_.push_back
		(
			{
				"ATTRIB",
				1,
				DXGI_FORMAT_R32G32_FLOAT,
				0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				0
			}
		);

		pipeline_desc.input_layout_.push_back
		(
			{
				"ATTRIB",
				2,
				DXGI_FORMAT_R8G8B8A8_UNORM,
				0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				0
			}
		);

		pipeline_ = renderer_->CreatePipeline(pipeline_desc);

		////////////////////////////////////////////////////////////////////////////////

		{
			ImGuiIO& io = ImGui::GetIO();

			unsigned char* fonts_pixels = nullptr;
			int fonts_width = 0;
			int fonts_height = 0;
			io.Fonts->GetTexDataAsRGBA32(&fonts_pixels, &fonts_width, &fonts_height);

			Engine::TextureDesc texture_desc;
			texture_desc.name_ = "Texture";
			texture_desc.width_ = fonts_width;
			texture_desc.height_ = fonts_height;
			texture_desc.dimension_ = Engine::ResourceDimension::Texture2D;
			texture_desc.format_ = DXGI_FORMAT_R8G8B8A8_UNORM;
			texture_desc.usage_ = Engine::ResourceUsage::Static;
			texture_desc.bind_ = Engine::ResourceBind::ShaderResource;

			Engine::TextureResourceData fonts_mip[] =
			{
				{ fonts_pixels, (std::uint32_t)(fonts_width * 4), 0 }
			};

			Engine::TextureData fonts_data(fonts_mip, _countof(fonts_mip));

			fonts_ = renderer_->CreateTexture(texture_desc, &fonts_data);

			Engine::TextureViewDesc view_desc;
			view_desc.type_ = Engine::ResourceView::ShaderResource;

			fonts_srv_ = fonts_->CreateView(view_desc);
			pipeline_->Bind(fonts_srv_);
			io.Fonts->TexID = (ImTextureID)&fonts_srv_;
		}
	}

	void Interface::BeginFrame(std::shared_ptr<Engine::SwapChain>& swapchain)
	{
		assert(pipeline_);

		renderer_->BeginFrame();
		
		auto& context = renderer_->GetContext();

		//TODO: new command list should be moved to Renderer::BeginFrame
		auto cmd_list = renderer_->GetCommands().GetNewCommandList();
		context.SetCommandList(cmd_list);

		//TODO: improve me...
		{
			auto bb_barier = CD3DX12_RESOURCE_BARRIER::Transition
			(
				swapchain->GetBackBufferResource(),
				D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATE_RENDER_TARGET
			);
			cmd_list->ResourceBarrier(1, &bb_barier);

			cmd_list->OMSetRenderTargets
			(
				1,
				swapchain->GetBackBufferHandle(),
				TRUE,
				NULL
			);
			
			float clear_color[] = { 0.1f, 0.1f, 0.1f, 1.0f };
			cmd_list->ClearRenderTargetView
			(
				*swapchain->GetBackBufferHandle(),
				clear_color,
				0,
				NULL
			);

			//TODO:
			//RSSetViewports
			//RSSetScissorRects
		}

		////////////////////////////////////////////////////////////////////////////////

		::ImGui_ImplWin32_NewFrame();
		::ImGui::NewFrame();
	}

	void Interface::RenderFrame()
	{
		auto& context = renderer_->GetContext();
		
		ImGui::ShowDemoWindow();
		ImGui::Render();

		auto draw_data = ImGui::GetDrawData();
		assert(draw_data != nullptr);

		if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
		{
			return;
		}

		////////////////////////////////////////////////////////////////////////////////

		Engine::BufferDesc vb_desc;
		vb_desc.name_ = "EditorInterfaceVB";
		vb_desc.size_ = draw_data->TotalVtxCount * sizeof(ImDrawVert);
		vb_desc.stride_ = sizeof(ImDrawVert);
		vb_desc.usage_ = Engine::ResourceUsage::Dynamic;
		vb_desc.bind_ = Engine::ResourceBind::VertexBuffer;

		auto vb = renderer_->CreateBuffer(vb_desc, nullptr);

		Engine::BufferDesc ib_desc;
		ib_desc.name_ = "EditorInterfaceIB";
		ib_desc.size_ = draw_data->TotalIdxCount * sizeof(ImDrawIdx);
		ib_desc.stride_ = sizeof(ImDrawIdx);
		ib_desc.usage_ = Engine::ResourceUsage::Dynamic;
		ib_desc.bind_ = Engine::ResourceBind::IndexBuffer;

		auto ib = renderer_->CreateBuffer(ib_desc, nullptr);

		////////////////////////////////////////////////////////////////////////////////
		
		{
			ImDrawVert* vtx_dst = (ImDrawVert*)vb->GetAddressCPU();
			ImDrawIdx* idx_dst = (ImDrawIdx*)ib->GetAddressCPU();
		
			for (int list_index = 0; list_index < draw_data->CmdListsCount; ++list_index)
			{
				const auto cmd_list = draw_data->CmdLists[list_index];
			
				assert(cmd_list->VtxBuffer.Data != nullptr);
				assert(cmd_list->VtxBuffer.Size > 0);
			
				std::memcpy
				(
					vtx_dst,
					cmd_list->VtxBuffer.Data,
					cmd_list->VtxBuffer.Size * sizeof(ImDrawVert)
				);
			
				assert(cmd_list->IdxBuffer.Data != nullptr);
				assert(cmd_list->IdxBuffer.Size > 0);
			
				std::memcpy
				(
					idx_dst,
					cmd_list->IdxBuffer.Data,
					cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx)
				);
			
				vtx_dst += cmd_list->VtxBuffer.Size;
				idx_dst += cmd_list->IdxBuffer.Size;
			}
		}

		////////////////////////////////////////////////////////////////////////////////

		Engine::BufferDesc buffer_desc;
		buffer_desc.name_ = "Constants";
		buffer_desc.size_ = sizeof(XMFLOAT4X4);
		buffer_desc.stride_ = sizeof(XMFLOAT4X4);
		buffer_desc.usage_ = Engine::ResourceUsage::Dynamic;
		buffer_desc.bind_ = Engine::ResourceBind::UniformBuffer;
		
		auto vs_constants = renderer_->CreateBuffer(buffer_desc, nullptr);
		
		//pipeline_->Bind(vs_constants_);

		{
			XMFLOAT4X4* cb_constants = (XMFLOAT4X4*)vs_constants->GetAddressCPU();
		
			float L = draw_data->DisplayPos.x;
			float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
			float T = draw_data->DisplayPos.y;
			float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
		
			*cb_constants = XMFLOAT4X4
			(
				2.0f / (R - L), 0.0f, 0.0f, 0.0f,
				0.0f, 2.0f / (T - B), 0.0f, 0.0f,
				0.0f, 0.0f, 0.5f, 0.0f,
				(R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f
			);
		}

		////////////////////////////////////////////////////////////////////////////////

		context.SetPipeline(pipeline_);

		//HERE HERE HERE
		//TODO: improve me...
		auto test = std::make_pair<std::shared_ptr<Alpha::Engine::Buffer>, std::size_t>(std::move(vb), (std::size_t)0);
		std::vector<Alpha::Engine::VertexStream> test2 = { test };
		context.SetVertexBuffer(0, test2, true);
		context.SetIndexBuffer(ib, 0u);
		
		context.CommitShaderResources();
		context.CommitConstantBuffers(vs_constants);
		
		const float blend_factors[4] = { 0.f, 0.f, 0.f, 0.f };
		context.SetBlendFactors(blend_factors);
		
		std::vector<D3D12_VIEWPORT> viewports;
		viewports.emplace_back(CD3DX12_VIEWPORT(0.0f, 0.0f, (float)draw_data->DisplaySize.x, (float)draw_data->DisplaySize.y));	
		context.SetViewports(viewports);

		////////////////////////////////////////////////////////////////////////////////

		int vtx_offset = 0;
		int idx_offset = 0;

		ImVec2 clip = draw_data->DisplayPos;

		for (int list_index = 0; list_index < draw_data->CmdListsCount; ++list_index)
		{
			const auto cmd_list = draw_data->CmdLists[list_index];

			for (int cmd_index = 0; cmd_index < cmd_list->CmdBuffer.Size; ++cmd_index)
			{
				const auto cmd = &cmd_list->CmdBuffer[cmd_index];

				if (cmd->UserCallback != nullptr)
				{
					//TODO: something useful...
					assert(false);
				}
				else
				{
					auto scisors = CD3DX12_RECT
					(
						(LONG)(cmd->ClipRect.x - clip.x),
						(LONG)(cmd->ClipRect.y - clip.y),
						(LONG)(cmd->ClipRect.z - clip.x),
						(LONG)(cmd->ClipRect.w - clip.y)
					);
					
					context.SetScissorRects(1, &scisors);
						
					auto* srv = reinterpret_cast<Engine::TextureView*>(cmd->TextureId);
					assert(srv == &fonts_srv_);
						
					Engine::DrawIndexedAttributes draw;
					draw.indices_ = cmd->ElemCount;
					draw.first_index_index_ = cmd->IdxOffset + idx_offset;
					draw.base_vertex_ = cmd->VtxOffset + vtx_offset;
					context.DrawIndexed(draw);
				}
			}

			vtx_offset += cmd_list->VtxBuffer.Size;
			idx_offset += cmd_list->IdxBuffer.Size;
		}

		////////////////////////////////////////////////////////////////////////////////
	}

	void Interface::EndFrame(std::shared_ptr<Engine::SwapChain>& swapchain)
	{
		auto& context = renderer_->GetContext();
		context.EndFrame();

		auto cmd_list = renderer_->GetCommands().GetNewCommandList();

		auto bb_barier = CD3DX12_RESOURCE_BARRIER::Transition
		(
			swapchain->GetBackBufferResource(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		);

		cmd_list->ResourceBarrier(1, &bb_barier);
		
		auto result = cmd_list->Close();
		Engine::ThrowIfFailed(result, "[Context::Flush] ID3D12GraphicsCommandList::Close failed");

		ID3D12CommandList* const lists[] = { cmd_list };
		renderer_->GetGraphicsQueue()->ExecuteCommandLists(1, lists);

		////////////////////////////////////////////////////////////////////////////////

		swapchain->Present();
		swapchain->Wait(renderer_->GetFrame());
		renderer_->EndFrame();
	}
}