#include "_engine.hpp"

#include "commands.hpp"
#include "renderer.hpp"

namespace Alpha::Engine
{
	Commands::Commands
	(
		Renderer* renderer,
		const std::uint32_t allocators_count,
		const std::uint32_t lists_count,
		const D3D12_COMMAND_QUEUE_DESC& desc
	)
		: Object{ renderer, {} }
		, lists_count_ { lists_count } //TODO: vector me
    {
		auto device = GetDevice();

		for (pool_.resize(allocators_count); auto& buffer : pool_)
		{
			HRESULT result;

			result = device->CreateCommandAllocator
			(
				desc.Type,
				IID_PPV_ARGS(&buffer.allocator_)
			);
			ThrowIfFailed(result, "Commands::Commands at CreateCommandAllocator failed");
		
			//TODO: set name...

			buffer.lists_ = new ID3D12GraphicsCommandList2* [lists_count];
		
			for (std::uint32_t index = 0; index < lists_count; ++index)
			{
				result = GetDevice()->CreateCommandList
				(
					GetNode(),
					desc.Type,
					buffer.allocator_,
					nullptr,
					IID_PPV_ARGS(&buffer.lists_[index])
				);			
				ThrowIfFailed(result, "Commands::Commands at CreateCommandList failed");
		
				result = buffer.lists_[index]->Close();
				ThrowIfFailed(result, "Commands::Commands at ID3D12GraphicsCommandList2::Close failed");
			}
		}
		
		////////////////////////////////////////////////////////////////////////////////
		
		auto queue = desc.Type == D3D12_COMMAND_LIST_TYPE_DIRECT
			? GetRenderer()->GetGraphicsQueue()
			: GetRenderer()->GetComputeQueue();
		
		for (auto& buffer : pool_)
		{		
			queue->ExecuteCommandLists
			(
				lists_count,
				(ID3D12CommandList* const*)(buffer.lists_)
			);
		}
		
		//TODO: check it...
		GetRenderer()->Flush(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		GetRenderer()->Flush(D3D12_COMMAND_LIST_TYPE_DIRECT);
		
		//TODO: check me
		current_ = &pool_[0];
    }

	Commands::~Commands()
	{
		for (auto& buffer : pool_)
		{
			for (std::uint32_t index = 0; index < lists_count_; ++index)
			{
				buffer.lists_[index]->Release();
			}

			delete [] buffer.lists_;

			buffer.allocator_->Release();
		}
	}

	////////////////////////////////////////////////////////////////////////////////

	ID3D12GraphicsCommandList2* Commands::GetNewCommandList()
	{
		assert(current_ != nullptr);
		//assert(current_->used_ < current_->lists_.size());

		ID3D12GraphicsCommandList2* list = current_->lists_[current_->used_++];
		
		auto result = list->Reset(current_->allocator_, nullptr);
		ThrowIfFailed(result, "Commands::GetNewCommandList at ID3D12GraphicsCommandList2::Reset failed");
		
		return list;
	}

	void Commands::BeginFrame(Frame frame)
	{
		current_ = &pool_[(frame) % pool_.size()];
		
		auto result = current_->allocator_->Reset();
		ThrowIfFailed(result, "Commands::BeginFrame at ID3D12CommandAllocator::Reset failed");
		
		current_->used_ = 0;
	}
}