#pragma once

#include "gltf/index.hpp"
#include "gltf/model.hpp"

namespace Alpha::Mockup
{
	struct Accessor
	{
		const void* data_ = nullptr;
		std::uint64_t count_ = 0;
		std::uint32_t dimension_;
		std::uint32_t type_size_;
	};

	Accessor ImportAccessor(glTF::Index index, const glTF::Model& model);
}