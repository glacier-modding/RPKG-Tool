#pragma once
#include <string>

class StreamWriter
{
public:
	StreamWriter(std::vector<char>* stream)
	{
		this->stream = stream;
	}

	template <typename T>
	void Write(T data)
	{
		Resize(sizeof(T));
		memcpy(reinterpret_cast<T*>(StreamPosition()), &data, sizeof(T));
		position += sizeof(T);
	}

	template <typename T>
	void Write(T data, size_t count)
	{
		Resize(sizeof(T) * count);

		for (size_t i = 0; i < count; i++)
		{
			memcpy(reinterpret_cast<T*>(StreamPosition()), &data, sizeof(T));
			position += sizeof(T);
		}
	}

	void Write(std::string& s)
	{
		Resize(s.length());
		memcpy(reinterpret_cast<char*>(StreamPosition()), s.data(), s.length());
		position += s.length();
	}

	void SeekTo(size_t offset) { position = offset; }

	void SeekToEnd() { position = stream->size(); }

	size_t GetPosition() { return position; }

	size_t StreamPosition() { return reinterpret_cast<uintptr_t>(stream->data()) + position; }

	void Resize(size_t length)
	{
		if (position + length > stream->size())
		{
			stream->resize(position + length);
		}
	}

private:
	std::vector<char>* stream;
	size_t position = 0;
};