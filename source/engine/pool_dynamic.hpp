#pragma once

namespace Alpha::Engine
{
	class Renderer;

	constexpr std::uint32_t cPoolDynamic_MemorySize = 1u << 24u;

	////////////////////////////////////////////////////////////////////////////////

	struct PoolDynamicDesc final : public ObjectDesc
	{
	};

	class PoolDynamic final : public Object<PoolDynamicDesc>
	{
		ComPtr<ID3D12Resource> resource_;
		std::byte* data_ = nullptr;

		Core::Ring<std::uint32_t> ring_;
		std::uint32_t size_ = 0;
		std::uint32_t bb_size_[cEngine_BackBufferCount] = { 0 };
	public:
		PoolDynamic(Renderer* renderer,	std::uint32_t size);

//		~PoolDynamic()
//		{
//			//unmap? release?
//			ring_.Free(ring_.GetSize());
//		}

		////////////////////////////////////////////////////////////////////////////////

		void BeginFrame(Frame frame);

		//TODO: check if both necessary 
		using Allocation = std::pair<D3D12_CPU_VIRTUAL_ADDRESS, D3D12_GPU_VIRTUAL_ADDRESS>;
		Allocation Allocate(std::uint32_t size);
	};
}