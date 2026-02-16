#include <memory>
#include <iostream>

template <class T>
struct CustomAllocator {
    using value_type = T;

    CustomAllocator() noexcept {}
    template <class U>
    CustomAllocator(const CustomAllocator<U>&) noexcept {}

    T* allocate(std::size_t n) {
        // Your custom allocation logic here
        // Example: using new operator (default behavior, replace as needed)
        std::cout << "Allocating " << n << " objects of size " << sizeof(T) << std::endl;
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    void deallocate(T* p, std::size_t n) noexcept {
        // Your custom deallocation logic here
        // Example: using delete operator (default behavior, replace as needed)
        std::cout << "Deallocating " << n << " objects" << std::endl;
        ::operator delete(p);
    }
};

// Required for stateless allocators (most minimal allocators are stateless)
template <class T, class U>
constexpr bool operator==(const CustomAllocator<T>&, const CustomAllocator<U>&) noexcept {
    return true; // Instances are always equal, can deallocate each other's memory
}

template <class T, class U>
constexpr bool operator!=(const CustomAllocator<T>&, const CustomAllocator<U>&) noexcept {
    return false;
}
