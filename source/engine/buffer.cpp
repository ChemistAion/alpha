#include "_engine.hpp"

#include "buffer.hpp"
#include "renderer.hpp"

namespace Alpha::Engine
{
	Buffer::Buffer(Renderer* renderer, const BufferDesc& desc, const BufferData* data)
		: Object(renderer, desc)
		, size_ { desc.size_ < D3D12_TEXTURE_DATA_PITCH_ALIGNMENT ? D3D12_TEXTURE_DATA_PITCH_ALIGNMENT : desc.size_ }
		, stride_ { desc.stride_ }
		, usage_ { desc.usage_ }
		, bind_ { desc.bind_ }
	{
		assert(size_ != 0);
		assert(stride_ != 0);
		assert(data == nullptr);
		//assert(usage_ != ResourceUsage::Static);
		//assert(usage_ != ResourceUsage::Staging);
		assert(usage_ == ResourceUsage::Dynamic);

		//assert(allocation_.size() == 1u); //TODO: Buffer::Buffer temporary fixed for ResourceUsage::Dynamic only...
		
		if (usage_ == ResourceUsage::Dynamic && data != nullptr)
		{
			ThrowIfFailed(E_INVALIDARG, GetName() + " as DynamicBuffer must be initialized by mapping");
		}
		
		//if (desc.bind_ == ResourceBind::UniformBuffer)
		//{
		//	assert(desc.size_ < D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
		//	desc_.size_ = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;
		//}

		if (usage_ == ResourceUsage::Dynamic && bind_ != ResourceBind::ShaderResource && bind_ != ResourceBind::UnorderedAccess)
		{
			SetResourceState(D3D12_RESOURCE_STATE_GENERIC_READ);
		}
		else
		{
			//TODO: throw
			assert(false);
		}

		////////////////////////////////////////////////////////////////////////////////

		auto& context = GetRenderer()->GetContext();
		allocation_ = context.AllocateDynamicBuffer(size_);
	}
};