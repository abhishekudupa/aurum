// CartesianProduct.hpp ---
//
// Filename: CartesianProduct.hpp
// Author: Abhishek Udupa
// Created: Mon Jul  6 18:41:55 2015 (-0400)
//
//
// Copyright (c) 2015, Abhishek Udupa, University of Pennsylvania
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by The University of Pennsylvania
// 4. Neither the name of the University of Pennsylvania nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//

// Code:

#if !defined AURUM_RANGES_CARTESIAN_PRODUCT_HPP_
#define AURUM_RANGES_CARTESIAN_PRODUCT_HPP_

#include <tuple>

#include "../basetypes/AurumTypes.hpp"

namespace aurum {
namespace ranges {

namespace detail_ {

template <u64 INDEX, typename IteratorTupleType>
inline typename
std::enable_if<INDEX == std::tuple_size<typename std::decay<IteratorTupleType>::type>::value,
               bool>::type
try_increment_little_endian_at_index_(IteratorTupleType& the_tuple,
                                      const IteratorTupleType& begins,
                                      const IteratorTupleType& ends)
{
    return false;
}

template <u64 INDEX, typename IteratorTupleType>
inline typename
std::enable_if<INDEX != std::tuple_size<typename std::decay<IteratorTupleType>::type>::value,
               bool>::type
try_increment_little_endian_at_index_(IteratorTupleType& the_tuple,
                                      const IteratorTupleType& begins,
                                      const IteratorTupleType& ends)
{
    auto& cur_iterator = std::get<INDEX>(the_tuple);
    auto const& cur_end = std::get<INDEX>(ends);
    if (++cur_iterator == cur_end) {
        auto const& cur_begin = std::get<INDEX>(begins);
        cur_iterator = cur_begin;
        return try_increment_little_endian_at_index_<INDEX+1>(the_tuple, begins, ends);
    } else {
        return true;
    }
}

template <typename IteratorTupleType>
inline void increment_little_endian_iterator_(IteratorTupleType& the_tuple,
                                              const IteratorTupleType& begins,
                                              const IteratorTupleType& ends)
{
    auto const success =
        try_increment_little_endian_at_index_<0, IteratorTupleType>(the_tuple, begins, ends);
    if (!success) {
        the_tuple = ends;
    }
}

template <u64 INDEX, typename IteratorTupleType>
inline typename std::enable_if<INDEX == 0, bool>::type
try_increment_big_endian_at_index_(IteratorTupleType& the_tuple,
                                   const IteratorTupleType& begins,
                                   const IteratorTupleType& ends)
{
    auto& cur_iterator = std::get<0>(the_tuple);
    auto const& cur_end = std::get<0>(ends);

    if (++cur_iterator == cur_end) {
        return false;
    } else {
        return true;
    }
}

template <u64 INDEX, typename IteratorTupleType>
inline typename std::enable_if<INDEX != 0, bool>::type
try_increment_big_endian_at_index_(IteratorTupleType& the_tuple,
                                   const IteratorTupleType& begins,
                                   const IteratorTupleType& ends)
{
    auto& cur_iterator = std::get<INDEX>(the_tuple);
    auto const& cur_end = std::get<INDEX>(ends);

    if (++cur_iterator == cur_end) {
        auto const& cur_begin = std::get<INDEX>(begins);
        cur_iterator = cur_begin;
        return try_increment_big_endian_at_index_<INDEX-1>(the_tuple, begins, ends);
    } else {
        return true;
    }
}

template <typename IteratorTupleType>
inline void increment_big_endian_iterator_(IteratorTupleType& the_tuple,
                                           const IteratorTupleType& begins,
                                           const IteratorTupleType& ends)
{
    constexpr auto TUPLE_SIZE =
        std::tuple_size<typename std::decay<IteratorTupleType>::type>::value;

    auto const success =
        try_increment_big_endian_at_index_<TUPLE_SIZE-1, IteratorTupleType>(the_tuple,
                                                                            begins,
                                                                            ends);
    if (!success) {
        the_tuple = ends;
    }
}

template <u64 INDEX, typename IteratorTupleType>
inline typename
std::enable_if<INDEX == std::tuple_size<typename std::decay<IteratorTupleType>::type>::value,
               bool>::type
try_decrement_little_endian_at_index_(IteratorTupleType& the_tuple,
                                      const IteratorTupleType& begins,
                                      const IteratorTupleType& ends)
{
    return false;
}

template <u64 INDEX, typename IteratorTupleType>
inline typename
std::enable_if<INDEX != std::tuple_size<typename std::decay<IteratorTupleType>::type>::value,
               bool>::type
try_decrement_little_endian_at_index_(IteratorTupleType& the_tuple,
                                      const IteratorTupleType& begins,
                                      const IteratorTupleType& ends)
{
    auto& cur_iterator = std::get<INDEX>(the_tuple);
    auto const& cur_begin = std::get<INDEX>(begins);

    if (cur_iterator == cur_begin) {
        auto const& cur_end = std::get<INDEX>(ends);
        cur_iterator = cur_end;
        --cur_iterator;
        return try_decrement_little_endian_at_index_<INDEX+1, IteratorTupleType>(the_tuple,
                                                                                 begins, ends);
    } else {
        --cur_iterator;
        return true;
    }
}

template <typename IteratorTupleType>
inline void decrement_little_endian_iterator(IteratorTupleType& the_tuple,
                                             const IteratorTupleType& begins,
                                             const IteratorTupleType& ends)
{
    auto const success =
        try_decrement_little_endian_at_index_<0, IteratorTupleType>(the_tuple, begins, ends);
    if (!success) {
        the_tuple = begins;
    }
}

template <u64 INDEX, typename IteratorTupleType>
inline typename std::enable_if<INDEX == 0, bool>::type
try_decrement_big_endian_at_index_(IteratorTupleType& the_tuple,
                                   const IteratorTupleType& begins,
                                   const IteratorTupleType& ends)
{
    auto& cur_iterator = std::get<0>(the_tuple);
    auto const& cur_begin = std::get<0>(begins);

    if (cur_iterator == cur_begin) {
        return false;
    } else {
        --cur_iterator;
        return true;
    }
}

template <u64 INDEX, typename IteratorTupleType>
inline typename std::enable_if<INDEX != 0, bool>::type
try_decrement_big_endian_at_index_(IteratorTupleType& the_tuple,
                                   const IteratorTupleType& begins,
                                   const IteratorTupleType& ends)
{
    auto& cur_iterator = std::get<INDEX>(the_tuple);
    auto const& cur_begin = std::get<INDEX>(begins);

    if (cur_iterator == cur_begin) {
        auto const& cur_end = std::get<INDEX>(ends);
        cur_iterator = cur_end;
        --cur_iterator;
        return try_decrement_big_endian_at_index_<INDEX-1, IteratorTupleType>(the_tuple,
                                                                              begins, ends);
    } else {
        --cur_iterator;
        return true;
    }
}

template <typename IteratorTupleType>
inline void decrement_big_endian_iterator_(IteratorTupleType& the_tuple,
                                           const IteratorTupleType& begins,
                                           const IteratorTupleType& ends)
{
     constexpr auto TUPLE_SIZE =
        std::tuple_size<typename std::decay<IteratorTupleType>::type>::value;

    auto const success =
        try_decrement_big_endian_at_index_<TUPLE_SIZE-1, IteratorTupleType>(the_tuple,
                                                                            begins,
                                                                            ends);
    if (!success) {
        the_tuple = begins;
    }
}

template <u64 INDEX, typename IteratorTupleType, typename ValueTupleType>
inline typename
std::enable_if<INDEX ==
               std::tuple_size<typename std::decay<IteratorTupleType>::type>::value, void>::type
update_value_at_index_(const IteratorTupleType& iterator_tuple,
                       ValueTupleType& value_tuple)
{
    return;
}

template <u64 INDEX, typename IteratorTupleType, typename ValueTupleType>
inline typename
std::enable_if<INDEX !=
               std::tuple_size<typename std::decay<IteratorTupleType>::type>::value, void>::type
update_value_at_index_(const IteratorTupleType& iterator_tuple,
                       ValueTupleType& value_tuple)
{
    std::get<INDEX>(value_tuple) = *(std::get<INDEX>(iterator_tuple));
    update_value_at_index_<INDEX+1>(iterator_tuple, value_tuple);
}

template <typename IteratorTupleType, typename ValueTupleType>
inline void update_value_tuple_with_iterator_tuple_(const IteratorTupleType& iterator_tuple,
                                                    ValueTupleType& value_tuple)
{
    update_value_at_index_<0>(iterator_tuple, value_tuple);
}

} /* end namespace detail_ */

template <bool LITTLE_ENDIAN_ITERATORS, typename... IterableTypes>
class CartesianProduct
{
    friend class Iterator;

private:
    typedef std::tuple<typename std::decay<IterableTypes>::type::const_iterator...> IteratorTupleType;
    typedef std::tuple<typename std::decay<IterableTypes>::type::const_iterator::value_type...> ValueTupleType;

    IteratorTupleType m_begins;
    IteratorTupleType m_ends;


public:
    class Iterator : std::iterator<std::bidirectional_iterator_tag,
                                   ValueTupleType, u64,
                                   const ValueTupleType*,
                                   const ValueTupleType&>
    {
    private:
        IteratorTupleType m_current;
        const CartesianProduct* m_product_object;
        ValueTupleType m_cached_value;

        inline void update_cached_value()
        {
            detail_::update_value_tuple_with_iterator_tuple_(m_current, m_cached_value);
        }

    public:
        Iterator(const IteratorTupleType& current,
                 const CartesianProduct* m_product_object)
            : m_current(current), m_product_object(m_product_object)
        {
            // Nothing here
        }

        Iterator()
            : m_current(), m_product_object(nullptr)
        {
            // Nothing here
        }

        Iterator(const Iterator& other)
            : m_current(other.m_current), m_product_object(other.m_product_object),
              m_cached_value(other.m_cached_value)
        {
            // Nothing here
        }

        Iterator(Iterator&& other)
            : m_current(std::move(other.m_current)),
              m_product_object(std::move(other.m_product_object)),
              m_cached_value(std::move(other.m_cached_value))
        {
            // Nothing here
        }

        ~Iterator()
        {
            // Nothing here
        }

        inline Iterator& operator = (const Iterator& other)
        {
            if (&other == this) {
                return *this;
            }
            m_current = other.m_current;
            m_product_object = other.m_product_object;
            m_cached_value = other.m_cached_value;
            return *this;
        }

        inline Iterator& operator = (Iterator&& other)
        {
            if (&other == this) {
                return *this;
            }
            m_current = std::move(other.m_current);
            m_product_object = std::move(other.m_product_object);
            m_cached_value = std::move(other.m_cached_value);
            return *this;
        }

        inline bool operator == (const Iterator&& other) const
        {
            return (m_product_object == other.m_product_object &&
                    m_current == other.m_current);
        }

        inline bool operator != (const Iterator& other) const
        {
            return (!((*this) == other));
        }

        inline const ValueTupleType& operator * () const
        {
            return m_cached_value;
        }

        inline const ValueTupleType* operator -> () const
        {
            return &m_cached_value;
        }

        inline Iterator& operator ++ ()
        {
            auto const& begins = m_product_object->m_begins;
            auto const& ends = m_product_object->m_ends;

            if (LITTLE_ENDIAN_ITERATORS) {
                detail_::increment_little_endian_iterator_(m_current, begins, ends);
            } else {
                detail_::increment_big_endian_iterator_(m_current, begins, ends);
            }

            update_cached_value();
            return *this;
        }

        inline Iterator operator ++ (int unused)
        {
            auto retval = *this;
            ++(*this);
            return retval;
        }

        inline Iterator& operator -- ()
        {
            auto const& begins = m_product_object->m_begins;
            auto const& ends = m_product_object->m_ends;

            if (LITTLE_ENDIAN_ITERATORS) {
                detail_::decrement_little_endian_iterator(m_current, begins, ends);
            } else {
                detail_::decrement_big_endian_iterator_(m_current, m_begins, ends);
            }

            update_cached_value();
            return *this;
        }

        inline Iterator operator -- (int unused)
        {
            auto retval = *this;
            --(*this);
            return retval;
        }
    };

    typedef Iterator iterator;
    typedef iterator ConstIterator;
    typedef ConstIterator const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef reverse_iterator ReverseIterator;
    typedef const_reverse_iterator ConstReverseIterator;

    // constructors
    inline CartesianProduct(IterableTypes... iterables)
        : m_begins(std::begin(iterables)...), m_ends(std::end(iterables)...)
    {
        // Nothing here
    }

    inline CartesianProduct(const CartesianProduct& other)
        : m_begins(other.m_begins), m_ends(other.m_ends)
    {
        // Nothing here
    }

    inline CartesianProduct(CartesianProduct&& other)
        : m_begins(std::move(other.m_begins)),
          m_ends(std::move(other.m_ends))
    {
        // Nothing here
    }

    inline CartesianProduct& operator = (const CartesianProduct& other)
    {
        if (&other == this) {
            return *this;
        }

        m_begins = other.m_begins;
        m_ends = other.m_ends;
        return *this;
    }

    inline CartesianProduct& operator = (CartesianProduct&& other)
    {
        if (&other == this) {
            return *this;
        }
        m_begins = std::move(other.m_begins);
        m_ends = std::move(other.m_ends);
        return *this;
    }

    inline ~CartesianProduct()
    {
        // Nothing here
    }

    inline const_iterator begin() const
    {
        return Iterator(m_begins, this);
    }

    inline const_iterator end() const
    {
        return Iterator(m_ends, this);
    }

    inline const_iterator cbegin() const
    {
        return begin();
    }

    inline const_iterator cend() const
    {
        return end();
    }

    inline const_reverse_iterator rbegin() const
    {
        return const_reverse_iterator(end());
    }

    inline const_reverse_iterator rend() const
    {
        return const_reverse_iterator(begin());
    }

    inline const_reverse_iterator crbegin() const
    {
        return rbegin();
    }

    inline const_reverse_iterator crend() const
    {
        return rend();
    }
};

template <typename... IterableTypes>
inline auto make_le_cartesian_product(IterableTypes&&... iterables)
{
    return CartesianProduct<true, IterableTypes...>(std::forward<IterableTypes>(iterables)...);
}

template <typename... IterableTypes>
inline auto make_be_cartesian_product(IterableTypes&&... iterables)
{
    return CartesianProduct<false, IterableTypes...>(std::forward<IterableTypes>(iterables)...);
}

} /* end namespace ranges */
} /* end namespace aurum */

#endif /* AURUM_RANGES_CARTESIAN_PRODUCT_HPP_ */

//
// CartesianProduct.hpp ends here
