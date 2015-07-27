// EComparable.hpp ---
//
// Filename: EComparable.hpp
// Author: Abhishek Udupa
// Created: Tue Jul 14 17:04:15 2015 (-0400)
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

#if !defined AURUM_BASETYPES_ECOMPARABLE_HPP_
#define AURUM_BASETYPES_ECOMPARABLE_HPP_

#include "AurumTypes.hpp"
#include "AurumTraits.hpp"

namespace aurum {

namespace detail_ {

class EComparableEBC
{
    // Nothing here
};

} /* end namespace detail_ */

template <typename Derived>
class EComparable : public detail_::EComparableEBC
{
private:
    inline bool hash_equals(const EComparable& other,
                            const std::true_type& is_hashable) const
    {
        return (static_cast<const Derived*>(this)->hash() ==
                static_cast<const Derived*>(&other)->hash());
    }

    inline bool hash_equals(const EComparable& other,
                            const std::false_type& is_hashable) const
    {
        return true;
    }

public:
    bool equals(const EComparable& other) const
    {
        typename IsHashable<Derived>::type is_hashable;

        if (!hash_equals(other, is_hashable)) {
            return false;
        } else {
            auto this_as_derived = static_cast<const Derived*>(this);
            auto other_as_derived = static_cast<const Derived*>(&other);
            return this_as_derived->equal_to(*other_as_derived);
        }
    }

    inline bool operator == (const EComparable& other) const
    {
        return this->equals(other);
    }

    inline bool operator != (const EComparable& other) const
    {
        return (!(this->equals(other)));
    }
};

} /* end namespace aurum */

#endif /* AURUM_BASETYPES_ECOMPARABLE_HPP_ */

//
// EComparable.hpp ends here
