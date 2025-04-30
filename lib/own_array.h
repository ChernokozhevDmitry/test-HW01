#include <cstddef>

class OwnArray {
    public:
        OwnArray(size_t size);
        ~OwnArray();
        OwnArray(const OwnArray&) = delete;
        OwnArray& operator=(const OwnArray&) = delete;
        OwnArray(OwnArray&& other) noexcept;
        OwnArray& operator=(OwnArray&& other) noexcept;
        float& operator[](size_t index);
        const float& operator[](size_t index) const;
        size_t size() const;
        void resize(size_t new_size);
    private:
        size_t size_;
        float* data_;
    public:
        using value_type = float;
};