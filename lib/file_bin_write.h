#include <fstream>
#include <string>
#include <type_traits>
#include <stdexcept>
#include <memory>

class FileBinWrite {
public:
    explicit FileBinWrite(const std::string& filename) 
        : file_(filename, std::ios::binary) {
        if (!file_) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
    }

    // simple char*
    void writeChar(const char* data, size_t size) {
        file_.write(data, size);
        checkStream();
    }

    // int, float, double
    template<typename T>
    typename std::enable_if<std::is_pod<T>::value>::type
    write(const T& value) {
        file_.write(reinterpret_cast<const char*>(&value), sizeof(T));
        checkStream();
    }

    // arrays int, float, double
    template<typename T>
    typename std::enable_if<std::is_pod<T>::value>::type
    writeArray(const T* data, size_t count) {
        write(static_cast<uint32_t>(count));
        file_.write(reinterpret_cast<const char*>(&data[0]), count * sizeof(T));
        checkStream();
    }

// template for Own Structure
    template<typename T>
    void writeOwnArray(const T& array) {
//        using value_type = typename T::value_type;
        using value_type = std::remove_reference_t<decltype(array[0])>;
        write(static_cast<uint32_t>(array.size()));
        file_.write(reinterpret_cast<const char*>(&array[0]), array.size() * sizeof(value_type));
        checkStream();
    }

    void close() {
        file_.close();
    }

    ~FileBinWrite() {
        if (file_.is_open()) {
            file_.close();
        }
    }

private:
    std::ofstream file_;

    void checkStream() {
        if (!file_) {
            throw std::runtime_error("Error writing to file");
        }
    }
};