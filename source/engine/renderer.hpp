#pragma once

#include "fence.hpp"
#include "device.hpp"
#include "buffer.hpp"
#include "texture.hpp"
#include "pipeline.hpp"
#include "commands.hpp"
#include "swapchain.hpp"

namespace Alpha::Engine
{
	class Pipeline;
	class SwapChain;
	struct SwapChainDesc;
	struct PipelineLayoutDesc;

	class Renderer final : public Device
	{
		Commands commands_;

		HeapDescriptor heaps_[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
		HeapUpload upload_;
		
		Context context_;	

		Frame frame_;

//		//TODO: change to concept with args...
//		//template <typename Object, typename ObjectDesc, typename ObjectCtor>
//		//void CreateObject(const char* name, const ObjectDesc& desc, std::shared_ptr<Object>& object, ObjectCtor construct)
		template <typename Construct>
		auto CreateObject(Construct construct)
		{
			try
			{
				return construct();
			}
		
			//TODO: extend this
			catch (const std::runtime_error& error)
			{
				auto logger = Core::Logger::GetInstance();
				logger->Critical("[Renderer::CreateObject]: " + std::string(error.what()));
			}
		}
	public:
		//TODO: explicit Renderer(Adapter& adapter);
		Renderer();

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		Renderer(Renderer&&) = delete;
		Renderer& operator=(Renderer&&) = delete;

		////////////////////////////////////////////////////////////////////////////////

		std::shared_ptr<Buffer> CreateBuffer(const BufferDesc& desc, const BufferData* data)
		{
			std::shared_ptr<Buffer> buffer;
		
			CreateObject([&]() { buffer = std::make_shared<Buffer>(this, desc, data); });
			return buffer;
		}
		
		std::shared_ptr<Texture> CreateTexture(const TextureDesc& desc, const TextureData* data)
		{
			std::shared_ptr<Texture> texture;
		
			CreateObject([&]() { texture = std::make_shared<Texture>(this, desc, data); });
			return texture;
		}
		
		//std::shared_ptr<Texture> CreateTexture(const TextureDesc& desc, ID3D12Resource* resource, D3D12_RESOURCE_STATES state);
		
		std::shared_ptr<Shader> CreateShader(const ShaderDesc& desc)
		{
			std::shared_ptr<Shader> shader;
		
			CreateObject([&]() { shader = std::make_shared<Shader>(this, desc); });
			return shader;
		}
		
		std::shared_ptr<Pipeline> CreatePipeline(const PipelineDesc& desc)
		{
			std::shared_ptr<Pipeline> pipeline;
		
			CreateObject([&]() { pipeline = std::make_shared<Pipeline>(this, desc); });	
			return pipeline;
		}

		std::shared_ptr<SwapChain> CreateSwapChain(const SwapChainDesc& desc)
		{
			std::shared_ptr<SwapChain> swapchain;

			CreateObject([&]() { swapchain = std::make_shared<SwapChain>(this, desc); });
			return swapchain;
		}

		////////////////////////////////////////////////////////////////////////////////

		Context& GetContext()
		{
			return context_;
		}

		Commands& GetCommands()
		{
			return commands_;
		}

		HeapUpload* GetHeapUpload()
		{
			return &upload_;
		}

		////////////////////////////////////////////////////////////////////////////////

		HeapDescriptor::Element AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE heap_type, std::uint32_t count)
		{
			assert(heap_type >= D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV && heap_type < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES);
			return heaps_[heap_type].Allocate(count);
		}

		void BeginFrame();

		void EndFrame()
		{
			++frame_;
		}

		Frame GetFrame() const noexcept
		{
			return frame_;
		}
	};
}
