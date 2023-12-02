#include "_mockup.hpp"

#include "model.hpp"

namespace Alpha::Mockup
{
	Mesh Model::ImportMesh(const glTF::Mesh& element, const glTF::Model& model)
	{
		Mesh mesh;
	
		//assert(element.name_.has_value());
		//mesh.primitives_.resize(element.primitives_.size());
	
		auto ImportBuffer = [&](glTF::Index index) -> std::pair<Index, Buffer>
		{
			assert(index < model.accessors_.size());
			const auto& accessor = model.accessors_[index];

			assert(accessor.buffer_view_.has_value());
			assert(accessor.buffer_view_.value() < model.views_.size());
			const auto& view = model.views_[accessor.buffer_view_.value()];

			assert(accessor.offset_ == 0);

			assert(view.buffer_ < buffers_data_.size());
			const auto& buffer_data = buffers_data_[view.buffer_];

			////////////////////////////////////////////////////////////////////////////////
	
			const auto count = accessor.count_;
			const auto dimension = accessor.type_.second;
			const auto type_size = accessor.component_type_.second;

			const auto size = count * dimension * type_size;
			assert(size > 0);
			assert(size == view.size_);

			Buffer buffer;
			auto& desc = buffer.first;
			auto& data = buffer.second;
	
			desc.stride_ = dimension * type_size;
			desc.size_ = size;
			//desc.usage_ = Engine::ResourceUsage::Static;
			//desc.bind_ = bind;
			
			assert((view.offset_ + size) <= buffer_data.Size());
			data.data_ = &buffer_data[view.offset_];
			data.size_ = size;
	
			return { index, buffer };
		};
	
		for (const auto& primitive : element.primitives_)
		{
			for (const auto& attribute : primitive.attributes_)
			{
				auto [index, buffer] = ImportBuffer(attribute.accessor_);
				auto& desc = buffer.first;

				desc.name_ = attribute.id_;
				desc.bind_ = Engine::ResourceBind::VertexBuffer;
				desc.usage_ = Engine::ResourceUsage::Static;

				mesh.buffers_.emplace_back(std::move(buffer));
			}
	
			if (primitive.indices_.has_value())
			{
				auto [index, buffer] = ImportBuffer(primitive.indices_.value());
				auto& desc = buffer.first;

				desc.name_ = "INDICES";
				desc.bind_ = Engine::ResourceBind::IndexBuffer;
				desc.usage_ = Engine::ResourceUsage::Static;

				mesh.indices_ = std::move(buffer);
			}
		}
	
		return mesh;
	}

	Data Model::ImportBufferData(const glTF::Buffer& element, const glTF::Model& model)
	{
		assert(!element.data_.empty());
		
		Data data(element.data_.size());
		std::memcpy(data.get(), element.data_.data(), data.Size());

		return data;
	}

    void Model::Import(const glTF::Model& model)
    {
		for (const auto& buffer : model.buffers_)
		{
			buffers_data_.emplace_back(ImportBufferData(buffer, model));
		}

		for (const auto& mesh : model.meshes_)
		{
			meshes_.emplace_back(ImportMesh(mesh, model));
		}

		return;
    }

//	void Model::Load(const std::string& file)
//	{
//	}
//	
//	void Model::Save(const std::string& file)
//	{
//	}

}