#pragma once

//#include "renderer.hpp"
#include "device.hpp"

namespace Alpha::Engine
{
	struct ObjectDesc
	{
		std::string name_ = "unnamed";
	};

	class Renderer;

	//TODO: remove renderer_ etc.

	//TODO: check if ObjectDescType is based on ObjectDesc
	//TODO: no state! 
	template <typename ObjectDescType>
	class Object
	{
		//TODO: change to shared_ptr, finally remove it
		Renderer* renderer_ = nullptr;
		std::string name_;
	public:
		Object(Renderer* renderer, const ObjectDescType& desc) noexcept
			: renderer_ { renderer }
			, name_ { desc.name_ }
		{
		}

		const std::string& GetName() const noexcept
		{
			return name_;
		}

		Renderer* GetRenderer() const noexcept
		{
			assert(renderer_ != nullptr);
			return renderer_;
		}

		ID3D12Device* GetDevice() const noexcept
		{
			assert(renderer_ != nullptr);
			return renderer_->GetDevice();
		}

		UINT GetNode() const noexcept
		{
			assert(renderer_ != nullptr);
			return renderer_->GetNode();
		}
	};
}