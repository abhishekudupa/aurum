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
#include <string.h>

#include "../basetypes/AurumTypes.hpp"
#include "../basetypes/AurumTraits.hpp"

#include "HashFunctions.hpp"

namespace aurum {
namespace hashing {

namespace am = aurum::memory;

namespace detail_ {

static constexpr u64 sc_hash_initializer = 0xcbf29ce484222325UL;
static constexpr u64 sc_hash_multiplier = 0x100000001b3UL;

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
        return hashing::integer_identity_hash((u64)object);                       \
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
    float as_float = object;
    u32 as_int;
    memcpy(&as_int, &as_float, sizeof(float));
    return (aurum_hash(as_int));
}

template <>
inline u64 aurum_hash<double>(const double& object)
{
    auto as_double = object;
    u64 as_int;
    memcpy(&as_int, &as_double, sizeof(double));
    return (aurum_hash(as_int));
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

        return ((h1 * detail_::sc_hash_multiplier) ^ h2);
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
        accumulated_hash = (accumulated_hash * detail_::sc_hash_multiplier) ^ h1;
        compute_hash<INDEX+1>(the_tuple, accumulated_hash);
    }

public:
    inline u64 operator () (const std::tuple<ArgTypes...>& the_tuple) const
    {
        u64 retval = detail_::sc_hash_initializer;
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
        u64 retval = detail_::sc_hash_initializer;
        for (auto it = iterable.begin(), last = iterable.end(); it != last; ++it) {
            auto cur_hash = elem_hasher(*it);
            retval = (retval * detail_::sc_hash_multiplier) ^ cur_hash;
        }
        return retval;
    }
};

// Deep hashes for pointers
template <typename T, u64 NUM_DEREFS_ALLOWED>
class DeepHasher : public Hasher<T>
{};

template <typename T>
class DeepHasher<T*, 0> : public Hasher<T*>
{};

template <typename T>
class DeepHasher<const T*, 0> : public Hasher<const T*>
{};

template <typename T>
class DeepHasher<memory::ManagedPointer<T>, 0> : public Hasher<memory::ManagedPointer<T> >
{};

template <typename T>
class DeepHasher<memory::ManagedConstPointer<T>, 0>
    : public Hasher<memory::ManagedConstPointer<T> >
{};

template <typename T, u64 NUM_DEREFS_ALLOWED>
class DeepHasher<T*, NUM_DEREFS_ALLOWED>
{
public:
    inline u64 operator () (const T* const& object) const
    {
        DeepHasher<T, NUM_DEREFS_ALLOWED-1> sub_hasher;
        return sub_hasher(*object);
    }
};

template <typename T, u64 NUM_DEREFS_ALLOWED>
class DeepHasher<const T*, NUM_DEREFS_ALLOWED>
    : public DeepHasher<T*, NUM_DEREFS_ALLOWED>
{};

template <typename T1, typename T2, u64 NUM_DEREFS_ALLOWED>
class DeepHasher<std::pair<T1, T2>, NUM_DEREFS_ALLOWED>
{
private:
    typedef std::pair<T1, T2> PairType;

public:
    inline bool operator () (const PairType& object) const
    {
        DeepHasher<T1, NUM_DEREFS_ALLOWED> hasher1;
        DeepHasher<T2, NUM_DEREFS_ALLOWED> hasher2;

        u64 retval = detail_::sc_hash_initializer;
        retval = (retval * detail_::sc_hash_multiplier) ^ hasher1(object.first);
        retval = (retval * detail_::sc_hash_multiplier) ^ hasher2(object.second);
        return retval;
    }
};

template <typename... TupleTypes, u64 NUM_DEREFS_ALLOWED>
class DeepHasher<std::tuple<TupleTypes...>, NUM_DEREFS_ALLOWED>
{
private:
    typedef std::tuple<TupleTypes...> TheTupleType;

    template <u64 INDEX>
    inline typename std::enable_if<INDEX == sizeof...(TupleTypes), void>::type
    compute_hash(const TheTupleType& the_tuple, u64& accumulator) const
    {
        return;
    }

    template <u64 INDEX>
    inline typename std::enable_if<INDEX != sizeof...(TupleTypes), void>::type
    compute_hash(const TheTupleType& the_tuple, u64& accumulator) const
    {
        typedef typename std::tuple_element<INDEX, TheTupleType>::type ElemType;

        DeepHasher<ElemType, NUM_DEREFS_ALLOWED> sub_hasher;
        auto const& elem = std::get<INDEX>(the_tuple);

        accumulator = (accumulator * detail_::sc_hash_multiplier) ^ sub_hasher(elem);
        return;
    }

public:
    inline u64 operator () (const TheTupleType& the_tuple) const
    {
        u64 retval = detail_::sc_hash_initializer;
        compute_hash<0>(the_tuple, retval);
        return retval;
    }
};

template <typename T, typename ElemHasher = DeepHasher<typename T::value_type, 8> >
class DeepIterableHasher
{
public:
    inline u64 operator () (const T& iterable) const
    {
        ElemHasher elem_hasher;
        u64 retval = detail_::sc_hash_initializer;
        for (auto it = iterable.begin(), last = iterable.end(); it != last; ++it) {
            auto cur_hash = elem_hasher(*it);
            retval = (retval * detail_::sc_hash_multiplier) ^ cur_hash;
        }
        return retval;
    }
};

} /* end namespace hashing */
} /* end namespace aurum */

#endif /* AURUM_HASHING_HASH_HPP_ */

//
// Hashers.hpp ends here
