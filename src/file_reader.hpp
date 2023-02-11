#pragma once

#include <string>
#include <fstream>

// From RPKGv3
class FileReader {
public:
    explicit FileReader(const std::string& path) {
        file = std::ifstream(path, std::ios_base::binary | std::ios_base::ate);
        size = file.tellg();
        file.seekg(0);
    }

    ~FileReader() { file.close(); }

    template<typename T>
    void Read(T* data) { file.read(reinterpret_cast<char*>(data), sizeof(T)); }

    template<typename T>
    void Read(T* data, size_t count) { file.read(reinterpret_cast<char*>(data), sizeof(T) * count); }

    void SeekBy(size_t length) { file.seekg((size_t) file.tellg() + length); }

    void SeekTo(size_t offset) { file.seekg(offset); }

    size_t Position() { return file.tellg(); }

    [[nodiscard]] size_t Size() const { return size; }

private:
    std::ifstream file;
    size_t size = 0;
};