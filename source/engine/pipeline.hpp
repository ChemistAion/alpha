#pragma once

#include "shader.hpp"
#include "context.hpp"
#include "signature.hpp"

namespace Alpha::Engine
{
	class Renderer;

	////////////////////////////////////////////////////////////////////////////////

	struct PipelineDesc final : public ObjectDesc
	{	
		std::map<ShaderType, std::shared_ptr<Shader>> shaders_;
		
		D3D12_BLEND_DESC blend_state_ = CD3DX12_BLEND_DESC();
		UINT sample_mask_ = ::DefaultSampleMask();
		
		D3D12_RASTERIZER_DESC rasterizer_state_ = CD3DX12_RASTERIZER_DESC(CD3DX12_DEFAULT());
		D3D12_DEPTH_STENCIL_DESC depth_stencil_state_ = CD3DX12_DEPTH_STENCIL_DESC(CD3DX12_DEFAULT());

		std::vector<D3D12_INPUT_ELEMENT_DESC> input_layout_;

		//TODO: D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED
		D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive_topology_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		
		//TODO: as vector
		//TODO: as struct: count and std::array<DXGI_FORMAT>[8] D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT
		UINT RT_count_ = 0;
		
		DXGI_FORMAT RTV_format[8] = { DXGI_FORMAT_UNKNOWN };
		DXGI_FORMAT DSV_format = DXGI_FORMAT_UNKNOWN;
		
		//TODO: check it
		DXGI_SAMPLE_DESC sample_desc_ = ::DefaultSampleDesc();
	};

	////////////////////////////////////////////////////////////////////////////////

	class Buffer;
	class Texture;
	class ContextOld;
	class CommandContextOld;

	class Pipeline final : public Object<PipelineDesc>
	{
		ComPtr<ID3D12PipelineState> state_;
		Signature signature_;
	public:
		explicit Pipeline(Renderer* renderer, const PipelineDesc& desc);

		auto GetPipelineState() const
		{
			return state_.Get();
		}

		ID3D12RootSignature* GetRootSignature() const noexcept
		{
			return signature_.GetRootSignature();
		}

		Signature& GetSignature()
		{
			return signature_;
		}

		//void Bind(const std::shared_ptr<Buffer>& buffer);
		void Bind(TextureView& texture_view);

		//void EndFrame();

		void CommitShaderResources(Context* context);
		void CommitConstantBuffers(Context* context, const std::shared_ptr<Buffer>& buffer);
	};
}