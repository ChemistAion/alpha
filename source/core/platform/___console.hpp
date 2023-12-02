#pragma once

#include <stdio.h>

namespace Alpha::Core
{
	constexpr decltype(auto) cConsoleTitle_Name = _T("Alpha::Console");

	class Console final : public Singleton<Console>
	{
		friend Singleton<Console>;
		Console();
	};
}