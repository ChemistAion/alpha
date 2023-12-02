#include "_engine.hpp"

#include "texture.hpp"
#include "renderer.hpp"

namespace Alpha::Engine
{
	TextureView::TextureView
	(
		Renderer* renderer,
		const TextureViewDesc& desc,
		const std::shared_ptr<Texture>& texture,
		HeapDescriptor::Element&& element
	)
		: Object(renderer, desc)
		, texture_ { texture }
		, element_ { std::move(element) }
		, type_ { desc.type_ }
	{
	}

	TextureView Texture::CreateSRV(TextureViewDesc& desc)
	{
		assert(desc.type_ == ResourceView::ShaderResource);
		assert(bind_ == ResourceBind::ShaderResource);

		assert(format_ != DXGI_FORMAT_UNKNOWN);
		desc.format_ == DXGI_FORMAT_UNKNOWN ? format_ : desc.format_;

		D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc;
		std::memset(&srv_desc, 0, sizeof(srv_desc));

		srv_desc.Format = desc.format_;
		srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		switch (dimension_)
		{
		case ResourceDimension::Texture1D:
		{
			srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
			srv_desc.Texture1D.MostDetailedMip = desc.most_mip_level_;
			srv_desc.Texture1D.MipLevels = desc.mip_levels_;
			srv_desc.Texture1D.ResourceMinLODClamp = 0.0f;
			break;
		}
		case ResourceDimension::Texture1D_Array:
		{
			srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
			srv_desc.Texture1DArray.ArraySize = desc.count_slice_;
			srv_desc.Texture1DArray.FirstArraySlice = desc.first_slice_;
			srv_desc.Texture1DArray.MostDetailedMip = desc.most_mip_level_;
			srv_desc.Texture1DArray.MipLevels = desc.mip_levels_;
			srv_desc.Texture1DArray.ResourceMinLODClamp = 0.0f;
			break;
		}
		case ResourceDimension::Texture2D:
		{
			if (sample_count_ > 1)
			{
				assert(false);
				//srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
				//srv_desc.Texture2DMS.UnusedField_NothingToDefine = 0;
			}
			else
			{
				srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				srv_desc.Texture2D.MostDetailedMip = desc.most_mip_level_;
				srv_desc.Texture2D.MipLevels = desc.mip_levels_;
				srv_desc.Texture2D.PlaneSlice = 0;
				srv_desc.Texture2D.ResourceMinLODClamp = 0.0f;
			}
			break;
		}
		case ResourceDimension::Texture2D_Array:
		{
			if (sample_count_ > 1)
			{
				assert(false);
				//srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
				//srv_desc.Texture2DMSArray.ArraySize = desc.count_slice_;
				//srv_desc.Texture2DMSArray.FirstArraySlice = desc.first_slice_;
				//srv_desc.Texture2DArray.PlaneSlice = 0;
				//srv_desc.Texture2DArray.ResourceMinLODClamp = 0.0f;
			}
			else
			{
				srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
				srv_desc.Texture2DArray.ArraySize = desc.count_slice_;
				srv_desc.Texture2DArray.FirstArraySlice = desc.first_slice_;
				srv_desc.Texture2DArray.MostDetailedMip = desc.most_mip_level_;
				srv_desc.Texture2DArray.MipLevels = desc.mip_levels_;
			}
			break;
		}
		case ResourceDimension::Texture3D:
		{
			srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
			srv_desc.Texture3D.MostDetailedMip = desc.most_mip_level_;
			srv_desc.Texture3D.MipLevels = desc.mip_levels_;
			srv_desc.Texture3D.ResourceMinLODClamp = 0.0f;
			break;
		}
		case ResourceDimension::TextureCube:
		{
			srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			srv_desc.TextureCube.MostDetailedMip = desc.most_mip_level_;
			srv_desc.TextureCube.MipLevels = desc.mip_levels_;
			srv_desc.TextureCube.ResourceMinLODClamp = 0.0f;
			break;
		}
		case ResourceDimension::TextureCube_Array:
		{
			srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
			srv_desc.TextureCubeArray.MostDetailedMip = desc.most_mip_level_;
			srv_desc.TextureCubeArray.MipLevels = desc.mip_levels_;
			srv_desc.TextureCubeArray.First2DArrayFace = desc.first_slice_;
			srv_desc.TextureCubeArray.NumCubes = desc.count_slice_ / 6;
			srv_desc.TextureCubeArray.ResourceMinLODClamp = 0.0f;
			break;
		}
		default:
		{
			ThrowIfFailed(E_INVALIDARG, "Invalid TextureViewDesc dimension type.");
		}
		}

		return { GetRenderer(), desc, shared_from_this(), {} };
	}
	
	TextureView Texture::CreateRTV(TextureViewDesc& desc)
	{
		assert(desc.type_ == ResourceView::RenderTarget);
		assert(bind_ == ResourceBind::RenderTarget);

		assert(format_ != DXGI_FORMAT_UNKNOWN);
		desc.format_ == DXGI_FORMAT_UNKNOWN ? format_ : desc.format_;

		D3D12_RENDER_TARGET_VIEW_DESC rtv_desc;
		std::memset(&rtv_desc, 0, sizeof(rtv_desc));

		rtv_desc.Format = desc.format_;

		switch (dimension_)
		{
		case ResourceDimension::Texture1D:
		{
			rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
			rtv_desc.Texture1D.MipSlice = desc.most_mip_level_;
			break;
		}
		case ResourceDimension::Texture1D_Array:
		{
			rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
			rtv_desc.Texture1DArray.ArraySize = desc.count_slice_;
			rtv_desc.Texture1DArray.FirstArraySlice = desc.first_slice_;
			rtv_desc.Texture1DArray.MipSlice = desc.most_mip_level_;
			break;
		}
		case ResourceDimension::Texture2D:
		{
			if (sample_count_ > 1)
			{
				assert(false);
				//rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
				//rtv_desc.Texture2DMS.UnusedField_NothingToDefine = 0;
			}
			else
			{
				rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
				rtv_desc.Texture2D.MipSlice = desc.most_mip_level_;
				rtv_desc.Texture2D.PlaneSlice = 0;
			}
			break;
		}
		case ResourceDimension::Texture2D_Array:
		{
			if (sample_count_ > 1)
			{
				assert(false);
				//rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
				//rtv_desc.Texture2DMSArray.ArraySize = desc.count_slice_;
				//rtv_desc.Texture2DMSArray.FirstArraySlice = desc.first_slice_;
				//rtv_desc.Texture2DArray.PlaneSlice = 0;
			}
			else
			{
				rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
				rtv_desc.Texture2DArray.ArraySize = desc.count_slice_;
				rtv_desc.Texture2DArray.FirstArraySlice = desc.first_slice_;
				rtv_desc.Texture2DArray.MipSlice = desc.most_mip_level_;
			}
			break;
		}
		case ResourceDimension::Texture3D:
		{
			rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
			rtv_desc.Texture3D.FirstWSlice = desc.first_slice_;
			rtv_desc.Texture3D.WSize = desc.count_slice_;
			rtv_desc.Texture3D.MipSlice = desc.most_mip_level_;
			break;
		}
		default:
		{
			ThrowIfFailed(E_INVALIDARG, "Invalid TextureViewDesc dimension type.");
		}
		}

		////////////////////////////////////////////////////////////////////////////////
		
		auto element = GetRenderer()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1);

		GetDevice()->CreateRenderTargetView(GetResource(), &rtv_desc, element.GetHandleCPU(0));

		return { GetRenderer(), desc, shared_from_this(), std::move(element) };
	}
	
	TextureView Texture::CreateDSV(TextureViewDesc& desc)
	{
		assert(desc.type_ == ResourceView::DepthStencil);
		assert(bind_ == ResourceBind::DepthStencil);

		assert(format_ != DXGI_FORMAT_UNKNOWN);
		desc.format_ == DXGI_FORMAT_UNKNOWN ? format_ : desc.format_;

		D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc;
		std::memset(&dsv_desc, 0, sizeof(dsv_desc));

		dsv_desc.Format = desc.format_;
		dsv_desc.Flags = D3D12_DSV_FLAG_NONE;

		switch (dimension_)
		{
		case ResourceDimension::Texture1D:
		{
			dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1D;
			dsv_desc.Texture1D.MipSlice = desc.most_mip_level_;
			break;
		}
		case ResourceDimension::Texture1D_Array:
		{
			dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
			dsv_desc.Texture1DArray.ArraySize = desc.count_slice_;
			dsv_desc.Texture1DArray.FirstArraySlice = desc.first_slice_;
			dsv_desc.Texture1DArray.MipSlice = desc.most_mip_level_;
			break;
		}
		case ResourceDimension::Texture2D:
		{
			if (sample_count_ > 1)
			{
				assert(false);
				//dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
				//dsv_desc.Texture2DMS.UnusedField_NothingToDefine = 0;
			}
			else
			{
				dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				dsv_desc.Texture2D.MipSlice = desc.most_mip_level_;
			}
			break;
		}
		case ResourceDimension::Texture2D_Array:
		{
			if (sample_count_ > 1)
			{
				assert(false);
				//dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY;
				//dsv_desc.Texture2DMSArray.ArraySize = desc.count_slice_;
				//dsv_desc.Texture2DMSArray.FirstArraySlice = desc.first_slice_;
			}
			else
			{
				dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
				dsv_desc.Texture2DArray.ArraySize = desc.count_slice_;
				dsv_desc.Texture2DArray.FirstArraySlice = desc.first_slice_;
				dsv_desc.Texture2DArray.MipSlice = desc.most_mip_level_;
			}
			break;
		}
		default:
		{
			ThrowIfFailed(E_INVALIDARG, "Invalid TextureViewDesc dimension type.");
		}
		}

		////////////////////////////////////////////////////////////////////////////////

		auto element = GetRenderer()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);

		GetDevice()->CreateDepthStencilView(GetResource(), &dsv_desc, element.GetHandleCPU(0));

		return { GetRenderer(), desc, shared_from_this(), std::move(element) };
	}
	
	TextureView Texture::CreateUAV(TextureViewDesc& desc)
	{
		assert(desc.type_ == ResourceView::UnorderedAccess);
		assert(bind_ == ResourceBind::UnorderedAccess);

		assert(format_ != DXGI_FORMAT_UNKNOWN);
		desc.format_ == DXGI_FORMAT_UNKNOWN ? format_ : desc.format_;

		D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc;
		std::memset(&uav_desc, 0, sizeof(uav_desc));

		uav_desc.Format = desc.format_;

		switch (dimension_)
		{
		case ResourceDimension::Texture1D:
		{
			uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
			uav_desc.Texture1D.MipSlice = desc.most_mip_level_;
			break;
		}
		case ResourceDimension::Texture1D_Array:
		{
			uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
			uav_desc.Texture1DArray.ArraySize = desc.count_slice_;
			uav_desc.Texture1DArray.FirstArraySlice = desc.first_slice_;
			uav_desc.Texture1DArray.MipSlice = desc.most_mip_level_;
			break;
		}
		case ResourceDimension::Texture2D:
		{
			uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
			uav_desc.Texture2D.MipSlice = desc.most_mip_level_;
			uav_desc.Texture2D.PlaneSlice = 0;
			break;
		}
		case ResourceDimension::Texture2D_Array:
		{
			uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
			uav_desc.Texture2DArray.ArraySize = desc.count_slice_;
			uav_desc.Texture2DArray.FirstArraySlice = desc.first_slice_;
			uav_desc.Texture2DArray.MipSlice = desc.most_mip_level_;
			uav_desc.Texture2DArray.PlaneSlice = 0;
			break;
		}
		case ResourceDimension::Texture3D:
		{
			uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
			uav_desc.Texture3D.FirstWSlice = desc.first_slice_;
			uav_desc.Texture3D.WSize = desc.count_slice_;
			uav_desc.Texture3D.MipSlice = desc.most_mip_level_;
			break;
		}
		default:
		{
			ThrowIfFailed(E_INVALIDARG, "Invalid TextureViewDesc dimension type.");
		}
		}

		return { GetRenderer(), desc, shared_from_this(), {} };
	}

	////////////////////////////////////////////////////////////////////////////////

	Texture::Texture(Renderer* renderer, const TextureDesc& desc, const TextureData* data)
		: Object(renderer, desc)
		, usage_{ desc.usage_ }
		, bind_{ desc.bind_ }
		, format_{ desc.format_ }
		, dimension_{ desc.dimension_ }
		, width_{ desc.width_ }
		, height_{ desc.height_ }
		, depth_{ desc.depth_ }
		, mip_levels_{ desc.mip_levels_ }
		, sample_count_{ desc.sample_count_ }
	{
		if (usage_ == ResourceUsage::Static && (data == nullptr || data->sub_resources_ == nullptr))
		{
			auto error = GetName() + " as StaticTexture is not initialized";
			ThrowIfFailed(E_INVALIDARG, error.c_str());
		}

		////////////////////////////////////////////////////////////////////////////////

		CD3DX12_RESOURCE_DESC resource_desc = {};

		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

		switch (bind_)
		{
		case ResourceBind::RenderTarget:
		{
			flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
			break;
		}
		case ResourceBind::UnorderedAccess:
		{
			flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
			break;
		}
		case ResourceBind::DepthStencil:
		{
			flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
			break;
		}
		default:
			break;
		}

		if (bind_ != ResourceBind::ShaderResource)
		{
			flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
		}

		//TODO: check as STATE
		switch (desc.dimension_)
		{
		case ResourceDimension::Texture1D:
		case ResourceDimension::Texture1D_Array:
		{
			resource_desc = CD3DX12_RESOURCE_DESC::Tex1D
			(
				format_,
				width_,
				depth_,
				mip_levels_,
				flags
			);

			break;
		}
		case ResourceDimension::Texture2D:
		case ResourceDimension::Texture2D_Array:
		case ResourceDimension::TextureCube:
		case ResourceDimension::TextureCube_Array:
		{
			resource_desc = CD3DX12_RESOURCE_DESC::Tex2D
			(
				format_,
				width_,
				height_,
				depth_,
				mip_levels_,
				sample_count_,
				0, //TODO: fixed
				flags
			);

			break;
		}
		case ResourceDimension::Texture3D:
		{
			resource_desc = CD3DX12_RESOURCE_DESC::Tex3D
			(
				format_,
				width_,
				height_,
				depth_,
				mip_levels_,
				flags
			);

			break;
		}
		default:
		{
			ThrowIfFailed(E_INVALIDARG, "Invalid TextureDesc dimension type.");
		}
		}

		assert(usage_ != ResourceUsage::Staging);

		////////////////////////////////////////////////////////////////////////////////

		auto device = GetDevice();

		D3D12_CLEAR_VALUE clear_value;
		D3D12_CLEAR_VALUE* clear_value_ptr = nullptr;

		if (resource_desc.Flags & (D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL))
		{
			assert(resource_desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE);
			assert(desc.clear_value_.format_ != DXGI_FORMAT_UNKNOWN);
			clear_value.Format = desc.clear_value_.format_;

			if (resource_desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
			{
				std::memcpy(clear_value.Color, desc.clear_value_.color_.data(), sizeof(clear_value.Color));
			}
			else if (resource_desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
			{
				clear_value.DepthStencil.Depth = desc.clear_value_.depth_stencil_.depth_;
				clear_value.DepthStencil.Stencil = desc.clear_value_.depth_stencil_.stencil_;
			}

			clear_value_ptr = &clear_value;
		}

		auto resource_heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		bool initialize = (data != nullptr && data->sub_resources_ != nullptr && data->count_ > 0);
		SetResourceState(D3D12_RESOURCE_STATE_COMMON);

		auto result = device->CreateCommittedResource
		(
			&resource_heap,
			D3D12_HEAP_FLAG_NONE,
			&resource_desc,
			D3D12_RESOURCE_STATE_COMMON,
			clear_value_ptr,
			IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf())
		);

		ThrowIfFailed(result, "Texture failed");

		{
			const auto& name = GetName();

			std::wstring w_name(name.cbegin(), name.cend());
			resource_->SetName(w_name.c_str());
		}

		////////////////////////////////////////////////////////////////////////////////

		//TODO: split ctor to static/dynamic...
		//TODO: fixed to 1 subresource
		if (initialize)
		{
			std::uint32_t count = resource_desc.MipLevels;
			count *= resource_desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D
				? 1u
				: resource_desc.DepthOrArraySize;
			
			if (data->count_ != count)
			{
				ThrowIfFailed(E_INVALIDARG, "Wrong number of Texture SubResources");
			}
			
			UINT64 upload_size = 0;
			D3D12_PLACED_SUBRESOURCE_FOOTPRINT placed[1] = {};
			UINT rows[1] = {};
			UINT64 sizes[1] = {};
			
			device->GetCopyableFootprints
			(
				&resource_desc,
				0,
				count,
				0,
				placed,
				rows,
				sizes,
				&upload_size
			);

			auto upload = GetRenderer()->GetHeapUpload();
			auto place = upload->Allocate(upload_size, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);

			assert(place != nullptr);
			placed[0].Offset += UINT64(place - upload->GetBasePointer());

			assert(data->sub_resources_ != nullptr);
			assert(data->count_ == 1);

			//TODO: verify width/height resource_desc vs. placed
			//TODO: check: resource_desc.Height * 4
			std::memcpy
			(
				place,
				data->sub_resources_[0].data_,
				resource_desc.Width * resource_desc.Height * 4
			);

			////////////////////////////////////////////////////////////////////////////////

			//TODO: improve me...
			{
				{
					D3D12_RESOURCE_BARRIER RBDesc = {};
					RBDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					RBDesc.Transition.pResource = resource_.Get();
					RBDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
					RBDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
					RBDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
					upload->GetCommandList()->ResourceBarrier(1, &RBDesc);
				}

				CD3DX12_TEXTURE_COPY_LOCATION Dst(resource_.Get(), 0);
				CD3DX12_TEXTURE_COPY_LOCATION Src(upload->GetResource(), placed[0]);
				upload->GetCommandList()->CopyTextureRegion(&Dst, 0, 0, 0, &Src, NULL);

				{
					D3D12_RESOURCE_BARRIER RBDesc = {};
					RBDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					RBDesc.Transition.pResource = resource_.Get();
					RBDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
					RBDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
					RBDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
					upload->GetCommandList()->ResourceBarrier(1, &RBDesc);
				}
			}

			////////////////////////////////////////////////////////////////////////////////

			upload->Flush();
		}
	}

//	Texture::Texture
//	(
//		Renderer* renderer,
//		const TextureDesc& desc,
//		ID3D12Resource* resource,
//		D3D12_RESOURCE_STATES state
//	)
//		: Object(renderer, desc)
//		//, usage_{ desc.usage_ }
//		//, bind_{ desc.bind_ }
//		//, format_{ desc.format_ }
//	{
//		assert(resource != nullptr);
//	
//		auto resource_desc = resource->GetDesc();
//	
//		width_ = resource_desc.Width;
//		height_ = resource_desc.Height;
//		depth_ = resource_desc.DepthOrArraySize;
//		mip_levels_ = resource_desc.MipLevels;
//		sample_count_ = resource_desc.SampleDesc.Count;
//	
//		switch (resource_desc.Dimension)
//		{
//		case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
//			dimension_ = depth_ == 1 ? ResourceDimension::Texture1D : ResourceDimension::Texture1D_Array;
//			break;
//		case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
//			dimension_ = depth_ == 1 ? ResourceDimension::Texture2D : ResourceDimension::Texture2D_Array;
//			break;
//		case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
//			dimension_ = ResourceDimension::Texture3D;
//			break;
//		default:
//			assert(false);
//			break;
//		}
//	
//		format_ = resource_desc.Format;
//		usage_ = ResourceUsage::Default;
//		bind_ = ResourceBind::None;
//	
//		switch (resource_desc.Flags)
//		{
//		case D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET:
//			bind_ = ResourceBind::RenderTarget;
//			break;
//		case D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL:
//			bind_ = ResourceBind::DepthStencil;
//			break;
//		case D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS:
//			bind_ = ResourceBind::UnorderedAccess;
//			break;
//		default:
//			assert(false);
//		}
//	
//		//TODO: desc_.bind_ below...
//		//desc_.bind_ |= resource_desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE ? ResourceBind_None : ResourceBind_ShaderResource;
//		//desc_.access_ = ResourceAccess::None;
//	
//		assert(desc.bind_ == ResourceBind::None);
//		assert(resource_desc.Flags & (D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL));
//	
//		assert(!resource_);
//		resource_ = resource;
//		SetResourceState(state);
//	}
	
	TextureView Texture::CreateView(const TextureViewDesc& desc)
	{
		try
		{
			auto updated_desc = desc;

			if (desc.type_ == ResourceView::Undefined)
			{
				ThrowIfFailed(E_INVALIDARG, "TextureView type undefined");
			}

			assert(format_ != DXGI_FORMAT_UNKNOWN);

			if (desc.dimension_ == ResourceDimension::Undefined)
			{
				if (dimension_ == ResourceDimension::TextureCube || dimension_ == ResourceDimension::TextureCube_Array)
				{
					updated_desc.dimension_ = desc.type_ == ResourceView::ShaderResource ? dimension_ : ResourceDimension::Texture2D_Array;
				}
				else
				{
					updated_desc.dimension_ = dimension_;
				}
			}

			if (desc.most_mip_level_ >= mip_levels_)
			{
				ThrowIfFailed(E_INVALIDARG, "TextureView mip levels out of range");
			}

			if (desc.mip_levels_ != std::numeric_limits<std::uint32_t>::max() && desc.most_mip_level_ + desc.mip_levels_ > mip_levels_)
			{
				ThrowIfFailed(E_INVALIDARG, "TextureView mip levels out of range");
			}

			if (desc.mip_levels_ == 0 || desc.mip_levels_ == std::numeric_limits<std::uint32_t>::max())
			{
				//TODO: error?
				//updated_desc.mip_levels_ = desc.type_ == ResourceView::ShaderResource ? desc_.mip_levels_ - desc.most_mip_level_ : 1;
				updated_desc.mip_levels_ = desc.type_ == ResourceView::ShaderResource ? desc.mip_levels_ - desc.most_mip_level_ : 1;
			}

			if (desc.count_slice_ == 0 || desc.count_slice_ == std::numeric_limits<std::uint32_t>::max())
			{
				bool is_array = false;

				is_array |= desc.dimension_ == ResourceDimension::Texture1D_Array;
				is_array |= desc.dimension_ == ResourceDimension::Texture2D_Array;
				is_array |= desc.dimension_ == ResourceDimension::TextureCube;
				is_array |= desc.dimension_ == ResourceDimension::TextureCube_Array;

				if (is_array)
				{
					//TODO: check it
					//updated_desc.count_slice_ = desc_.array_size_ - desc.first_slice_;
					updated_desc.count_slice_ = depth_ - desc.first_slice_;
				}
				else if (desc.dimension_ == ResourceDimension::Texture3D)
				{
					assert(false);
					//auto depth = desc_.depth_ >> desc.most_mip_level_;
					//updated_desc.count_depth_slices_ = depth - desc.first_depth_slice_;
				}
				else
				{
					updated_desc.count_slice_ = 1;
				}
			}

			////////////////////////////////////////////////////////////////////////////////

			switch (desc.type_)
			{
			case ResourceView::ShaderResource:
			{
				return CreateSRV(updated_desc);
			}
			case ResourceView::RenderTarget:
			{
				return CreateRTV(updated_desc);
			}
			case ResourceView::DepthStencil:
			{
				return CreateDSV(updated_desc);
			}
			case ResourceView::UnorderedAccess:
			{
				return CreateUAV(updated_desc);
			}
			default:
			{
				assert(false);
				break;
			}
			}

			//TODO: Mip generator

			assert(false);
			return {};
		}

		catch (...)
		{
			assert(false);
		}

		//TODO: better error handling
		ThrowIfFailed(E_FAIL, "[Texture::CreateView] failed");
		return {};
	}
}