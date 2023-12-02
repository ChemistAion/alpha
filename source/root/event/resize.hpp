#pragma once

namespace Alpha::Root::Event
{
	struct Resize
	{
		std::uint32_t width_ = 0;
		std::uint32_t height_ = 0;
	};
}