#pragma once

#include "index.hpp"

namespace Alpha::Mockup
{
	struct Transform
	{
		XMMATRIX rotation_ = XMMatrixIdentity();
		XMVECTOR translation_ = XMVectorSet(0, 0, 0, 0);
		XMVECTOR scale_ = XMVectorSet(1, 1, 1, 0);
	};

	struct Node
	{
		std::optional<Index> parent_;
		std::vector<Index> children_;
		Transform tranform_;
		//std::vector<PrimitiveIndex> primitives;
		std::optional<Index> skin_;
		//std::string name_;
	};
}



