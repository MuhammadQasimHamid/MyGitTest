#ifndef VECTOR_H
#define VECTOR_H

#include <cstddef>
#include <stdexcept>

template <typename T>
class Vector
{
private:
    T *data;
    size_t size;
    size_t capacity;

public:
    Vector()
        : data(nullptr), size(0), capacity(0) {}

    explicit Vector(size_t n)
        : data(new T[n]), size(0), capacity(n) {}

    ~Vector()
    {
        delete[] data;
    }

    Vector(const Vector &other)
        : data(new T[other.capacity]), size(other.size), capacity(other.capacity)
    {
        for (size_t i = 0; i < size; i++)
            data[i] = other.data[i];
    }

    Vector &operator=(const Vector &other)
    {
        if (this == &other)
            return *this;

        delete[] data;

        capacity = other.capacity;
        size = other.size;
        data = new T[capacity];

        for (size_t i = 0; i < size; i++)
            data[i] = other.data[i];

        return *this;
    }

    Vector(Vector &&other) noexcept
        : data(other.data), size(other.size), capacity(other.capacity)
    {
        other.data = nullptr;
        other.size = 0;
        other.capacity = 0;
    }

    Vector &operator=(Vector &&other) noexcept
    {
        if (this == &other)
            return *this;

        delete[] data;

        data = other.data;
        size = other.size;
        capacity = other.capacity;

        other.data = nullptr;
        other.size = 0;
        other.capacity = 0;

        return *this;
    }

    void push_back(const T &value)
    {
        if (size >= capacity)
        {
            size_t newCap = (capacity == 0) ? 1 : capacity * 2;
            T *newData = new T[newCap];

            for (size_t i = 0; i < size; i++)
                newData[i] = data[i];

            delete[] data;
            data = newData;
            capacity = newCap;
        }

        data[size++] = value;
    }

    void pop_back()
    {
        if (size == 0)
            throw std::out_of_range("Vector is empty");

        size--;
    }

    T &operator[](size_t index) { return data[index]; }
    const T &operator[](size_t index) const { return data[index]; }

    T &at(size_t index)
    {
        if (index >= size)
            throw std::out_of_range("Index out of range");
        return data[index];
    }

    size_t Size() const { return size; }
    size_t Capacity() const { return capacity; }
    bool empty() const { return size == 0; }

    T *begin() { return data; }
    T *end() { return data + size; }
    const T *begin() const { return data; }
    const T *end() const { return data + size; }

    void clear() { size = 0; }
};

#endif
