#pragma once

#include "texture.hpp"
#include "renderer.hpp"
#include "fence.hpp"

namespace Alpha::Engine
{
	class Window;
	class Renderer;
	class Context;

	struct SwapChainDesc final : public ObjectDesc
	{
		std::uint32_t width_ = 0;
		std::uint32_t height_ = 0;

		DXGI_FORMAT format_ = DXGI_FORMAT_R8G8B8A8_UNORM;

		float depth_ = 1.0f;
		std::uint8_t stencil_ = 0;
		
		std::shared_ptr<Core::Window> window_;
	};

	class SwapChain final : public Object<SwapChainDesc>
	{
		ComPtr<IDXGISwapChain4> swapchain_;
		ComPtr<IDXGIFactory6> factory_;
		ComPtr<ID3D12DescriptorHeap> heap_;

		Fence fence_;

		std::array<D3D12_CPU_DESCRIPTOR_HANDLE, cEngine_BackBufferCount> bb_view_;
		std::shared_ptr<Core::Window> window_;

		void Create(ID3D12CommandQueue* queue);
		void InitializeResources();

		//STATE
		std::uint32_t width_ = 0;
		std::uint32_t height_ = 0;
		DXGI_FORMAT format_ = DXGI_FORMAT_R8G8B8A8_UNORM;
	public:
		explicit SwapChain(Renderer* renderer, const SwapChainDesc& desc);

		SwapChain(const SwapChain&) = delete;
		SwapChain& operator=(const SwapChain&) = delete;

		SwapChain(SwapChain&&) = default;
		SwapChain& operator=(SwapChain&&) = default;

		////////////////////////////////////////////////////////////////////////////////

		void Resize(std::uint32_t width, std::uint32_t height);

		UINT GetBackBufferIndex() const noexcept
		{
			assert(swapchain_);
			return swapchain_->GetCurrentBackBufferIndex();
		}

		ID3D12Resource* GetBackBufferResource()
		{
			ComPtr<ID3D12Resource> resource;
			
			auto index = GetBackBufferIndex();

			auto result = swapchain_->GetBuffer(index, IID_PPV_ARGS(resource.ReleaseAndGetAddressOf()));
			ThrowIfFailed(result, "[SwapChain::GetBackBufferResource] GetBuffer failed");

			return resource.Get();
		}

		D3D12_CPU_DESCRIPTOR_HANDLE* GetBackBufferHandle()
		{
			auto index = GetBackBufferIndex();

			return &bb_view_[index];
		}

		void Present();

		void Wait(Frame frame);

		auto GetWidth() const noexcept
		{
			return width_;
		}

		auto GetHeight() const noexcept
		{
			return height_;
		}

	};
}
