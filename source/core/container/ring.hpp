#pragma once

namespace Alpha::Core
{
	//TODO: concept/template...
	//TODO: is numeric

	template <typename Type>
	class Ring
	{
		const Type total_ {};

		Type head_ {};
		Type size_ {};
	public:
		Ring(Type total)
			: total_ { total }
		{
		}

		[[nodiscard]] auto GetHead() const noexcept
		{
			return head_;
		}

		[[nodiscard]] auto GetSize() const noexcept
		{
			return size_;
		}

		[[nodiscard]] auto GetTail() const noexcept
		{
			return (head_ + size_) % total_;
		}

		////////////////////////////////////////////////////////////////////////////////

		//TODO: check likely/unlikely
		Type Padding(Type size) const noexcept
		{
			if (auto tail = GetTail(); (tail + size) > total_) [[unlikely]]
			{			
				return total_ - tail;
			}
			
			//TODO: check it
			return {};
		}

		//TODO: check likely/unlikely
		bool Allocate(Type size, Type* output)
		{
			if (size_ + size <= total_) [[likely]]
			{
				if (output != nullptr)
				{
					*output = GetTail();
				}
				
				size_ += size;
				return true;
			}
			
			//TODO: throw...
			assert(false);
			return false;		
		}

		bool Free(Type size) noexcept
		{
			if (size_ >= size) [[likely]]
			{				
				head_ = (head_ + size) % total_;
				size_ -= size;

				return true;
			}

			assert(false);
			return false;
		}
	};
}