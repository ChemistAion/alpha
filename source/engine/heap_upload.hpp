#pragma once

namespace Alpha::Engine
{
	class Renderer;

	constexpr std::uint32_t cHeapUpload_Size = 0x40000000;

	struct HeapUploadDesc final : public ObjectDesc
	{
	};

	class HeapUpload final : public Object<HeapUploadDesc>
	{
		Core::Synchron allocate_, flush_;

		std::byte* begin_ = nullptr;
		std::byte* end_ = nullptr;
		std::byte* current_ = nullptr;

		std::uint64_t fence_value_ = 0;

		ComPtr<ID3D12Resource> resource_;

		ComPtr<ID3D12CommandAllocator> cmd_allocator_;
		ComPtr<ID3D12GraphicsCommandList> cmd_list_;

		ComPtr<ID3D12Fence> fence_;
		HANDLE event_ = INVALID_HANDLE_VALUE;

		std::mutex lock_;
	public:
		HeapUpload
		(
			Renderer* renderer,
			std::size_t size
		);

		std::byte* Allocate(std::size_t size, std::uint64_t alignment);

		std::byte* GetBasePointer() const noexcept
		{
			return begin_;
		}

		ID3D12GraphicsCommandList* GetCommandList()
		{
			return cmd_list_.Get();
		}

		ID3D12Resource* GetResource()
		{
			return resource_.Get();
		}

		////////////////////////////////////////////////////////////////////////////////

		void Flush();
	};
}