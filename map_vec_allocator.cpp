#include <iostream>
#include <memory> 
#include <vector> 
#include <map>
#include <cstddef>
#include <new>
#include <type_traits>

// VECTOR
template<typename T, std::size_t MAX_SIZE>
class MyVector {
private:
    T* data;           
    size_t count;      
    size_t capacity;   
    std::allocator<T> alloc; 

public:
    
    explicit MyVector(std::size_t max_size = MAX_SIZE) : count(0), capacity(max_size) {
        data = alloc.allocate(capacity); 
    }

    ~MyVector() {
        if (data) {
            for (size_t i = 0; i < count; ++i) {
                std::allocator_traits<std::allocator<T>>::destroy(alloc, data + i); 
            }
            alloc.deallocate(data, capacity); 
        }
    }

    MyVector(const MyVector& other) : count(other.count), capacity(other.capacity) {
        data = alloc.allocate(capacity);
        std::uninitialized_copy_n(other.begin(), count, data); 
    }

    MyVector& operator=(const MyVector& other) {
        if (this != &other) {
            for (size_t i = 0; i < count; ++i) {
                std::allocator_traits<std::allocator<T>>::destroy(alloc, data + i);
            }
            alloc.deallocate(data, capacity);
            
            count = other.count;
            capacity = other.capacity;
            data = alloc.allocate(capacity);
            std::uninitialized_copy_n(other.begin(), count, data);
        }
        return *this;
    }

    void push_back(const T& value) {
        if (count >= capacity) {
            throw std::length_error("Vector is full");
        }
        std::allocator_traits<std::allocator<T>>::construct(alloc, data + count, value);
        ++count;
    }

    T& at(size_t index) {
        if (index >= count) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }

    const T& at(size_t index) const {
        if (index >= count) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }

    bool empty() const {
        return count == 0;
    }

    size_t size() const {
        return count;
    }

    void pop_back() {
        if (!empty()) {
            std::allocator_traits<std::allocator<T>>::destroy(alloc, data + (--count)); 
        }
    }

    void clear() {
        for (size_t i = 0; i < count; ++i) {
            std::allocator_traits<std::allocator<T>>::destroy(alloc, data + i); 
        }
        count = 0; 
    }

    T* begin() {
        return data;
    }

    T* end() {
        return data + count;
    }

    const T* cbegin() const {
        return data;
    }

    const T* cend() const {
        return data + count;
    }
};
// END VECTOR

// MAP
template<typename T, std::size_t MALLOC_SIZE>
class SizeParamAllocator {
private:
    alignas(T) char buffer[MALLOC_SIZE * sizeof(T)];
    
    char* free_memory = buffer;
    
    size_t available_bytes = MALLOC_SIZE * sizeof(T);

public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    SizeParamAllocator() noexcept {}

    template<class U>
    SizeParamAllocator(const SizeParamAllocator<U, MALLOC_SIZE>&) noexcept {}

    T* allocate(std::size_t n) {
        if (n > MALLOC_SIZE) {
            throw std::bad_alloc();
        }

        if (n * sizeof(T) > available_bytes) {
            throw std::bad_alloc();
        }

        T* result = reinterpret_cast<T*>(free_memory);
        free_memory += n * sizeof(T);
        available_bytes -= n * sizeof(T);
        return result;
    }

    void deallocate(T* p, std::size_t n) {
        if (p < reinterpret_cast<T*>(buffer) || p >= reinterpret_cast<T*>(buffer + MALLOC_SIZE * sizeof(T))) {
            throw std::runtime_error("Invalid pointer");
        }

        free_memory = reinterpret_cast<char*>(p);
        available_bytes += n * sizeof(T);
    }

    template<class U>
    struct rebind {
        using other = SizeParamAllocator<U, MALLOC_SIZE>;
    };
};
// END MAP

int factorial(int n) {
    if (n == 0)
        return 1;
    return n * factorial(n - 1);
}


int main() {
// VECTOR
    std::vector<int> stdvec; 
    MyVector<int, 10> myvec; 

    for (int i = 0; i < 10; ++i) {
        stdvec.push_back(i);
    }

    std::cout << "stdvec : ";
    for (size_t i = 0; i < stdvec.size(); ++i) {
        std::cout << stdvec.at(i) << " ";
    }
    std::cout << std::endl;

    for (int i = 0; i < 10; ++i) {
        myvec.push_back(i);
    }

    std::cout << "myvec : ";
    for (size_t i = 0; i < myvec.size(); ++i) {
        std::cout << myvec.at(i) << " ";
    }
    std::cout << std::endl;

// END VECTOR

// MAP
    std::map<int, int> stdMap;
    std::map<int, int, std::less<int>, SizeParamAllocator<std::pair<const int, int>, 11>> myMap;

    for (int i = 0; i < 10; ++i) {
        stdMap[i] = factorial(i);
    }

    for (const auto& [key, value] : stdMap) {
        std::cout << "stdMap " << key << " " << value << std::endl;
    }

    for (int i = 0; i < 10; ++i) {
        myMap[i] = factorial(i);
    }

    for (const auto& [key, value] : myMap) {
        std::cout << "myMap " << key << " " << value << std::endl;
    }
// END MAP

    return 0;
}