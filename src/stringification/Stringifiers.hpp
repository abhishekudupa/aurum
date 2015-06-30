// Stringifiers.hpp ---
//
// Filename: Stringifiers.hpp
// Author: Abhishek Udupa
// Created: Tue Jun 30 18:55:44 2015 (-0400)
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

#if !defined AURUM_STRINGIFICATION_STRINGIFIERS_HPP_
#define AURUM_STRINGIFICATION_STRINGIFIERS_HPP_

#include "../basetypes/AurumTypes.hpp"
#include <type_traits>
#include <sstream>

namespace aurum {
namespace stringification {

template <typename T>
class Stringifier
{
private:
    inline std::string to_string_(const T& object, i64 verbosity,
                                  const std::true_type& is_stringifiable) const
    {
        return object.to_string(verbosity);
    }

    inline std::string to_string_(const T& object, i64 verbosity,
                                  const std::false_type& is_stringifiable) const
    {
        std::ostringstream sstr;
        sstr << object;
        return sstr.str();
    }

public:
    inline std::string operator () (const T& object, i64 verbosity) const
    {
        typename std::is_base_of<StringifiableEBC, T>::type is_stringifiable;
        return to_string_(object, verbosity, is_stringifiable);
    }
};

// Specializations
template <typename T1, typename T2>
class Stringifier<std::pair<T1, T2> >
{
public:
    inline std::string operator () (const T& object, i64 verbosity) const
    {
        ostringstream sstr;
        Stringifier<T1> stringifier1;
        Stringifier<T2> stringifier2;

        sstr << "<" << stringifier1(object, verbosity) << ", "
             << stringifier2(object, verbosity) << ">";
        return sstr.str();
    }
};

namespace detail {


} /* end namespace detail */

template <typename... TupleTypes>
class Stringifier<std::tuple<TupleTypes...> >
{
private:

};

} /* end namespace stringification */
} /* end namespace aurum */

#endif /* AURUM_STRINGIFICATION_STRINGIFIERS_HPP_ */

//
// Stringifiers.hpp ends here
