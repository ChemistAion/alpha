#include "_engine.hpp"

#include "fence.hpp"
#include "renderer.hpp"

namespace Alpha::Engine
{
	Fence::Fence(Renderer* renderer, const FenceDesc& desc)
		: Object(renderer, desc)
		, event_ { ::CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS) }
		, frame_ { 0 }
	{
		auto result = GetDevice()->CreateFence
		(
			frame_,
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(fence_.ReleaseAndGetAddressOf())
		);

		ThrowIfFailed(result, "[Fence::Fence] CreateFence failed");
	}

	////////////////////////////////////////////////////////////////////////////////

	void Fence::Signal(ID3D12CommandQueue* command_queue, Frame frame)
	{
		assert(command_queue != nullptr);
		assert(frame_ == frame);

		frame_++;

		auto result = command_queue->Signal(fence_.Get(), frame_);
		ThrowIfFailed(result, "[Fence::Signal] ID3D12CommandQueue::Signal failed");
	}

	bool Fence::WaitCPU(Frame old_frame)
	{
		if (frame_ > old_frame) [[likely]]
		{
			auto wait = frame_ - old_frame;

			if (fence_->GetCompletedValue() <= wait)
			{
				auto result = fence_->SetEventOnCompletion(wait, event_);
				ThrowIfFailed(result, "[Fence::WaitCPU] ID3D12Fence::SetEventOnCompletion failed");
			
				if (auto state = ::WaitForSingleObject(event_, NULL); state == WAIT_OBJECT_0) [[likely]]
				{
					return false;
				}
				else [[unlikely]]
				{
					auto error = ::GetLastError();
					ThrowIfFailed(E_FAIL, "[Fence::WaitCPU] WaitForSingleObject failed, error=" + std::to_string(error));
				}

				return false;
			}
		}

		return true;
	}

	void Fence::WaitGPU(ID3D12CommandQueue* command_queue)
	{
		assert(command_queue != nullptr);

		auto result = command_queue->Wait(fence_.Get(), frame_);
		ThrowIfFailed(result, "[Fence::WaitGPU] ID3D12CommandQueue::Wait failed");
	}
}