#pragma once

namespace Alpha::Engine
{
	class Renderer;

	struct CommandsDesc : public ObjectDesc
	{
	};

	class Commands final : public Object<CommandsDesc>
	{
		struct CommandsBuffer
		{
			ID3D12CommandAllocator* allocator_ = nullptr;
			ID3D12GraphicsCommandList2** lists_ = nullptr;
			std::uint32_t used_ = 0;
		};

		std::vector<CommandsBuffer> pool_;
		CommandsBuffer* current_ = nullptr;

		const std::uint32_t lists_count_ = 0;
	public:
		explicit Commands
		(
			Renderer* renderer,
			const std::uint32_t allocators_count,
			const std::uint32_t lists_count,
			const D3D12_COMMAND_QUEUE_DESC& desc
		);

		Commands() = default;
		~Commands();

		Commands(const Commands&) = delete;
		Commands& operator=(const Commands&) = delete;

		Commands(Commands&&) = delete;
		Commands& operator=(Commands&&) = delete;

		////////////////////////////////////////////////////////////////////////////////

		ID3D12CommandAllocator* GetAllocator() noexcept
		{
			assert(current_);
			return current_->allocator_;
		}

		ID3D12GraphicsCommandList2* GetNewCommandList();

		void BeginFrame(Frame frame);
	};
}