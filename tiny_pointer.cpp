
/*
* Instead of full 64-bit pointers, store a small offset (say 16 or 32 bits).
* Offset is relative to a known base address (start of the memory pool).
* You save space because you don’t store the full address.
* All memory is assumed to be in a known pool (like a slab or arena).
* Instead of storing a full pointer (8 bytes on x64), you store just a 2-byte offset (huge win for small objects).
* You can pack this with other metadata (e.g., freelist, tags, etc.).
 + Enhancements
- Use uint8_t, uint16_t, or uint32_t offsets depending on your pool size.
- Support multiple pools by tagging high bits.
- Combine this with freelist pointer encoding (next offset in pool).
*/

#include <iostream>
#include <cstdint>
#include <vector>
#include <cassert>
#include <cstdlib>
#include <cstring>

constexpr size_t POOL_SIZE = 1024; // bytes

// Memory pool
uint8_t memory_pool[POOL_SIZE];

// Tiny pointer: stores 16-bit offset from pool base
template <typename T>
class TinyPtr {
private:
    uint16_t offset; // 16-bit offset

public:
    TinyPtr() : offset(0) {}

    TinyPtr(T* ptr) {
        set(ptr);
    }

    void set(T* ptr) {
        uintptr_t base = reinterpret_cast<uintptr_t>(memory_pool);
        uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);

        assert(addr >= base && addr < base + POOL_SIZE);
        offset = static_cast<uint16_t>(addr - base);
    }

    T* get() const {
        return reinterpret_cast<T*>(memory_pool + offset);
    }

    T& operator*() const {
        return *get();
    }

    T* operator->() const {
        return get();
    }
};

// Simple struct stored in pool
struct Data {
    int value;
};

int main() {
    // Place Data object in the memory pool manually
    Data* obj = new (memory_pool + 100) Data{123}; // placement new

    TinyPtr<Data> tp(obj);

    std::cout << "TinyPtr value: " << tp->value << std::endl;

    // Modify through tiny pointer
    tp->value = 456;
    std::cout << "Modified value: " << tp->value << std::endl;

    return 0;
}
