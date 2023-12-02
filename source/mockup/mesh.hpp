#pragma once

#include "buffer.hpp"

namespace Alpha::Mockup
{
	struct Primitives
	{
		XMVECTOR center_;
		XMVECTOR radius_;
	};
	
	struct Mesh
	{
		//std::vector<Primitives> primitives_;
		//std::map<Index, Buffer> buffers_;
		std::vector<Buffer> buffers_;
		Buffer indices_;
	};
}