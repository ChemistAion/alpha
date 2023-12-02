#include "_root.hpp"

#include "imgui/backends/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Alpha::Root::Component
{
	void Editor::Create(const std::shared_ptr<Alpha::Engine::Renderer>& renderer)
	{
		::IMGUI_CHECKVERSION();

		//TODO: cleanup this...
		//::ImGui::CreateContext();
		auto imgui_context = ::ImGui::CreateContext();
		::ImGui::SetCurrentContext(imgui_context);

		ImGuiIO& io = ImGui::GetIO();
		io.BackendRendererName = "madX Alpha";
		io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
		//io.IniFilename = nullptr;

		////////////////////////////////////////////////////////////////////////////////

		Alpha::Engine::PipelineDesc pipeline_desc;

		pipeline_desc.name_ = "Editor";

		//TODO: fixed!
		pipeline_desc.RT_count_ = 1;
		pipeline_desc.RTV_format[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		pipeline_desc.DSV_format = DXGI_FORMAT_UNKNOWN;
		pipeline_desc.rasterizer_state_.CullMode = D3D12_CULL_MODE_NONE;
		pipeline_desc.depth_stencil_state_.DepthEnable = FALSE;

		////////////////////////////////////////////////////////////////////////////////

		{
			Alpha::Engine::ShaderDesc shader_desc;

			shader_desc.name_ = "EditorVS";
			shader_desc.type_ = Alpha::Engine::ShaderType::Vertex;
			shader_desc.file_ = "shaders\\editor.vsh";
			shader_desc.entry_ = L"main";
			//TODO: template for automatic creation based on <shader_desc>
			pipeline_desc.shaders_[shader_desc.type_] = renderer->CreateShader(shader_desc);

			shader_desc.name_ = "EditorPS";
			shader_desc.type_ = Alpha::Engine::ShaderType::Pixel;
			shader_desc.file_ = "shaders\\editor.psh";
			shader_desc.entry_ = L"main";
			//TODO: template for automatic creation based on <shader_desc>
			pipeline_desc.shaders_[shader_desc.type_] = renderer->CreateShader(shader_desc);
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

		pipeline_desc.input_layout_.push_back(
		  { "ATTRIB", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

		pipeline_desc.input_layout_.push_back(
		  { "ATTRIB", 1, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

		pipeline_desc.input_layout_.push_back(
		  { "ATTRIB", 2, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

		pipeline_ = renderer->CreatePipeline(pipeline_desc);

		////////////////////////////////////////////////////////////////////////////////

		{
			ImGuiIO& io = ImGui::GetIO();

			unsigned char* fonts_pixels = nullptr;
			int fonts_width = 0;
			int fonts_height = 0;
			io.Fonts->GetTexDataAsRGBA32(&fonts_pixels, &fonts_width, &fonts_height);

			Alpha::Engine::TextureDesc texture_desc;
			texture_desc.name_ = "Texture";
			texture_desc.width_ = fonts_width;
			texture_desc.height_ = fonts_height;
			texture_desc.dimension_ = Alpha::Engine::ResourceDimension::Texture2D;
			texture_desc.format_ = DXGI_FORMAT_R8G8B8A8_UNORM;
			texture_desc.usage_ = Alpha::Engine::ResourceUsage::Static;
			texture_desc.bind_ = Alpha::Engine::ResourceBind::ShaderResource;

			Alpha::Engine::TextureResourceData fonts_mip[] = { { fonts_pixels, (std::uint32_t)(fonts_width * 4), 0 } };

			Alpha::Engine::TextureData fonts_data(fonts_mip, _countof(fonts_mip));

			fonts_ = std::make_shared<Fonts>();
			fonts_->texture_ = renderer->CreateTexture(texture_desc, &fonts_data);

			Alpha::Engine::TextureViewDesc view_desc;
			view_desc.type_ = Alpha::Engine::ResourceView::ShaderResource;

			fonts_->srv_ = fonts_->texture_->CreateView(view_desc);
			pipeline_->Bind(fonts_->srv_);
			io.Fonts->TexID = (ImTextureID)&fonts_->srv_;
		}

		////////////////////////////////////////////////////////////////////////////////

		auto window = Alpha::Core::Window::GetInstance();
		window->RegisterPump(::ImGui_ImplWin32_WndProcHandler);
		::ImGui_ImplWin32_Init(window->GetHandle());
		::ImGui::StyleColorsDark();
	}

	void Editor::Destroy()
	{
		::ImGui_ImplWin32_Shutdown();
		::ImGui::DestroyContext();

		fonts_.reset();
		pipeline_.reset();
	}


	void Editor::Begin(const std::shared_ptr<Alpha::Engine::Renderer>& renderer)
	{
		::ImGui_ImplWin32_NewFrame();
		::ImGui::NewFrame();
	}

	void Editor::Render(const std::shared_ptr<Alpha::Engine::Renderer>& renderer)
	{
		auto& context = renderer->GetContext();

		ImGui::Render();

		auto draw_data = ImGui::GetDrawData();
		assert(draw_data != nullptr);

		if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
		{
			return;
		}

		////////////////////////////////////////////////////////////////////////////////

		Alpha::Engine::BufferDesc vb_desc;
		vb_desc.name_ = "EditorInterfaceVB";
		vb_desc.size_ = draw_data->TotalVtxCount * sizeof(ImDrawVert);
		vb_desc.stride_ = sizeof(ImDrawVert);
		vb_desc.usage_ = Alpha::Engine::ResourceUsage::Dynamic;
		vb_desc.bind_ = Alpha::Engine::ResourceBind::VertexBuffer;

		auto vb = renderer->CreateBuffer(vb_desc, nullptr);

		Alpha::Engine::BufferDesc ib_desc;
		ib_desc.name_ = "EditorInterfaceIB";
		ib_desc.size_ = draw_data->TotalIdxCount * sizeof(ImDrawIdx);
		ib_desc.stride_ = sizeof(ImDrawIdx);
		ib_desc.usage_ = Alpha::Engine::ResourceUsage::Dynamic;
		ib_desc.bind_ = Alpha::Engine::ResourceBind::IndexBuffer;

		auto ib = renderer->CreateBuffer(ib_desc, nullptr);

		////////////////////////////////////////////////////////////////////////////////

		{
			ImDrawVert* vtx_dst = (ImDrawVert*)vb->GetAddressCPU();
			ImDrawIdx* idx_dst = (ImDrawIdx*)ib->GetAddressCPU();

			for (int list_index = 0; list_index < draw_data->CmdListsCount; ++list_index)
			{
				const auto cmd_list = draw_data->CmdLists[list_index];

				assert(cmd_list->VtxBuffer.Data != nullptr);
				assert(cmd_list->VtxBuffer.Size > 0);

				std::memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));

				assert(cmd_list->IdxBuffer.Data != nullptr);
				assert(cmd_list->IdxBuffer.Size > 0);

				std::memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));

				vtx_dst += cmd_list->VtxBuffer.Size;
				idx_dst += cmd_list->IdxBuffer.Size;
			}
		}

		////////////////////////////////////////////////////////////////////////////////

		Alpha::Engine::BufferDesc buffer_desc;
		buffer_desc.name_ = "Constants";
		buffer_desc.size_ = sizeof(XMFLOAT4X4);
		buffer_desc.stride_ = sizeof(XMFLOAT4X4);
		buffer_desc.usage_ = Alpha::Engine::ResourceUsage::Dynamic;
		buffer_desc.bind_ = Alpha::Engine::ResourceBind::UniformBuffer;

		auto vs_constants = renderer->CreateBuffer(buffer_desc, nullptr);

		//pipeline_->Bind(vs_constants_);

		{
			XMFLOAT4X4* cb_constants = (XMFLOAT4X4*)vs_constants->GetAddressCPU();

			float L = draw_data->DisplayPos.x;
			float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
			float T = draw_data->DisplayPos.y;
			float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;

			*cb_constants = XMFLOAT4X4(2.0f / (R - L), 0.0f, 0.0f, 0.0f, 0.0f, 2.0f / (T - B), 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f,
			  (R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f);
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
					auto scisors = CD3DX12_RECT((LONG)(cmd->ClipRect.x - clip.x), (LONG)(cmd->ClipRect.y - clip.y),
					  (LONG)(cmd->ClipRect.z - clip.x), (LONG)(cmd->ClipRect.w - clip.y));

					context.SetScissorRects(1, &scisors);

					auto* srv = reinterpret_cast<Alpha::Engine::TextureView*>(cmd->TextureId);
					assert(srv == &fonts_->srv_);

					Alpha::Engine::DrawIndexedAttributes draw;
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

	void Editor::End(const std::shared_ptr<Alpha::Engine::Renderer>& renderer)
	{
		return;
	}
}

