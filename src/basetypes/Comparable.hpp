// Comparable.hpp ---
//
// Filename: Comparable.hpp
// Author: Abhishek Udupa
// Created: Tue Jul 14 19:11:18 2015 (-0400)
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

#if !defined AURUM_BASETYPES_COMPARABLE_HPP_
#define AURUM_BASETYPES_COMPARABLE_HPP_

#include "EComparable.hpp"

namespace aurum {

namespace detail_ {

class ComparableEBC
{
    // Nothing here
};

} /* end namespace detail_ */

// Base class for comparable objects
template <typename DerivedClass>
class Comparable : public detail_::ComparableEBC, public EComparable<DerivedClass>
{
public:
    i64 compare(const Comparable& other) const
    {
        auto this_as_derived = static_cast<const DerivedClass*>(this);
        auto other_as_derived = static_cast<const DerivedClass*>(&other);
        return this_as_derived->compare_with(*other_as_derived);
    }

    inline bool operator < (const Comparable& other) const
    {
        return (compare(other) < 0);
    }

    inline bool operator <= (const Comparable& other) const
    {
        return (compare(other) <= 0);
    }

    inline bool operator > (const Comparable& other) const
    {
        return (compare(other) > 0);
    }

    inline bool operator >= (const Comparable& other) const
    {
        return (compare(other) >= 0);
    }
};


} /* end namespace aurum */

#endif /* AURUM_BASETYPES_COMPARABLE_HPP_ */

//
// Comparable.hpp ends here
