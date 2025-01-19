#ifndef STABLE_VECTOR_HPP
#define STABLE_VECTOR_HPP

#include <vector>

template <typename T>
class stable_vector {
private:
    std::vector<T> data;

public:
    using iterator = typename std::vector<T>::iterator;
    using const_iterator = typename std::vector<T>::const_iterator;

    // Add an element to the vector
    void push_back(const T& value) {
        data.push_back(value);
    }

    // Access element at index
    T& operator[](size_t index) {
        return data[index];
    }

    // Get size of the vector
    size_t size() const {
        return data.size();
    }

    // check if vector is empty
    bool empty() const {
        return data.empty();
    }

    // Begin iterator
    iterator begin() {
        return data.begin();
    }

    // End iterator
    iterator end() {
        return data.end();
    }

    // Const begin iterator
    const_iterator begin() const {
        return data.begin();
    }

    // Const end iterator
    const_iterator end() const {
        return data.end();
    }
};

#endif // STABLE_VECTOR_HPP
