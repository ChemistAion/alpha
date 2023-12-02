#pragma once

namespace Alpha::Mockup
{
	struct Geometry
	{
		DXGI_FORMAT type_ = DXGI_FORMAT_UNKNOWN;
		std::uint32_t indices_;
		D3D12_INDEX_BUFFER_VIEW ib_view;
		std::vector<D3D12_VERTEX_BUFFER_VIEW> vb_views_;
	};
}