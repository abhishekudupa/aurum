// Comparators.hpp ---
//
// Filename: Comparators.hpp
// Author: Abhishek Udupa
// Created: Tue Mar 17 16:09:39 2015 (-0400)
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

#if !defined AURUM_COMPARISONS_COMPARATORS_HPP_
#define AURUM_COMPARISONS_COMPARATORS_HPP_

#include <functional>

#include "../basetypes/AurumTypes.hpp"
#include "../basetypes/AurumTraits.hpp"

namespace aurum {
namespace comparisons {

template <typename T, typename BinaryPredicate>
class Negate
{
public:
    inline bool operator () (const T& obj1, const T& obj2) const
    {
        BinaryPredicate fun;
        return (!fun(obj1, obj2));
    }
};

template <typename T, typename BinaryPredicate>
class Reverse
{
public:
    inline bool operator () (const T& obj1, const T& obj2) const
    {
        BinaryPredicate fun;
        return fun(obj2, obj1);
    }
};

template <typename T>
class Lesser
{
public:
    inline bool operator () (const T& obj1, const T& obj2) const
    {
        std::less<T> less_fun;
        return less_fun(obj1, obj2);
    }
};

template <typename T>
using LesserEqual = Negate<T, Reverse<T, Lesser<T> > >;

template <typename T>
using GreaterEqual = Negate<T, Lesser<T> >;

template <typename T>
using Greater = Negate<T, LesserEqual<T> >;

template <typename T, u64 NUM_DEREFS_ALLOWED>
class DeepLesser : public Lesser<T>
{};

template <typename T>
class DeepLesser<T*, 0> : public Lesser<T*>
{};

template <typename T>
class DeepLesser<const T*, 0> : public Lesser<const T*>
{};

template <typename T>
class DeepLesser<memory::ManagedPointer<T>, 0> : public Lesser<memory::ManagedPointer<T> >
{};

template <typename T>
class DeepLesser<memory::ManagedConstPointer<T>, 0>
    : public Lesser<memory::ManagedConstPointer<T> >
{};

template <typename T, u64 NUM_DEREFS_ALLOWED>
class DeepLesser<T*, NUM_DEREFS_ALLOWED>
{
public:
    inline bool operator () (const T* const& obj1,
                             const T* const& obj2) const
    {
        DeepLesser<T, NUM_DEREFS_ALLOWED-1> sub_comparator;
        return sub_comparator(*obj1, *obj2);
    }
};

template <typename T, u64 NUM_DEREFS_ALLOWED>
class DeepLesser<const T*, NUM_DEREFS_ALLOWED>
    : public DeepLesser<T*, NUM_DEREFS_ALLOWED>
{};

template <typename T, u64 NUM_DEREFS_ALLOWED>
class DeepLesser<memory::ManagedPointer<T>, NUM_DEREFS_ALLOWED>
    : public DeepLesser<T*, NUM_DEREFS_ALLOWED>
{};

template <typename T, u64 NUM_DEREFS_ALLOWED>
class DeepLesser<memory::ManagedConstPointer<T>, NUM_DEREFS_ALLOWED>
    : public DeepLesser<T*, NUM_DEREFS_ALLOWED>
{};

template <typename T1, typename T2, u64 NUM_DEREFS_ALLOWED>
class DeepLesser<std::pair<T1, T2>, NUM_DEREFS_ALLOWED>
{
private:
    typedef std::pair<T1, T2> PairType;

public:
    inline bool operator () (const PairType& obj1, const PairType& obj2) const
    {
        DeepLesser<T1, NUM_DEREFS_ALLOWED> sub_comparator1;
        DeepLesser<T2, NUM_DEREFS_ALLOWED> sub_comparator2;

        return (sub_comparator1(obj1.first, obj2.first) ||
                (!(sub_comparator1(obj2.first, obj1.first)) &&
                 sub_comparator2(obj1.second, obj2.second)));
    }
};

template <typename... TupleTypes, u64 NUM_DEREFS_ALLOWED>
class DeepLesser<std::tuple<TupleTypes...>, NUM_DEREFS_ALLOWED>
{
private:
    typedef std::tuple<TupleTypes...> TheTupleType;

    template <u64 INDEX>
    inline typename std::enable_if<INDEX == sizeof...(TupleTypes), bool>::type
    compare_element(const TheTupleType& obj1, const TheTupleType& obj2) const
    {
        return false;
    }

    template <u64 INDEX>
    inline typename std::enable_if<INDEX != sizeof...(TupleTypes), bool>::type
    compare_element(const TheTupleType& obj1, const TheTupleType& obj2) const
    {
        typedef typename std::tuple_element<INDEX, TheTupleType>::type ElementType;

        DeepLesser<ElementType, NUM_DEREFS_ALLOWED> sub_comparator;
        auto const& elem1 = std::get<INDEX>(obj1);
        auto const& elem2 = std::get<INDEX>(obj2);

        return (sub_comparator(elem1, elem2) ||
                (!(sub_comparator(elem2, elem1)) &&
                 compare_element<INDEX+1>(obj1, obj2)));
    }

public:
    inline bool operator () (const TheTupleType& obj1, const TheTupleType& obj2) const
    {
        return compare_element<0>(obj1, obj2);
    }
};

template <typename T, u64 NUM_DEREFS_ALLOWED>
using DeepLesserEqual = Negate<T, Reverse<T, DeepLesser<T, NUM_DEREFS_ALLOWED> > >;

template <typename T, u64 NUM_DEREFS_ALLOWED>
using DeepGreaterEqual = Negate<T, DeepLesser<T, NUM_DEREFS_ALLOWED> >;

template <typename T, u64 NUM_DEREFS_ALLOWED>
using DeepGreater = Negate<T, DeepLesserEqual<T, NUM_DEREFS_ALLOWED> >;

template <typename T>
class EqualTo
{
public:
    inline bool operator () (const T& object1, const T& object2) const
    {
        std::equal_to<T> equal_fun;
        return equal_fun(object1, object2);
    }
};

template <typename T>
using NotEqualTo = Negate<T, EqualTo<T> >;

template <typename T, u64 NUM_DEREFS_ALLOWED>
class DeepEqualTo : public EqualTo<T>
{};

template <typename T>
class DeepEqualTo<T*, 0> : public EqualTo<T>
{};

template <typename T>
class DeepEqualTo<const T*, 0> : public EqualTo<T>
{};

template <typename T>
class DeepEqualTo<memory::ManagedPointer<T>, 0> : public EqualTo<memory::ManagedPointer<T> >
{};

template <typename T>
class DeepEqualTo<memory::ManagedConstPointer<T>, 0>
    : public EqualTo<memory::ManagedConstPointer<T> >
{};

template <typename T, u64 NUM_DEREFS_ALLOWED>
class DeepEqualTo<T*, NUM_DEREFS_ALLOWED>
{
    inline bool operator () (const T* const& obj1, const T* const& obj2) const
    {
        DeepEqualTo<T, NUM_DEREFS_ALLOWED-1> sub_comparator;
        return sub_comparator(*obj1, *obj2);
    }
};

template <typename T, u64 NUM_DEREFS_ALLOWED>
class DeepEqualTo<const T*, NUM_DEREFS_ALLOWED>
    : public DeepEqualTo<T*, NUM_DEREFS_ALLOWED>
{};

template <typename T, u64 NUM_DEREFS_ALLOWED>
class DeepEqualTo<memory::ManagedPointer<T>, NUM_DEREFS_ALLOWED>
    : public DeepEqualTo<T*, NUM_DEREFS_ALLOWED>
{};

template <typename T, u64 NUM_DEREFS_ALLOWED>
class DeepEqualTo<memory::ManagedConstPointer<T>, NUM_DEREFS_ALLOWED>
    : public DeepEqualTo<T*, NUM_DEREFS_ALLOWED>
{};

template <typename T1, typename T2, u64 NUM_DEREFS_ALLOWED>
class DeepEqualTo<std::pair<T1, T2>, NUM_DEREFS_ALLOWED>
{
private:
    typedef std::pair<T1, T2> PairType;

public:
    inline bool operator () (const PairType& obj1, const PairType& obj2) const
    {
        DeepEqualTo<T1, NUM_DEREFS_ALLOWED> sub_comparator1;
        DeepEqualTo<T2, NUM_DEREFS_ALLOWED> sub_comparator2;

        return (sub_comparator1(obj1.first, obj2.first) &&
                sub_comparator2(obj1.second, obj2.second));
    }
};

template <typename... TupleTypes, u64 NUM_DEREFS_ALLOWED>
class DeepEqualTo<std::tuple<TupleTypes...>, NUM_DEREFS_ALLOWED>
{
private:
    typedef std::tuple<TupleTypes...> TheTupleType;

    template <u64 INDEX>
    inline typename std::enable_if<INDEX == sizeof...(TupleTypes), bool>::type
    compare_element(const TheTupleType& obj1, const TheTupleType& obj2) const
    {
        return false;
    }

    template <u64 INDEX>
    inline typename std::enable_if<INDEX != sizeof...(TupleTypes), bool>::type
    compare_element(const TheTupleType& obj1, const TheTupleType& obj2) const
    {
        typedef typename std::tuple_element<INDEX, TheTupleType>::type ElementType;

        DeepEqualTo<ElementType, NUM_DEREFS_ALLOWED> sub_comparator;
        auto const& elem1 = std::get<INDEX>(obj1);
        auto const& elem2 = std::get<INDEX>(obj2);

        return (sub_comparator(elem1, elem2) && compare_element<INDEX+1>(obj1, obj2));
    }

public:
    inline bool operator () (const TheTupleType& obj1, const TheTupleType& obj2) const
    {
        return compare_element<0>(obj1, obj2);
    }
};

template <typename T, u64 NUM_DEREFS_ALLOWED>
using DeepNotEqualTo = Negate<T, DeepEqualTo<T, NUM_DEREFS_ALLOWED> >;

} /* end namespace comparisons */
} /* end namespace aurum */

#endif /* AURUM_COMPARISONS_COMPARATORS_HPP_ */

//
// Comparators.hpp ends here
