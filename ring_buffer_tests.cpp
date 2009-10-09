#include <iostream>

#include "ring_buffer.hpp"

void
assert_element(size_t a, size_t b) {
    if (a != b) {
        std::cerr << "FAILED: expected element " << a << " but got " << b << std::endl;
        throw 23;
    }
}

template <typename T, size_t Size>
void
assert_empty(ring_buffer<T, Size> const & buffer) {
    if ( ! buffer.empty() ) {
        std::cerr << "FAILED: buffer not empty" << std::endl;
        throw 23;
    }
}

template <typename T, size_t Size>
void
assert_size(ring_buffer<T, Size> const & buffer, size_t expected_size) {
    if (buffer.size() != expected_size) {
        std::cerr << "FAILED: Expected buffer size of " << expected_size <<
                " but buffer has " << buffer.size() << " elements." << std::endl;
        throw 23;
    }
}

int main() {
    ring_buffer<size_t, 5> buf;
    for (size_t i = 0; i < 5; ++i) {
        assert_size(buf, i);
        buf.push_back(i);
    }
    if ( ! buf.full() ) {
        std::cerr << "Buffer is not full" << std::endl;
        throw 23;
    }
    for (size_t i = 0; i < 5; ++i) {
        assert_size(buf, 5 - i);
        int e = buf.front();
        buf.pop_front();
        assert_element(i, e);
    }

    assert_empty( buf );
    for (size_t i = 0; i < 10000; ++i) {
        assert_size(buf, 0);
        buf.push_back(i);
        assert_size(buf, 1);
        buf.push_back(i);
        assert_size(buf, 2);
        buf.push_back(i);
        assert_size(buf, 3);

        assert_element(i, buf.back());
        buf.pop_front();
        assert_size(buf, 2);
        buf.pop_front();
        assert_size(buf, 1);
        assert_element(i, buf.front());
        
        buf.pop_front();
        assert_size(buf, 0);
        assert_empty( buf );
    }

    return 0;
}
