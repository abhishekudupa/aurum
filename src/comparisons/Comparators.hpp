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
class PtrLesser
{
private:
    inline bool apply(const T& obj1, const T& obj2,
                      const std::true_type& is_pointer) const
    {
        Lesser<typename RemovePointer<T>::type> less_fun;
        return less_fun(*obj1, *obj2);
    }

    inline bool apply(const T& obj1, const T& obj2, const std::false_type& is_pointer) const
    {
        Lesser<T> less_fun;
        return less_fun(obj1, obj2);
    }

public:
    inline bool operator () (const T& obj1, const T& obj2) const
    {
        typename IsPtrLike<typename std::decay<T>::type>::type is_pointer;
        return apply(obj1, obj2, is_pointer);
    }
};

template <typename T>
using LesserEqual = Negate<T, Reverse<T, Lesser<T> > >;

template <typename T>
using GreaterEqual = Negate<T, Lesser<T> >;

template <typename T>
using Greater = Negate<T, LesserEqual<T> >;

template <typename T>
using PtrLesserEqual = Negate<T, Reverse<T, PtrLesser<T> > >;

template <typename T>
using PtrGreaterEqual = Negate<T, PtrLesser<T> >;

template <typename T>
using PtrGreater = Negate<T, PtrLesserEqual<T> >;

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
class PtrEqualTo
{
private:
    inline bool apply(const T& object1, const T& object2,
                      const std::true_type& is_pointer) const
    {
        EqualTo<typename RemovePointer<T>::type> equal_fun;
        return equal_fun(*object1, *object2);
    }

    inline bool apply(const T& object1, const T& object2,
                      const std::false_type& is_pointer) const
    {
        EqualTo<T> equal_fun;
        return equal_fun(object1, object2);
    }

public:
    inline bool operator () (const T& object1, const T& object2) const
    {
        typename IsPtrLike<typename std::decay<T>::type>::type is_pointer;
        return apply(object1, object2, is_pointer);
    }
};

template <typename T>
using NotEqualTo = Negate<T, EqualTo<T> >;

template <typename T>
using PtrNotEqualTo = Negate<T, PtrEqualTo<T> >;

} /* end namespace comparisons */
} /* end namespace aurum */

#endif /* AURUM_COMPARISONS_COMPARATORS_HPP_ */

//
// Comparators.hpp ends here
