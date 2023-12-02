#pragma once

namespace Alpha::Core::Config::JSON
{
	namespace Detial
	{
		MetaValue GetMetaFromDOM(const simdjson::dom::element& element) noexcept;
	}

	MetaObject GetMetaFromJSON(const Core::Filesystem::Stream& stream) noexcept;
}