#ifndef RING_BUFFER_INCLUDED
#define RING_BUFFER_INCLUDED

#include <inttypes.h>
#include "std_types.hpp"

namespace etl {

template <typename ElementType, size_t Size>
class ring_buffer {
    public:
        typedef ElementType        value_type;
        typedef value_type       * iterator;
        typedef value_type const * const_iterator;
        typedef value_type       & reference;
        typedef value_type const & const_reference;

        ring_buffer() :
            begin_( buffer_ ),
            end_( buffer_)
        {}

        inline
        iterator
        begin() {
            return begin_; 
        }

        inline
        const_iterator
        begin() const {
            return begin_; 
        }

        inline
        iterator
        end() {
            return end_; 
        }

        inline
        const_iterator
        end() const { 
            return end_; 
        }

        inline
        reference
        front() {
            return * begin_; 
        }

        inline
        const_reference
        front() const {
            return * begin_; 
        }

        inline
        reference
        back() {
            if (end_ == buffer_) {
                return buffer_[ACTUAL_SIZE - 1];
            }
            return * (end_ - 1); 
        }

        inline
        const_reference
        back() const {
            if (end_ == buffer_) {
                return buffer_[ACTUAL_SIZE - 1];
            }
            return * (end_ - 1); 
        }

        /* XXX: write some tests*/
        inline
        void
        push_front(ElementType const& e) {
            if (begin_ == buffer_) {
                begin_ = buffer_ + (ACTUAL_SIZE-1);
            } else {
                begin_ += 1;
            }
            *begin_ = e;
        }

        inline
        void
        push_back(ElementType const& e) {
            *end_ = e;
            end_ += 1;
            if (end_ > buffer_ + (ACTUAL_SIZE-1)) {
                end_ = buffer_;
            }
        }

        inline
        void
        pop_front() {
            begin_ += 1;
            if (begin_ > buffer_ + (ACTUAL_SIZE- 1)) {
                begin_ = buffer_;
            }
        }

        inline
        size_t
        size() const {
            if (begin_ <= end_) {
                return  end_ - begin_;
            }
            return ACTUAL_SIZE - (begin_ - end_);
        }

        inline
        size_t
        capacity() const {
            return Size;
        }

        inline
        bool
        empty() const {
            return begin_ == end_;
        }

        inline
        bool
        full() const {
            return size() == capacity();
        }

        inline
        void
        clear() {
            end_ = begin_;
        }

    private:
        enum { ACTUAL_SIZE = Size + 1 };
        ElementType buffer_[ACTUAL_SIZE];
        ElementType * begin_;
        ElementType * end_;
};

}
#endif // RING_BUFFER_INCLUDED
