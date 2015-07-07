// TupleUtils.hpp ---
//
// Filename: TupleUtils.hpp
// Author: Abhishek Udupa
// Created: Mon Jul  6 16:07:51 2015 (-0400)
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

#if !defined AURUM_CONTAINERS_TUPLE_UTILS_HPP_
#define AURUM_CONTAINERS_TUPLE_UTILS_HPP_

#include <tuple>
#include <utility>
#include <type_traits>

#include "../basetypes/AurumTypes.hpp"

namespace aurum {
namespace containers {

namespace detail_ {

template <u64 INDEX, typename TupleType, typename FuncType>
inline typename std::enable_if<INDEX == (std::size_t)0, void>::type
apply_on_index_(TupleType&& the_tuple,
                FuncType&& fun_object)
{
    fun_object(std::get<INDEX>(std::forward<TupleType>(the_tuple)));
}

template <u64 INDEX, typename TupleType, typename FuncType>
inline typename std::enable_if<INDEX != (std::size_t)0, void>::type
apply_on_index_(TupleType&& the_tuple,
                FuncType&& fun_object)
{
    apply_on_index_<INDEX-1>(std::forward<TupleType>(the_tuple),
                             std::forward<FuncType>(fun_object));

    fun_object(std::get<INDEX>(std::forward<TupleType>(the_tuple)));
}

template <typename TupleType, typename FuncType, std::size_t... INDEX>
decltype(auto) expand_tuple_and_apply_helper_(TupleType&& the_tuple,
                                              FuncType&& fun_object,
                                              std::index_sequence<INDEX...>)
{
    return fun_object(std::get<INDEX>(std::forward<TupleType>(the_tuple))...);
}

} /* end namespace detail_ */

template <typename TupleType, typename FuncType>
void tuple_foreach(TupleType&& the_tuple, FuncType&& fun_object)
{
    constexpr auto TUPLE_SIZE = std::tuple_size<typename std::decay<TupleType>::type>::value;
    detail_::apply_on_index_<TUPLE_SIZE-1>(std::forward<TupleType>(the_tuple),
                                           std::forward<FuncType>(fun_object));
}

template <typename TupleType, typename FuncType>
inline decltype(auto)
expand_tuple_and_apply(TupleType&& the_tuple, FuncType&& fun_object)
{
    constexpr auto TUPLE_SIZE = std::tuple_size<typename std::decay<TupleType>::type>::value;
    return detail_::expand_tuple_and_apply_helper_(std::forward<TupleType>(the_tuple),
                                                   std::forward<FuncType>(fun_object),
                                                   std::make_index_sequence<TUPLE_SIZE>{});
}

} /* end namespace containers */
} /* end namespace aurum */

#endif /* AURUM_CONTAINERS_TUPLE_UTILS_HPP_ */

//
// TupleUtils.hpp ends here
