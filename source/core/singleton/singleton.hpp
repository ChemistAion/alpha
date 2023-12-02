#pragma once

namespace Alpha::Core
{
    ////////////////////////////////////////////////////////////////////////////////

    template <typename Type>
    class Singleton
    {
        static std::atomic<std::weak_ptr<Type>> singleton_;
        static std::mutex lock_;

        struct Deleter
        {
            void operator ()(Type* ptr)
            {
                delete ptr;
            }
        };

        static Deleter deleter_;
    protected:
        Singleton() = default;
    public:
        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton) = delete;

        static std::shared_ptr<Type> GetInstance();
        //static std::size_t GetInstanceCount();
    };

    ////////////////////////////////////////////////////////////////////////////////

	//TODO: C++20 has it
    template <typename Type>
    std::atomic<std::weak_ptr<Type>> Singleton<Type>::singleton_;

    template <typename Type>
    std::mutex Singleton<Type>::lock_;

    template <typename Type>
    typename Singleton<Type>::Deleter Singleton<Type>::deleter_;

    template <typename Type>
    std::shared_ptr<Type> Singleton<Type>::GetInstance()
    {
        std::shared_ptr<Type> instance;
        
        if (instance = singleton_.load(std::memory_order_acquire).lock(); instance)
        {
            return instance;
        }

        const std::lock_guard<std::mutex> lock(lock_);

        //TODO: try/catch
      
        if (instance = singleton_.load(std::memory_order_relaxed).lock(); !instance)
        {
            instance.reset(new Type(), deleter_);
            singleton_.store(instance, std::memory_order_release);
        }

        return instance;
    }

	//	template <typename Type>
	//	std::size_t Singleton<Type>::GetInstanceCount()
	//	{
	//	    return singleton_.load().use_count();
	//	}
}