#pragma once

namespace Alpha::Core
{
    template <typename Key>
    class SparseSet
    {
        std::vector<Key> packed_;
        std::vector<std::size_t> sparse_;
    public:
        using Iterator = typename std::vector<Key>::iterator;
        using ConstIterator = typename std::vector<Key>::const_iterator;

        Iterator begin() noexcept
        {
            return packed_.begin();
        }

        Iterator end() noexcept
        {
            return packed_.end();
        }

        ConstIterator begin() const noexcept
        {
            return packed_.begin();
        }

        ConstIterator end() const noexcept
        {
            return packed_.end();
        }

        ConstIterator cbegin() const noexcept
        {
            return packed_.cbegin();
        }

        ConstIterator cend() const noexcept
        {
            return packed_.cend();
        }

        ////////////////////////////////////////////////////////////////////////////////

        SparseSet() = default;

        SparseSet(const SparseSet&) = default;
        SparseSet& operator=(const SparseSet&) = default;

        SparseSet(SparseSet&&) noexcept = default;
        SparseSet& operator=(SparseSet&&) noexcept = default;

        virtual ~SparseSet() = default;

        ////////////////////////////////////////////////////////////////////////////////

        [[nodiscard]] constexpr std::size_t Size() const noexcept
        {
            return packed_.size();
        }

        constexpr void Reserve(const std::size_t capacity)
        {
            packed_.reserve(capacity);
        }

        [[nodiscard]] constexpr std::size_t Capacity() const noexcept
        {
            return packed_.capacity();
        }

        [[nodiscard]] constexpr bool Empty() const noexcept
        {
            return packed_.empty();
        }

		//[[nodiscard]] const Type* Data() const noexcept
		//{
		//    return packed_.data();
		//}

        constexpr void Clear() noexcept
        {
            packed_.clear();
        }

        ////////////////////////////////////////////////////////////////////////////////

        [[nodiscard]] constexpr std::size_t Index(const Key key) const
        {
            assert(Contains(key));
			return sparse_[key];
        }

        [[nodiscard]] constexpr bool Contains(const Key key) const noexcept
        {
			return
                key < sparse_.size() &&
                sparse_[key] < packed_.size() &&
                packed_[sparse_[key]] == key;
        }

        [[nodiscard]] ConstIterator Find(const Key key) const noexcept
        {
            return Contains(key)
                ? begin() + sparse_[key]
                : end();
        }

        void Emplace(const Key key)
        {
            assert(!Contains(key));

            if (key >= sparse_.size())
            {
                //TODO: check max size vs number of types
				//TODO: uniform <key>, next/++key etc.
                sparse_.resize(std::max(sparse_.size() * 2u, std::size_t(key + 1u)));
            }

            packed_.push_back(key);
            sparse_[key] = packed_.size() - 1u;
        }

        void Erase(const Key key)
        {
            assert(Contains(key));

            const auto packed_index = sparse_[key];

            if (packed_index != (packed_.size() - 1u))
            {
                auto element = std::move(packed_.back());
                packed_[packed_index] = std::move(element);

                sparse_[packed_[packed_index]] = packed_index;
            }

            packed_.pop_back();
        }
    };
}