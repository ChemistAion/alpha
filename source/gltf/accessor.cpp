#include "_gltf.hpp"

#include "accessor.hpp"

namespace Alpha::glTF
{
	Accessors GetAccessors(const simdjson::dom::element& element)
	{
		Accessors accessors;

		const auto& items = Get<simdjson::dom::array>(element);
		accessors.reserve(items.size());

		for (const auto item : items)
		{
			const auto object = Get<simdjson::dom::object>(item);
			auto& accessor = accessors.emplace_back();

			for (const auto [key, value] : object)
			{
				if (key == "bufferView")
				{
					accessor.buffer_view_ = Get<std::uint64_t>(value);
					continue;
				}

				if (key == "byteOffset")
				{
					accessor.offset_ = Get<std::uint64_t>(value);
					continue;
				}

				if (key == "count")
				{
					accessor.count_ = Get<std::uint64_t>(value);
					continue;
				}

				if (key == "componentType")
				{
					//TODO: check it...
					auto component_type = Get<std::uint64_t>(value);

					if (component_type == 5120)
					{
						accessor.component_type_ = { Accessor::ComponentType::Byte, 1 };
						continue;
					}
					if (component_type == 5121)
					{
						accessor.component_type_ = { Accessor::ComponentType::UnsignedByte, 1 };
						continue;
					}
					if (component_type == 5122)
					{
						accessor.component_type_ = { Accessor::ComponentType::Short, 2 };
						continue;
					}
					if (component_type == 5123)
					{
						accessor.component_type_ = { Accessor::ComponentType::UnsignedShort, 2 };
						continue;
					}
					if (component_type == 5124)
					{
						accessor.component_type_ = { Accessor::ComponentType::Int, 4 };
						continue;
					}
					if (component_type == 5125)
					{
						accessor.component_type_ = { Accessor::ComponentType::UnsignedInt, 4 };
						continue;
					}
					if (component_type == 5126)
					{
						accessor.component_type_ = { Accessor::ComponentType::Float, 4 };
						continue;
					}

					assert(false);
					continue;
				}

				if (key == "normalized")
				{
					//TODO: check it...
					assert(false);
					continue;
				}

				if (key == "type")
				{
					auto type = Get<std::string_view>(value);

					if (type == "SCALAR")
					{
						accessor.type_ = { Accessor::Type::Scalar, 1 };
						continue;
					}

					if (type == "VEC2")
					{
						accessor.type_ = { Accessor::Type::Vec2, 2 };
						continue;
					}

					if (type == "VEC3")
					{
						accessor.type_ = { Accessor::Type::Vec3, 3 };
						continue;
					}

					if (type == "VEC4")
					{
						accessor.type_ = { Accessor::Type::Vec4, 4 };
						continue;
					}

					if (type == "MAT2")
					{
						accessor.type_ = { Accessor::Type::Mat2, 2 * 2 };
						continue;
					}

					if (type == "MAT3")
					{
						accessor.type_ = { Accessor::Type::Mat3, 3 * 3 };
						continue;
					}

					if (type == "MAT4")
					{
						accessor.type_ = { Accessor::Type::Mat4, 4 * 4 };
						continue;
					}

					assert(false);
					continue;
				}

				if (key == "min")
				{
					accessor.min_= GetVector<double>(value);
					continue;
				}

				if (key == "max")
				{
					accessor.max_ = GetVector<double>(value);
					continue;
				}

				if (key == "sparse")
				{
					//TODO: check it...
					assert(false);
					continue;
				}

				////////////////////////////////////////////////////////////////////////////////

				if (key == "name")
				{
					accessor.name_ = Get<std::string_view>(value);
					continue;
				}

				if (key == "extensions")
				{
					continue;
				}

				if (key == "extras")
				{
					accessor.extras_ = std::move(GetExtras(value));
					continue;
				}

				assert(false);
			}
		}

		//TODO: checks...
		//min/max size equal?

		return accessors;
	}
}