#ifndef BEE_FISH__DATABASE__ALLOCATOR_HPP
#define BEE_FISH__DATABASE__ALLOCATOE_HPP

#include <iomanip>
#include <memory>
#include <iostream>
#include "Branch.hpp"

namespace BeeFishDatabase
{
    
    template <class T>
    struct Allocator {
        using value_type = T;
        void* _memory;
        Index _size;
        Index _allocated;
        
        Allocator(void* memory, Index size) noexcept
            :
            _memory(memory),
            _size(size)
        {
        }
        
        template <class U>
        Allocator(const Allocator<U>&) noexcept {}

        T* allocate(std::size_t n) {
            // Your custom allocation logic here
            // Example: using new operator (default behavior, replace as needed)
            std::cout << "Allocating " << n << " objects of size " << sizeof(T) << std::endl;
            if (_allocated + n * sizeof(T) > _size)
                _allocated = 0;
                
            T* memory = static_cast<T*>((void*)((char*)_memory + _allocated));
            _allocated += n * sizeof(T);
            
            return memory;
        }

        void deallocate(T* p, std::size_t n) noexcept {
            // Your custom deallocation logic here
            // Example: using delete operator (default behavior, replace as needed)
            std::cout << "Deallocating " << n << " objects" << std::endl;
         //   ::operator delete(p);
        }
    };

    // Required for stateless allocators (most minimal allocators are stateless)
    template <class T, class U>
    constexpr bool operator==(const Allocator<T>& a, const Allocator<U>& b) noexcept {
        return a._memory == b._memory; // Instances are always equal, can deallocate each other's memory
    }

    template <class T, class U>
    constexpr bool operator!=(const Allocator<T>& a, const Allocator<U>& b) noexcept {
        return a._memory == b._memory;
    }
    
}

#endif
