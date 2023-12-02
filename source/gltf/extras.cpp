#include "_gltf.hpp"

namespace Alpha::glTF
{
	Extras GetExtras(const simdjson::dom::element& element)
	{
		return GetValue(element);
	}
}
