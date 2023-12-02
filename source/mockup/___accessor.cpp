#include "_mockup.hpp"

#include "accessor.hpp"

namespace Alpha::Mockup
{
	Accessor ImportAccessor(glTF::Index index, const glTF::Model& model)
	{
		Accessor result;

		const auto& accessor = model.accessors_.at(index);

		assert(accessor.buffer_view_.has_value());
		const auto& view = model.views_.at(accessor.buffer_view_.value());

		assert(accessor.offset_ == 0);

		//auto offset = view.offset_;
		//auto size = view.size_;

		const auto& buffer = model.buffers_.at(view.buffer_);

		result.data_ = &buffer.data_.at(view.offset_);
		result.count_ = accessor.count_;
		result.dimension_ = accessor.type_.second;
		result.type_size_ = accessor.component_type_.second;

		assert(view.size_ == (accessor.count_ * accessor.type_.second * accessor.component_type_.second));
		assert(view.stride_ == 0);

		return result;
	}
}

