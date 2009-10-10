#ifndef ETL_STATIC_STRING_INCLUDED
#define ETL_STATIC_STRING_INCLUDED

#include "std_types.hpp"

namespace etl {

template <class CharT, size_t Capacity>
class static_string {
    public:

        typedef CharT              value_type;
        typedef value_type       * iterator;
        typedef value_type const * const_iterator;
        typedef value_type       & reference;
        typedef value_type const & const_reference;

        static_string():
            end_(buffer_)
        {
            *end_ = CharT();
        }

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
            *end_ = CharT();
        }

        inline
        iterator
        c_str() {
            return begin();
        }

        inline
        void
        clear() {
            end_ = buffer_;
            *end_ = value_type();
        }
    private:
        value_type buffer_[Capacity+1];
        iterator   end_;
};

}
#endif // ETL_STATIC_STRING_INCLUDED
