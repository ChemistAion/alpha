#pragma once

namespace Alpha::glTF
{
	using Extras = Value;

	Extras GetExtras(const simdjson::dom::element& element);
}