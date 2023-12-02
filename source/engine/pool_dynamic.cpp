#include "_engine.hpp"

#include "renderer.hpp"

namespace Alpha::Engine
{
	//TODO: alling up
	PoolDynamic::PoolDynamic(Renderer* renderer, std::uint32_t size)
		: Object(renderer, {})
		, ring_ { size }
	{
		HRESULT result;
	
		auto heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto buffer = CD3DX12_RESOURCE_DESC::Buffer(size);
	
		result = GetDevice()->CreateCommittedResource
		(
			&heap,
			D3D12_HEAP_FLAG_NONE,
			&buffer,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf())
		);
	
		ThrowIfFailed(result, "PoolDynamic::CreateCommittedResource failed");

		//TODO: SetName...

		result = resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
		ThrowIfFailed(result, "PoolDynamic::Map failed");
	}

	////////////////////////////////////////////////////////////////////////////////

	void PoolDynamic::BeginFrame(Frame frame)
	{
		auto index = frame % cEngine_BackBufferCount;

		bb_size_[index] = size_;

		auto to_free = bb_size_[(frame + 1) % cEngine_BackBufferCount];
		
		if (false == ring_.Free(to_free))
		{
			throw std::runtime_error("[PoolDynamic::BeginFrame] Ring::Free failed");
		}

		size_ = 0;
	}

	PoolDynamic::Allocation PoolDynamic::Allocate(std::uint32_t size)
    {
		PoolDynamic::Allocation allocation = {};

		//TODO: check it...
		size = Core::AlignUp(size, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

		if (auto padding = ring_.Padding(size); padding != 0) [[unlikely]]
		{
			size_ += padding;

			if (false == ring_.Allocate(padding, nullptr)) [[unlikely]]
			{
				throw std::runtime_error("PoolDynamic::BeginFrame");
				return allocation;
			}
		}
		
		if (std::uint32_t offset; ring_.Allocate(size, &offset)) [[likely]]
		{
			size_ += size;

			//allocation.first = static_cast<byte*>(cpu_) + offset;
			//allocation.second = gpu_ + offset;
			allocation.first = (void*)(data_ + offset);
			allocation.second = resource_->GetGPUVirtualAddress() + offset;
		}

		return allocation;
    }
}

