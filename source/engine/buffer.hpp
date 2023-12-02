#pragma once 

#include "context.hpp"

namespace Alpha::Engine
{
	struct BufferData
	{
		const void* data_ = nullptr;
		std::size_t size_ = 0; //TODO: redunant
	
		BufferData() = default;

		BufferData(const void* data, std::size_t size)
			: data_{ data }
			, size_{ size }
		{
		}
	};
	
	////////////////////////////////////////////////////////////////////////////////

	class Buffer;

	struct BufferViewDesc : public ObjectDesc
	{
		ResourceView type_ = ResourceView::Undefined;

		DXGI_FORMAT format_ = DXGI_FORMAT_UNKNOWN;

		std::size_t offset_ = 0;
		std::size_t width_ = 0;
	};

	class BufferView final : public Object<BufferViewDesc>
	{
		std::shared_ptr<Buffer> buffer_;
		void* descriptor_ = nullptr;
	public:
		BufferView()
			: Object(nullptr, {})
			, descriptor_{}
		{
		}

		BufferView(const BufferView&) = delete;
		BufferView& operator=(const BufferView&) = delete;

		BufferView(BufferView&&) = delete;
		BufferView& operator=(BufferView&&) = default;

		auto GetBuffer()
		{
			return buffer_.get();
		}

		//auto GetResourceViewType() const
		//{
		//	return desc_.type_;
		//}

		//auto GetDescriptorHandle()
		//{
		//	assert(false);
		//	return nullptr;
		//	//return descriptor_.GetHandleCPU(0);
		//}
	};

	////////////////////////////////////////////////////////////////////////////////

	struct BufferDesc final : public ObjectDesc
	{
		ResourceBind bind_ = ResourceBind::None;
		ResourceUsage usage_ = ResourceUsage::Default;

		std::uint32_t size_ = 0;
		std::uint32_t stride_ = 0;
	};

	////////////////////////////////////////////////////////////////////////////////

	class Buffer final : public Object<BufferDesc>, public Resource, public std::enable_shared_from_this<Buffer>
	{
		//TODO: no allocation per context, rather per "frame"
		//std::vector<PoolDynamic::Allocation> allocation_;
		PoolDynamic::Allocation allocation_;

		//STATE
		const std::uint32_t size_ = 0;
		const std::uint32_t stride_ = 0; //BitsPerPixel(element.Format) >> 3u;
		const ResourceUsage usage_ = ResourceUsage::Default;
		const ResourceBind bind_ = ResourceBind::None;
	public:
		Buffer(Renderer* renderer, const BufferDesc& desc, const BufferData* data);

		////////////////////////////////////////////////////////////////////////////////

		//STATE
		const auto GetBufferSize() const
		{
			return size_;
		}

		//STATE
		const auto GetBufferStride() const
		{
			return stride_;
		}

		//STATE
		const auto GetBufferUsage() const
		{
			return usage_;
		}

		//STATE
		const auto GetBufferBind() const
		{
			return bind_;
		}

		////////////////////////////////////////////////////////////////////////////////

		D3D12_CPU_VIRTUAL_ADDRESS GetAddressCPU()
		{
			return allocation_.first;
		}
		
		D3D12_GPU_VIRTUAL_ADDRESS GetAddressGPU()
		{
			return allocation_.second;
		}
	};
}