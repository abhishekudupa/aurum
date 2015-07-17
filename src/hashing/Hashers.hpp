// Hashers.hpp ---
// Filename: Hashers.hpp
// Author: Abhishek Udupa
// Created: Mon Feb 16 18:49:12 2015 (-0500)
//
// Copyright (c) 2013, Abhishek Udupa, University of Pennsylvania
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

#if !defined AURUM_HASHING_HASHERS_HPP_
#define AURUM_HASHING_HASHERS_HPP_

// this file contains the definition of builtin hash
// functions for common types, as well as facilities
// to combine/mix hash values in a nice fashion

#include <functional>

#include "../basetypes/AurumTypes.hpp"
#include "../basetypes/AurumTraits.hpp"

#include "HashFunctions.hpp"

namespace aurum {
namespace hashing {

namespace am = aurum::memory;

namespace detail_ {

template <typename T>
inline u64 compute_aurum_hash_(const T& object)
{
    return object.hash();
}

} /* end namespace detail_ */

template <typename T>
inline u64 aurum_hash(const T& object)
{
    static_assert(IsHashable<T>::value,
                  "Attempted to compute hash of unhashable type");
    return compute_aurum_hash_(object);
}

#define MAKE_PRIMITIVE_INTEGER_HASH_(TYPENAME__)                              \
    template <>                                                               \
    inline u64 aurum_hash<TYPENAME__>(const TYPENAME__& object)               \
    {                                                                         \
        return hashing::integer_fast_hash((u64)object);                       \
    }


MAKE_PRIMITIVE_INTEGER_HASH_(u64)
MAKE_PRIMITIVE_INTEGER_HASH_(i64)
MAKE_PRIMITIVE_INTEGER_HASH_(u32)
MAKE_PRIMITIVE_INTEGER_HASH_(i32)
MAKE_PRIMITIVE_INTEGER_HASH_(u16)
MAKE_PRIMITIVE_INTEGER_HASH_(i16)
MAKE_PRIMITIVE_INTEGER_HASH_(u08)
MAKE_PRIMITIVE_INTEGER_HASH_(i08)
MAKE_PRIMITIVE_INTEGER_HASH_(bool)

#undef MAKE_PRIMITIVE_INTEGER_HASH_

template <>
inline u64 aurum_hash<float>(const float& object)
{
    auto actual_object = object;
    u32 as_int = *(reinterpret_cast<u32*>(&actual_object));
    return aurum_hash(as_int);
}

template <>
inline u64 aurum_hash<double>(const double& object)
{
    auto actual_object = object;
    u64 as_int = *(static_cast<u64*>(static_cast<void*>(&actual_object)));
    return aurum_hash(as_int);
}

template <>
inline u64 aurum_hash<void*>(void* const& ptr)
{
    auto actual_ptr = ptr;
    u64 as_int = reinterpret_cast<u64>(actual_ptr);
    return aurum_hash(as_int);
}

template <>
inline u64 aurum_hash<const void*>(const void* const& ptr)
{
    auto actual_ptr = ptr;
    u64 as_int = reinterpret_cast<u64>(actual_ptr);
    return aurum_hash(as_int);
}

template <>
inline u64 aurum_hash<std::string>(const std::string& object)
{
    return hashing::default_hash_function(object.c_str(), object.length());
}

template <typename T>
class Hasher
{
public:
    inline u64 operator () (const T& object) const
    {
        return aurum_hash(object);
    }
};

// specialization for pointers
template <typename T>
class Hasher<T*>
{
public:
    inline u64 operator () (T* object) const
    {
        return aurum_hash<void*>(static_cast<void*>(object));
    }
};

template <typename T>
class Hasher<const T*>
{
public:
    inline u64 operator () (const T* object) const
    {
        return aurum_hash<const void*>(static_cast<const void*>(object));
    }
};

template <typename T>
class Hasher<am::ManagedPointer<T> >
{
    inline u64 operator () (const am::ManagedPointer<T>& managed_ptr) const
    {
        return aurum_hash<void*>(static_cast<void*>(managed_ptr->get_raw_pointer()));
    }
};

template <typename T>
class Hasher<am::ManagedConstPointer<T> >
{
    inline u64 operator () (const am::ManagedConstPointer<T>& managed_ptr) const
    {
        return aurum_hash<const void*>(static_cast<const void*>(managed_ptr->get_raw_pointer()));
    }
};

// deep hashes on pointers
template <typename T>
class PtrHasher
{
private:
    inline u64 apply(const T& object, const std::true_type& is_pointer) const
    {
        Hasher<typename RemovePointer<T>::type> hasher;
        return hasher(*object);
    }

    inline u64 apply(const T& object, const std::false_type& is_pointer) const
    {
        Hasher<T> hasher;
        return hasher(object);
    }

public:
    inline u64 operator () (const T& object) const
    {
        typename IsPtrLike<typename std::decay<T>::type>::type is_pointer;
        return apply(object, is_pointer);
    }
};

// Specialization for pairs
template <typename T1, typename T2>
class Hasher<std::pair<T1, T2> >
{
public:
    inline u64 operator () (const std::pair<T1, T2>& the_pair) const
    {
        Hasher<T1> t1_hasher;
        Hasher<T2> t2_hasher;
        auto h1 = t1_hasher(the_pair.first);
        auto h2 = t2_hasher(the_pair.second);

        return ((h1 * 0x100000001b3UL) ^ h2);
    }
};

template <typename T1, typename T2>
class PtrHasher<std::pair<T1, T2> >
{
public:
    inline u64 operator () (const std::pair<T1, T2>& the_pair) const
    {
        PtrHasher<T1> t1_hasher;
        PtrHasher<T2> t2_hasher;
        auto h1 = t1_hasher(the_pair.first);
        auto h2 = t2_hasher(the_pair.second);

        return ((h1 * 0x100000001b3UL) ^ h2);
    }
};

// Specialization for tuples
template <typename... ArgTypes>
class Hasher<std::tuple<ArgTypes...> >
{
private:
    template <u64 INDEX, typename... TupleTypes>
    inline typename std::enable_if<INDEX == sizeof...(TupleTypes), void>::type
    compute_hash(const std::tuple<TupleTypes...>& the_tuple,
                 u64& accumulated_hash) const
    {
        return;
    }

    template <u64 INDEX, typename... TupleTypes>
    inline typename std::enable_if<INDEX != sizeof...(TupleTypes), void>::type
    compute_hash(const std::tuple<TupleTypes...>& the_tuple,
                 u64& accumulated_hash) const
    {
        Hasher<typename std::tuple_element<INDEX, typename std::tuple<TupleTypes...> >::type> hasher;
        auto h1 = hasher(std::get<INDEX>(the_tuple));
        accumulated_hash = (accumulated_hash * 0x100000001b3UL) ^ h1;
        compute_hash<INDEX+1>(the_tuple, accumulated_hash);
    }

public:
    inline u64 operator () (const std::tuple<ArgTypes...>& the_tuple) const
    {
        u64 retval = 0xcbf29ce484222325UL;
        compute_hash<0>(the_tuple, retval);
        return retval;
    }
};

template <typename... ArgTypes>
class PtrHasher<std::tuple<ArgTypes...> >
{
private:
    template <u64 INDEX, typename... TupleTypes>
    inline typename std::enable_if<INDEX == sizeof...(TupleTypes), void>::type
    compute_hash(const std::tuple<TupleTypes...>& the_tuple,
                 u64& accumulated_hash) const
    {
        return;
    }

    template <u64 INDEX, typename... TupleTypes>
    inline typename std::enable_if<INDEX != sizeof...(TupleTypes), void>::type
    compute_hash(const std::tuple<TupleTypes...>& the_tuple,
                 u64& accumulated_hash) const
    {
        PtrHasher<typename std::tuple_element<INDEX, typename std::tuple<TupleTypes...> >::type>
            hasher;

        auto h1 = hasher(std::get<INDEX>(the_tuple));
        accumulated_hash = (accumulated_hash * 0x100000001b3UL) ^ h1;
        compute_hash<INDEX+1>(the_tuple, accumulated_hash);
    }

public:
    inline u64 operator () (const std::tuple<ArgTypes...>& the_tuple) const
    {
        u64 retval = 0xcbf29ce484222325UL;
        compute_hash<0>(the_tuple, retval);
        return retval;
    }
};

// hashes for iterables
template <typename T, typename ElemHasher = Hasher<typename T::value_type> >
class IterableHasher
{
public:
    inline u64 operator () (const T& iterable) const
    {
        ElemHasher elem_hasher;
        u64 retval = 0xcbf29ce484222325UL;
        for (auto it = iterable.begin(), last = iterable.end(); it != last; ++it) {
            auto cur_hash = elem_hasher(*it);
            retval = (retval * 0x100000001b3UL) ^ cur_hash;
        }
        return retval;
    }
};

template <typename T, typename ElemHasher = PtrHasher<typename T::value_type> >
class IterablePtrHasher
{
public:
    inline u64 operator () (const T& iterable) const
    {
        ElemHasher elem_hasher;
        u64 retval = 0xcbf29ce484222325UL;
        for (auto it = iterable.begin(), last = iterable.end(); it != last; ++it) {
            auto cur_hash = elem_hasher(*it);
            retval = (retval * 0x100000001b3UL) ^ cur_hash;
        }
        return retval;
    }
};

} /* end namespace hashing */
} /* end namespace aurum */

#endif /* AURUM_HASHING_HASH_HPP_ */

//
// Hashers.hpp ends here
