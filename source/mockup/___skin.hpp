#pragma once

namespace Alpha::Mockup
{
	struct Skin
	{
		struct Joint
		{
			Index node_;
			XMMATRIX ibm_ = XMMatrixIdentity();
		};

		std::vector<XMMATRIX> bones_;
		std::vector<Joint> joints_;
		Index node_;
	};
}