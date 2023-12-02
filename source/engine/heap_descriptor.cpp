#include "_engine.hpp"

#include "renderer.hpp"

namespace Alpha::Engine
{
	HeapDescriptor::HeapDescriptor
	(
		Renderer* renderer,
		std::uint32_t count,
		D3D12_DESCRIPTOR_HEAP_TYPE type
	)
		: Object(renderer, {})
		, count_ { count }
		, size_ { GetDevice()->GetDescriptorHandleIncrementSize(type) }
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc;
		desc.Type = type;
		desc.NumDescriptors = count;
		desc.Flags = ((type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV) || (type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV)) 
			? D3D12_DESCRIPTOR_HEAP_FLAG_NONE
			: D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		desc.NodeMask = GetNode();

		auto result = GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(heap_.ReleaseAndGetAddressOf()));

		//TODO: throw...
		assert(SUCCEEDED(result));
	}

	HeapDescriptor::Element HeapDescriptor::Allocate(std::uint32_t count)
    {
		assert(heap_);

		Element element;

		if ((index_ + count) > count_)
		{
			assert(false);
			return element;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE handle_cpu = heap_->GetCPUDescriptorHandleForHeapStart();
		handle_cpu.ptr += index_ * size_;

		D3D12_GPU_DESCRIPTOR_HANDLE handle_gpu = heap_->GetGPUDescriptorHandleForHeapStart();
		handle_gpu.ptr += index_ * size_;

		index_ += count;

		element.cpu_ = handle_cpu;
		element.gpu_ = handle_gpu;
		element.count_ = count;
		element.size_ = size_;
		element.heap_ = heap_.Get();

		return element;
    }
}