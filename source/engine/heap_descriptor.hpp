#pragma once

namespace Alpha::Engine
{
	class Renderer;

	constexpr std::uint32_t cHeapDescriptor_GenericSize = 0x3000;
	constexpr std::uint32_t cHeapDescriptor_SamplerSize = 0x10;
	constexpr std::uint32_t cHeapDescriptor_RTVSize = 0x20;
	constexpr std::uint32_t cHeapDescriptor_DSVSize = 0x20;

	////////////////////////////////////////////////////////////////////////////////

	struct DescriptorHeaps
	{
		ID3D12DescriptorHeap* generic_ = nullptr;
		ID3D12DescriptorHeap* sampler_ = nullptr;

		DescriptorHeaps() = default;

		explicit DescriptorHeaps(ID3D12DescriptorHeap* generic, ID3D12DescriptorHeap* sampler) noexcept
			: generic_{ generic }
			, sampler_{ sampler }
		{
		}

		bool operator==(const DescriptorHeaps& rhs) const
		{
			return generic_ == rhs.generic_ && sampler_ == rhs.sampler_;
		}
	};

	////////////////////////////////////////////////////////////////////////////////

	struct HeapDescriptorDesc final : public ObjectDesc
	{
	};

	//TODO: template
	class HeapDescriptor final : public Object<HeapDescriptorDesc>
	{
		std::uint32_t index_ = 0;
		const std::uint32_t count_ = 0;
		const std::uint32_t size_ = 0;

		ComPtr<ID3D12DescriptorHeap> heap_;
	public:
		//TODO: pair <count, type>
		HeapDescriptor
		(
			Renderer* renderer,
			std::uint32_t count,
			D3D12_DESCRIPTOR_HEAP_TYPE type
		);

		////////////////////////////////////////////////////////////////////////////////

		struct Element
		{
			D3D12_CPU_DESCRIPTOR_HANDLE cpu_ = { };
			D3D12_GPU_DESCRIPTOR_HANDLE gpu_ = { };

			std::uint32_t count_ = 0;
			std::uint32_t size_ = 0;

			ID3D12DescriptorHeap* heap_ = nullptr;

			////////////////////////////////////////////////////////////////////////////////

			bool IsValid() const noexcept
			{
				return heap_ && size_ != 0 && count_ != 0;
			}

			D3D12_CPU_DESCRIPTOR_HANDLE GetHandleCPU(std::uint64_t offest) const noexcept
			{
				assert(heap_ != nullptr);
				assert(cpu_.ptr != 0);
				assert(IsValid());

				auto offset = size_ * offest;
				return { cpu_.ptr + offset };
			}

			D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU(std::uint64_t offest) const noexcept
			{
				assert(heap_ != nullptr);
				assert(gpu_.ptr != 0);
				assert(IsValid());

				auto offset = size_ * offest;
				return { gpu_.ptr + offset };
			}

			ID3D12DescriptorHeap* GetDescriptorHeap() noexcept
			{
				return heap_;
			}
		};

		////////////////////////////////////////////////////////////////////////////////

		HeapDescriptor::Element Allocate(std::uint32_t count);
	};
}