#pragma once

#include <string>

// From RPKGv3
class StreamReader {
public:
    StreamReader(void* stream, size_t size) {
        this->stream = stream;
        this->size = size;
    }

    template<typename T>
    void Read(T* data) {
        memcpy(data, reinterpret_cast<T*>(StreamPosition()), sizeof(T));
        position += sizeof(T);
    }

    template<typename T>
    void Read(T* data, size_t count) {
        memcpy(data, reinterpret_cast<T*>(StreamPosition()), sizeof(T) * count);
        position += sizeof(T) * count;
    }

    void Read(std::string& s) {
        memcpy(s.data(), reinterpret_cast<char*>(StreamPosition()), s.length());
        position += s.length();
    }

    template<typename T>
    void Peek(T* data, size_t offset) {
        memcpy(data, reinterpret_cast<T*>(StreamPosition() + offset), sizeof(T));
    }

    void SeekBy(size_t length) { position += length; }

    void SeekTo(size_t offset) { position = offset; }

    [[nodiscard]] size_t Size() const { return size; }

    size_t StreamPosition() { return reinterpret_cast<uintptr_t>(stream) + position; }

private:
    void* stream;
    size_t size = 0;
    size_t position = 0;
};