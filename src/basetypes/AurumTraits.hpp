// AurumTraits.hpp ---
//
// Filename: AurumTraits.hpp
// Author: Abhishek Udupa
// Created: Tue Jul 14 17:28:39 2015 (-0400)
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

#if !defined AURUM_BASETYPES_AURUM_TRAITS_HPP_
#define AURUM_BASETYPES_AURUM_TRAITS_HPP_

#include "AurumTypes.hpp"
#include "../memory/ManagedPointer.hpp"

namespace aurum {

namespace detail_ {

struct TrueStruct
{
    typedef typename std::true_type type;
    static constexpr bool value = true;
};

struct FalseStruct
{
    typedef typename std::false_type type;
    static constexpr bool value = false;
};

template <template <class> class Tester, typename CurType, typename... RestTypes>
struct AllStruct
    : std::conditional<sizeof...(RestTypes) == 0,
                       typename Tester<CurType>::type,
                       typename std::conditional<Tester<CurType>::value &&
                                                 AllStruct<Tester, RestTypes...>::value,
                                                 TrueStruct,
                                                 FalseStruct> >::type
{};

} /* end namespace detail_ */

// Utility Macros
#define MARK_PRIMITIVE_TYPE_AS_STRUCT_(TYPENAME__, STRUCTNAME__)               \
    template <>                                                                \
    struct STRUCTNAME__<TYPENAME__> : detail_::TrueStruct                      \
    {}

#define MARK_TEMPLATE_TYPE_AS_STRUCT_(TEMPLATE_TYPENAME__, TYPENAME__, STRUCTNAME__)    \
    template <typename TEMPLATE_TYPENAME__>                                             \
    struct STRUCTNAME__<TYPENAME__ > : detail_::TrueStruct                              \
    {}


template <typename T>
struct IsStringifiable
    : std::conditional<std::is_base_of<detail_::StringifiableEBC, T>::value,
                       detail_::TrueStruct, detail_::FalseStruct>::type
{};

MARK_PRIMITIVE_TYPE_AS_STRUCT_(bool, IsStringifiable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(u08, IsStringifiable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(i08, IsStringifiable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(u16, IsStringifiable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(i16, IsStringifiable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(u32, IsStringifiable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(i32, IsStringifiable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(u64, IsStringifiable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(i64, IsStringifiable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(float, IsStringifiable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(double, IsStringifiable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(std::string, IsStringifiable);

MARK_TEMPLATE_TYPE_AS_STRUCT_(T, memory::ManagedPointer<T>, IsStringifiable);
MARK_TEMPLATE_TYPE_AS_STRUCT_(T, memory::ManagedConstPointer<T>, IsStringifiable);
MARK_TEMPLATE_TYPE_AS_STRUCT_(T, T*, IsStringifiable);
MARK_TEMPLATE_TYPE_AS_STRUCT_(T, const T*, IsStringifiable);

template <typename T1, typename T2>
struct IsStringifiable<std::pair<T1, T2> >
    : detail_::AllStruct<IsStringifiable, T1, T2>
{};

template <typename... TupleTypes>
struct IsStringifiable<std::tuple<TupleTypes...> >
    : detail_::AllStruct<IsStringifiable, TupleTypes...>
{};

// Hashable
template <typename T>
struct IsHashable
    : std::conditional<std::is_base_of<detail_::HashableEBC, T>::value,
                       detail_::TrueStruct, detail_::FalseStruct>
{};

MARK_PRIMITIVE_TYPE_AS_STRUCT_(bool, IsHashable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(u08, IsHashable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(i08, IsHashable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(u16, IsHashable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(i16, IsHashable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(u32, IsHashable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(i32, IsHashable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(u64, IsHashable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(i64, IsHashable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(float, IsHashable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(double, IsHashable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(std::string, IsHashable);

MARK_TEMPLATE_TYPE_AS_STRUCT_(T, memory::ManagedPointer<T>, IsHashable);
MARK_TEMPLATE_TYPE_AS_STRUCT_(T, memory::ManagedConstPointer<T>, IsHashable);
MARK_TEMPLATE_TYPE_AS_STRUCT_(T, T*, IsHashable);
MARK_TEMPLATE_TYPE_AS_STRUCT_(T, const T*, IsHashable);

template <typename T1, typename T2>
struct IsHashable<std::pair<T1, T2> > : detail_::AllStruct<IsHashable, T1, T2>
{};

template <typename... ArgTypes>
struct IsHashable<std::tuple<ArgTypes...> > : detail_::AllStruct<IsHashable, ArgTypes...>
{};

// EComparable
template <typename T>
struct IsEComparable
    : std::conditional<std::is_base_of<detail_::EComparableEBC, T>::value,
                       detail_::TrueStruct, detail_::FalseStruct>
{};

MARK_PRIMITIVE_TYPE_AS_STRUCT_(bool, IsEComparable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(u08, IsEComparable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(i08, IsEComparable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(u16, IsEComparable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(i16, IsEComparable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(u32, IsEComparable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(i32, IsEComparable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(u64, IsEComparable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(i64, IsEComparable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(std::string, IsEComparable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(float, IsEComparable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(double, IsEComparable);

MARK_TEMPLATE_TYPE_AS_STRUCT_(T, memory::ManagedPointer<T>, IsEComparable);
MARK_TEMPLATE_TYPE_AS_STRUCT_(T, memory::ManagedConstPointer<T>, IsEComparable);
MARK_TEMPLATE_TYPE_AS_STRUCT_(T, T*, IsEComparable);
MARK_TEMPLATE_TYPE_AS_STRUCT_(T, const T*, IsEComparable);

template <typename T1, typename T2>
struct IsEComparable<std::pair<T1, T2> >
    : detail_::AllStruct<IsEComparable, T1, T2>
{};

template <typename... ArgTypes>
struct IsEComparable<std::tuple<ArgTypes...> >
    : detail_::AllStruct<IsEComparable, ArgTypes...>
{};

// Comparable
template <typename T>
struct IsComparable
    : std::conditional<std::is_base_of<detail_::ComparableEBC, T>::value,
                       detail_::TrueStruct, detail_::FalseStruct>
{};

MARK_PRIMITIVE_TYPE_AS_STRUCT_(bool, IsComparable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(u08, IsComparable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(i08, IsComparable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(u16, IsComparable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(i16, IsComparable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(u32, IsComparable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(i32, IsComparable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(u64, IsComparable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(i64, IsComparable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(std::string, IsComparable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(float, IsComparable);
MARK_PRIMITIVE_TYPE_AS_STRUCT_(double, IsComparable);

MARK_TEMPLATE_TYPE_AS_STRUCT_(T, memory::ManagedPointer<T>, IsComparable);
MARK_TEMPLATE_TYPE_AS_STRUCT_(T, memory::ManagedConstPointer<T>, IsComparable);
MARK_TEMPLATE_TYPE_AS_STRUCT_(T, T*, IsComparable);
MARK_TEMPLATE_TYPE_AS_STRUCT_(T, const T*, IsComparable);

template <typename T1, typename T2>
struct IsComparable<std::pair<T1, T2> >
    : detail_::AllStruct<IsComparable, T1, T2>
{};

template <typename... ArgTypes>
struct IsComparable<std::tuple<ArgTypes...> >
    : detail_::AllStruct<IsComparable, ArgTypes...>
{};

// undefine these macros, they're not intended to be
// used anywhere else!
#undef MARK_PRIMITIVE_TYPE_AS_STRUCT_
#undef MARK_TEMPLATE_TYPE_AS_STRUCT_

template <typename T>
struct IsAnyManagedPointer : detail_::FalseStruct
{};

template <typename T>
struct IsConstManagedPointer : detail_::FalseStruct
{};

template <typename T>
struct IsNonConstManagedPointer : detail_::FalseStruct
{};

template <typename T>
struct IsAnyManagedPointer<memory::ManagedPointer<T> > : detail_::TrueStruct
{};

template <typename T>
struct IsAnyManagedPointer<memory::ManagedConstPointer<T> > : detail_::TrueStruct
{};

template <typename T>
struct IsConstManagedPointer<memory::ManagedConstPointer<T> > : detail_::TrueStruct
{};

template <typename T>
struct IsNonConstManagedPointer<memory::ManagedPointer<T> > : detail_::TrueStruct
{};

template <typename T>
struct IsPtrLike : detail_::FalseStruct
{};

template <typename T>
struct IsPtrLike<T*> : detail_::TrueStruct
{};

template <typename T>
struct IsPtrLike<const T*> : detail_::TrueStruct
{};

template <typename T>
struct IsPtrLike<memory::ManagedPointer<T> > : detail_::TrueStruct
{};

template <typename T>
struct IsPtrLike<memory::ManagedConstPointer<T> >: detail_::TrueStruct
{};

template <typename T>
struct RemovePointer
{
    typedef T type;
};

template <typename T>
struct RemovePointer<T*>
{
    typedef T type;
};

template <typename T>
struct RemovePointer<const T*>
{
    typedef T type;
};

template <typename T>
struct RemovePointer<memory::ManagedPointer<T> >
{
    typedef T type;
};

template <typename T>
struct RemovePointer<memory::ManagedConstPointer<T> >
{
    typedef T type;
};

template <typename T>
struct IsRefCountable
    : std::conditional<std::is_base_of<RefCountable, T>::value,
                       detail_::TrueStruct, detail_::FalseStruct>::type
{};

} /* end namespace aurum */

#endif /* AURUM_BASETYPES_AURUM_TRAITS_HPP_ */

//
// AurumTraits.hpp ends here
