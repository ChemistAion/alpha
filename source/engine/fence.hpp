#pragma once

#include "object.hpp"

namespace Alpha::Engine
{
	class Renderer;

	struct FenceDesc : public ObjectDesc
	{
	};

	class Fence final : public Object<FenceDesc>
	{
		HANDLE event_ { INVALID_HANDLE_VALUE };
		Frame frame_ { 0 };
		ComPtr<ID3D12Fence> fence_;
	public:
		Fence() = default;

		~Fence()
		{
			if (event_ != INVALID_HANDLE_VALUE)
			{
				::CloseHandle(event_);
			}
		}

		explicit Fence(Renderer* renderer, const FenceDesc& desc);

		////////////////////////////////////////////////////////////////////////////////

		void Signal(ID3D12CommandQueue* command_queue, Frame frame);
		bool WaitCPU(Frame old_frame);
		void WaitGPU(ID3D12CommandQueue* command_queue);
	};
}