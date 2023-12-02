#include "_engine.hpp"

#include "pipeline.hpp"
#include "renderer.hpp"
#include "buffer.hpp"
#include "texture.hpp"

namespace Alpha::Engine
{
	//void Pipeline::Bind(const std::shared_ptr<Buffer>& buffer)
	//{
	//	const auto& desc = buffer->GetDesc();
	//	
	//	auto attributes = signature_.GetAttributes(GetName());
	//	assert(attributes.IsValid());
	//	
	//	cbv_cache_.emplace_back(attributes.index_, buffer.get());
	//}

	void Pipeline::Bind(TextureView& texture_view)
	{
		auto texture = texture_view.GetTexture();
		
		auto attributes = signature_.GetAttributes(texture->GetName());
		assert(attributes.IsValid());

		D3D12_CPU_DESCRIPTOR_HANDLE handle = signature_.GetDescriptorHandleCPU<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>(attributes, 0);

		GetDevice()->CreateShaderResourceView(texture->GetResource(), nullptr, handle);
	}

	void Pipeline::CommitShaderResources(Context* context)
	{
		assert(context != nullptr);
			
		auto command_list = context->GetCommandList();
		assert(command_list != nullptr);

		command_list->SetGraphicsRootSignature(signature_.GetRootSignature());

		signature_.CommitBindings();
		//CommitConstantBuffers(context);
	}

	void Pipeline::CommitConstantBuffers(Context* context, const std::shared_ptr<Buffer>& buffer)
	{
		assert(context != nullptr);

		auto command_list = context->GetCommandList();
		assert(command_list != nullptr);

		auto attributes = signature_.GetAttributes(buffer->GetName());
		assert(attributes.IsValid());

		auto gpu = buffer->GetAddressGPU();
		command_list->SetGraphicsRootConstantBufferView((UINT)attributes.index_, gpu);
	
		//for (const auto& cache : cbv_cache_)
		//{
		//	auto gpu = cache.second->GetAddressGPU(context);
		//	command_list->SetGraphicsRootConstantBufferView((UINT)cache.first, gpu);
		//}
	}

	////////////////////////////////////////////////////////////////////////////////

	Pipeline::Pipeline(Renderer* renderer, const PipelineDesc& desc)
		: Object(renderer, desc)
		, signature_(renderer)
	{
		Core::Logger::GetInstance()->Info("Creating Pipeline: " + GetName());

		for (const auto& shader : desc.shaders_)
		{
			signature_.AddShaderResources(shader.second);
		}
		
		signature_.Finalize();

		////////////////////////////////////////////////////////////////////////////////

		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphics_desc = {};

		graphics_desc.pRootSignature = signature_.GetRootSignature();

		//TODO: transform me...
		for (const auto& shader : desc.shaders_)
		{
			D3D12_SHADER_BYTECODE* code = nullptr;

			switch (shader.first)
			{
			case ShaderType::Vertex:
				code = &graphics_desc.VS;
				break;
			case ShaderType::Pixel:
				code = &graphics_desc.PS;
				break;
			case ShaderType::Geometry:
				code = &graphics_desc.GS;
				break;
			case ShaderType::Hull:
				code = &graphics_desc.HS;
				break;
			case ShaderType::Domain:
				code = &graphics_desc.DS;
				break;
			default:
				assert(false);
				break;
			}

			assert(code != nullptr);

			auto object = shader.second->GetShaderObject();

			assert(object != nullptr);

			code->pShaderBytecode = object->GetBufferPointer();
			code->BytecodeLength = object->GetBufferSize();
		}

		graphics_desc.StreamOutput = {};

		graphics_desc.BlendState = desc.blend_state_;
		graphics_desc.SampleMask = desc.sample_mask_;

		graphics_desc.RasterizerState = desc.rasterizer_state_;
		graphics_desc.DepthStencilState = desc.depth_stencil_state_;

		////////////////////////////////////////////////////////////////////////////////

		graphics_desc.InputLayout.NumElements = (UINT)desc.input_layout_.size();
		graphics_desc.InputLayout.pInputElementDescs = desc.input_layout_.data();

		////////////////////////////////////////////////////////////////////////////////

		graphics_desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		graphics_desc.PrimitiveTopologyType = desc.primitive_topology_;

		graphics_desc.NumRenderTargets = desc.RT_count_;

		//TODO: handle SwapChain RTV
		//TODO: std::vector me
		static_assert(sizeof(graphics_desc.RTVFormats) == sizeof(desc.RTV_format));
		std::memcpy(graphics_desc.RTVFormats, desc.RTV_format, sizeof(DXGI_FORMAT[8]));

		graphics_desc.DSVFormat = desc.DSV_format;
		graphics_desc.SampleDesc = desc.sample_desc_;
		graphics_desc.NodeMask = GetNode();
		graphics_desc.CachedPSO = {};
		graphics_desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		////////////////////////////////////////////////////////////////////////////////

		HRESULT result = GetDevice()->CreateGraphicsPipelineState
		(
			&graphics_desc,
			IID_PPV_ARGS(state_.ReleaseAndGetAddressOf())
		);

		ThrowIfFailed(result, "Failed to create GraphicsPipelineState");
	}
}