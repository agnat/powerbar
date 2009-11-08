#ifndef ETL_STATIC_STRING_INCLUDED
#define ETL_STATIC_STRING_INCLUDED

//#include "std_types.hpp"

#include <stddef.h>
#include <string.h>
#include <avr/pgmspace.h>

namespace etl {

template <class CharT, size_t Capacity>
class static_string {
    public:

        typedef CharT              value_type;
        typedef value_type       * iterator;
        typedef value_type const * const_iterator;
        typedef value_type       & reference;
        typedef value_type const & const_reference;

        enum {npos = size_t(-1)};

        static_string():
            end_(buffer_)
        {}

        inline
        iterator
        begin() {
            return buffer_; 
        }

        inline
        const_iterator
        begin() const {
            return buffer_; 
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

        bool
        operator == (value_type const * other) {
            value_type const * o_it(other);
            for(iterator it = begin(); it != end(); ++it) {
                if (*it != *o_it) {
                    return false;
                }
                ++o_it;
            }
            return true;
        }

        inline
        void
        append(value_type e) {
            *(end_++) = e;
        }

        inline
        iterator
        c_str() {
            *end_ = value_type();
            return begin();
        }

        inline
        void
        clear() {
            end_ = buffer_;
        }
        
        inline
        size_t
        size() const {
            return end_ - buffer_;
        }

        inline
        size_t
        capacity() const {
            return Capacity;
        }
        
        inline
        bool
        empty() const {
            return end_ == buffer_;
        }

        inline
        bool
        full() const {
            return size() == capacity();
        }

        inline
        size_t
        find(const value_type * str) {
            char * match = strstr(c_str(), str);
            if (match) {
                return match - begin();
            }
            return npos;
        }
        inline
        size_t
        find_P(PGM_P str) {
            char * match = strstr_P(c_str(), str);
            if (match) {
                return match - begin();
            }
            return npos;
        }
    private:
        value_type buffer_[Capacity+1];
        iterator   end_;
};

} // end of namespace etl
#endif // ETL_STATIC_STRING_INCLUDED
