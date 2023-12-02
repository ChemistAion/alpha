#include "_gltf.hpp"

#include "node.hpp"

namespace Alpha::glTF
{
	Nodes GetNodes(const simdjson::dom::element& element)
	{
		Nodes nodes;

		enum class State
		{
			None,
			Matrix,
			TRS
		};

		const auto& items = Get<simdjson::dom::array>(element);
		nodes.reserve(items.size());

		for (const auto item : items)
		{
			State state = State::None;

			const auto object = Get<simdjson::dom::object>(item);
			auto& node = nodes.emplace_back();

			for (const auto [key, value] : object)
			{
				if (key == "camera")
				{
					node.camera_ = Get<std::uint64_t>(value);
					continue;
				}

				if (key == "children")
				{
					const auto array = Get<simdjson::dom::array>(value);

					for (const auto item : array)
					{
						node.children_.push_back(Get<std::uint64_t>(item));
					}
					continue;
				}

				if (key == "skin")
				{
					node.skin_ = Get<std::uint64_t>(value);
					continue;
				}

				if (key == "matrix")
				{
					assert(state == State::None);
					state = State::Matrix;

					auto vector = GetVector<float>(value);
					assert(vector.size() == 16);

					node.transform_ = XMMATRIX(vector.data());
					continue;
				}

				if (key == "translation")
				{
					assert(state == State::None || state == State::TRS);
					
					if (state == State::None)
					{
						node.transform_ = Transform();
					}

					state = State::TRS;
				
					auto vector = GetVector<float>(value);
					assert(vector.size() == 3);

					auto& translation = std::get<Transform>(node.transform_).translation_;
					translation = XMFLOAT3(vector.data());
					continue;
				}
				
				if (key == "rotation")
				{
					assert(state == State::None || state == State::TRS);

					if (state == State::None)
					{
						node.transform_ = Transform();
					}

					state = State::TRS;

					auto vector = GetVector<float>(value);
					assert(vector.size() == 4);

					auto& rotation = std::get<Transform>(node.transform_).rotation_;
					rotation = XMFLOAT4(vector.data());
					continue;
				}

				if (key == "scale")
				{
					assert(state == State::None || state == State::TRS);

					if (state == State::None)
					{
						node.transform_ = Transform();
					}

					state = State::TRS;

					auto vector = GetVector<float>(value);
					assert(vector.size() == 3);

					auto& scale = std::get<Transform>(node.transform_).scale_;
					scale = XMFLOAT3(vector.data());
					continue;
				}

				if (key == "mesh")
				{
					node.mesh_ = Get<std::uint64_t>(value);
					continue;
				}

				if (key == "weights")
				{
					assert(false);
					continue;
				}

				////////////////////////////////////////////////////////////////////////////////

				if (key == "name")
				{
					node.name_ = Get<std::string_view>(value);
					continue;
				}

				if (key == "extensions")
				{
					assert(false);
					continue;
				}

				if (key == "extras")
				{
					node.extras_ = std::move(GetExtras(value));
					continue;
				}

				assert(false);
			}
		
			if (state == State::None)
			{
				node.transform_ = XMMatrixIdentity();
			}
		}
	
		return nodes;
	}
} 
