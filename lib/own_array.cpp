#include <stdexcept>
#include <iostream>
#include <algorithm>
#include "own_array.h"

// class OwnArray {
//    public:
        OwnArray::OwnArray(size_t size) : size_(size), data_(nullptr) {
            data_ = new float[size_];
            if (data_ == nullptr) {
                throw std::runtime_error("Memory allocation failed.");
            }
        }
    
        OwnArray::~OwnArray() {
            delete[] data_;
        }

        OwnArray::OwnArray(OwnArray&& other) noexcept
            : size_(other.size_), data_(other.data_) {
            other.size_ = 0;
            other.data_ = nullptr;
        }
        
        OwnArray& OwnArray::operator=(OwnArray&& other) noexcept {
            if (this != &other) {
                delete[] data_;
                size_ = other.size_;
                data_ = other.data_;
                other.size_ = 0;
                other.data_ = nullptr;
            }
            return *this;
        }
    
        float& OwnArray::operator[](size_t index) {
            return data_[index];
        }
    
        const float& OwnArray::operator[](size_t index) const {
            return data_[index];
        }
    
        size_t OwnArray::size() const {
            return size_;
        }

        void OwnArray::resize(size_t new_size) {
            if (new_size == size_) {
                return;
            }
    
            float* new_data = nullptr;
            if (new_size > 0) {
                new_data = new float[new_size];
                if (new_data == nullptr) {
                    throw std::runtime_error("Memory allocation failed.");
                }
    
                size_t elements_to_copy = std::min(size_, new_size);
                for (size_t i = 0; i < elements_to_copy; ++i) {
                    new_data[i] = data_[i];
                }
            }
    
            delete[] data_;
            data_ = new_data;
            size_ = new_size;
        }
            
//    private:
//        size_t size_;
//        float* data_;
//    }; // END class OwnArray
