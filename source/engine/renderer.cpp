#include "_engine.hpp"

#include "renderer.hpp"
#include "buffer.hpp"
#include "texture.hpp"
#include "pipeline.hpp"
#include "swapchain.hpp"

namespace Alpha::Engine
{
	//TODO: "this" is forbidden in ctor
	//	Renderer::Renderer(Adapter& adapter)
	Renderer::Renderer()
		: Device()
		, heaps_
		{
			{ this, cHeapDescriptor_GenericSize, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV },
			{ this, cHeapDescriptor_SamplerSize, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER },
			{ this, cHeapDescriptor_RTVSize, D3D12_DESCRIPTOR_HEAP_TYPE_RTV },
			{ this, cHeapDescriptor_DSVSize, D3D12_DESCRIPTOR_HEAP_TYPE_DSV }
		}
		, commands_ { this, cEngine_BackBufferCount, 8u, GetGraphicsQueue()->GetDesc() }
		, upload_ { this, cHeapUpload_Size }
		, context_ { this }
		, frame_ { 0 }
	{
	}

	////////////////////////////////////////////////////////////////////////////////

    void Renderer::BeginFrame()
    {
		try
		{
			commands_.BeginFrame(frame_);
			context_.BeginFrame(frame_);
		}
		catch (const _com_error& error)
		{
			auto logger = Core::Logger::GetInstance();

			std::wstring error_message(error.ErrorMessage());
			logger->Critical("[Renderer::BeginFrame]: " + std::string(error_message.cbegin(), error_message.cend()));
		}
		catch (const std::runtime_error& error)
		{
			auto logger = Core::Logger::GetInstance();
			logger->Critical("[Renderer::BeginFrame]: " + std::string(error.what()));
		}
		catch (...)
		{
			auto logger = Core::Logger::GetInstance();
			logger->Critical("[Renderer::BeginFrame]: Unhandled exception");
		}
    }
}
