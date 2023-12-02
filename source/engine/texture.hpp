#pragma once

namespace Alpha::Engine
{
	class Renderer;
	class Buffer;

	struct TextureDepthClearValue
	{
		float depth_ = 1.0f;
		std::uint8_t stencil_ = 0;

		TextureDepthClearValue() noexcept = default;

		TextureDepthClearValue(float depth, std::uint8_t stencil) noexcept
			: depth_{ depth }
			, stencil_{ stencil }
		{
		}
	};

	constexpr std::array<float, 4> cTextureClearValue_ColorZero = { 0.0f, 0.0f, 0.0f, 0.0f };
	constexpr std::array<float, 4> cTextureClearValue_ColorDefault = { 0.3f, 0.3f, 0.3f, 1.0f };

	struct TextureClearValue
	{
		DXGI_FORMAT format_ = DXGI_FORMAT_UNKNOWN;
		std::array<float, 4> color_ = cTextureClearValue_ColorZero;
		TextureDepthClearValue depth_stencil_;

		TextureClearValue() noexcept = default;

		//bool operator == (const TextureClearValue& rhs) const
		//{
		//	return
		//		format_ == rhs.format_ &&
		//		color_[0] == rhs.color_[0] &&
		//		color_[1] == rhs.color_[1] &&
		//		color_[2] == rhs.color_[2] &&
		//		color_[3] == rhs.color_[3] &&
		//		depth_stencil_.depth_ == rhs.depth_stencil_.depth_ &&
		//		depth_stencil_.stencil_ == rhs.depth_stencil_.stencil_;
		//}
	};

	////////////////////////////////////////////////////////////////////////////////

	struct TextureDesc : public ObjectDesc
	{
		std::uint64_t width_ = 0;
		std::uint16_t height_ = 0;	
		std::uint16_t depth_ = 1;
		std::uint16_t mip_levels_ = 1;
		std::uint32_t sample_count_ = 1;

		ResourceDimension dimension_ = ResourceDimension::Undefined;

		DXGI_FORMAT format_ = DXGI_FORMAT_UNKNOWN;
		ResourceUsage usage_ = ResourceUsage::Default;
		ResourceBind bind_ = ResourceBind::None;

		TextureClearValue clear_value_;
	};

	////////////////////////////////////////////////////////////////////////////////

	struct TextureResourceData
	{
		const void* data_ = nullptr;
	
		std::uint32_t offset_ = 0;
		std::uint32_t stride_ = 0;
		std::uint32_t depth_stride_ = 0;
	
		TextureResourceData() noexcept = default;
	
		TextureResourceData
		(
			const void* data,
			std::uint32_t stride,
			std::uint32_t depth_stride
		) noexcept
			: data_{ data }
			, stride_{ stride }
			, depth_stride_{ depth_stride }
		{
		}
	};
	
	struct TextureData
	{
		TextureResourceData* sub_resources_ = nullptr;
		std::uint32_t count_ = 0;
	
		TextureData() noexcept = default;
	
		//TODO: vector based
		TextureData(TextureResourceData* sub_resources, std::uint32_t count) noexcept
			: sub_resources_{ sub_resources }
			, count_{ count }
		{
		}
	};
	
	////////////////////////////////////////////////////////////////////////////////

	class Texture;

	struct TextureViewDesc : public ObjectDesc
	{
		ResourceView type_ = ResourceView::Undefined;

		//TODO: check dimension_/format_ superfluous?
		ResourceDimension dimension_ = ResourceDimension::Undefined;
		DXGI_FORMAT format_ = DXGI_FORMAT_UNKNOWN;
		
		std::uint32_t most_mip_level_ = 0; //TODO: check it
		std::uint32_t mip_levels_ = 0; //std::numeric_limits<std::uint32_t>::max()

		std::uint32_t first_slice_ = 0;
		std::uint32_t count_slice_ = 0;
	};

	class TextureView final : public Object<TextureViewDesc>
	{
		std::shared_ptr<Texture> texture_;
		HeapDescriptor::Element element_ = {};

		//STATE
		ResourceView type_ = ResourceView::Undefined;
	public:
		TextureView()
			: Object(nullptr, {})
		{
		}

		TextureView
		(
			Renderer* renderer,
			const TextureViewDesc& desc,
			const std::shared_ptr<Texture>& texture,
			HeapDescriptor::Element&& element
		);

		TextureView(const TextureView&) = delete;
		TextureView& operator=(const TextureView&) = delete;

		TextureView(TextureView&&) = default;
		TextureView& operator=(TextureView&&) = default;

		Texture* GetTexture()
		{
			assert(texture_);
			return texture_.get();
		}

		auto GetResourceViewType() const noexcept
		{
			assert(texture_);
			return type_;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle()
		{
			//TODO: check it, only for RTV/DSV
			assert(texture_);
			assert(element_.IsValid());
			return element_.GetHandleCPU(0);
		}
	};

	////////////////////////////////////////////////////////////////////////////////

	class Texture final : public Object<TextureDesc>, public Resource, public std::enable_shared_from_this<Texture>
	{
		//TODO: static? move it to Renderer
		TextureView CreateSRV(TextureViewDesc& desc);
		TextureView CreateRTV(TextureViewDesc& desc);
		TextureView CreateDSV(TextureViewDesc& desc);
		TextureView CreateUAV(TextureViewDesc& desc);

		//STATE
		ResourceUsage usage_ = ResourceUsage::Default;
		ResourceBind bind_ = ResourceBind::None;
		DXGI_FORMAT format_ = DXGI_FORMAT_UNKNOWN;
		ResourceDimension dimension_ = ResourceDimension::Undefined;
		
		std::uint64_t width_ = 0;
		std::uint16_t height_ = 0;
		std::uint16_t depth_ = 0;
		std::uint16_t mip_levels_ = 0;
		std::uint32_t sample_count_ = 0;
	public:
		//Texture() = default;

		explicit Texture(Renderer* renderer, const TextureDesc& desc, const TextureData* data);
		//explicit Texture(Renderer* renderer, const TextureDesc& desc, ID3D12Resource* resource, D3D12_RESOURCE_STATES state);

		////////////////////////////////////////////////////////////////////////////////
		
		TextureView CreateView(const TextureViewDesc& desc);
	};
}
