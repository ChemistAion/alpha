#include "_engine.hpp"

#include "renderer.hpp"

namespace Alpha::Engine
{
	HeapUpload::HeapUpload(Renderer* renderer, std::size_t size)
		: Object{ renderer, {} }
	{
		HRESULT result;

		result = GetDevice()->CreateCommandAllocator
		(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(cmd_allocator_.ReleaseAndGetAddressOf())
		);

		ThrowIfFailed(result,"HeapUpload::HeapUpload failed on CreateCommandAllocator");
		
		result = GetDevice()->CreateCommandList
		(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			cmd_allocator_.Get(),
			nullptr,
			IID_PPV_ARGS(cmd_list_.ReleaseAndGetAddressOf())
		);	

		ThrowIfFailed(result, "HeapUpload::HeapUpload failed on CreateCommandList");

		////////////////////////////////////////////////////////////////////////////////

		//TODO: node check
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

		ThrowIfFailed(result, "HeapUpload::HeapUpload failed on CreateCommittedResource");

		////////////////////////////////////////////////////////////////////////////////

		result = resource_->Map(0, nullptr, (void**)&begin_);
		ThrowIfFailed(result, "PoolDynamic::Map failed");

		current_ = begin_;
		end_ = begin_ + buffer.Width;
	}

	std::byte* HeapUpload::Allocate(std::size_t size, std::uint64_t alignment)
	{
		flush_.Wait();

		std::byte* result = nullptr;

		{
			std::unique_lock<std::mutex> lock(lock_);

			assert(size < (size_t)(end_ - begin_));

			current_ = reinterpret_cast<std::byte*>(Core::AlignUp(reinterpret_cast<std::size_t>(current_), std::size_t(alignment)));

			if (current_ >= end_ || current_ + size >= end_)
			{
				return nullptr;
			}

			result = current_;
			current_ += size;
		}

		return result;
	}

	void HeapUpload::Flush()
	{
		flush_.Wait();
		flush_.Inc();
		allocate_.Wait();

		std::unique_lock<std::mutex> lock(lock_);

		//TODO: stuff...

		ThrowIfFailed(cmd_list_->Close(), "HeapUpload::FlushAndFinish failed on ID3D12GraphicsCommandList::Close");

		GetRenderer()->GetGraphicsQueue()->ExecuteCommandLists(1, CommandListCast(cmd_list_.GetAddressOf()));

		GetRenderer()->Flush(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		GetRenderer()->Flush(D3D12_COMMAND_LIST_TYPE_DIRECT);

		cmd_allocator_->Reset();
		cmd_list_->Reset(cmd_allocator_.Get(), nullptr);

		current_ = begin_;

		flush_.Dec();
	}
}