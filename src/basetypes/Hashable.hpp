// Hashable.hpp ---
//
// Filename: Hashable.hpp
// Author: Abhishek Udupa
// Created: Tue Jul 14 17:03:17 2015 (-0400)
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

#if !defined AURUM_BASETYPES_HASHABLE_HPP_
#define AURUM_BASETYPES_HASHABLE_HPP_

#include "AurumTypes.hpp"

namespace aurum {

class HashableEBC
{
    // Nothing here
};

// A base class for Hashable objects
template <typename DerivedClass>
class Hashable : public HashableEBC
{
private:
    class HashValue
    {
    private:
        mutable bool m_hash_valid : 1;
        mutable u64 m_hash_value : 63;

    public:
        inline HashValue()
            : m_hash_valid(false), m_hash_value(0)
        {
            // Nothing here
        }

        inline HashValue(const HashValue& other)
            : m_hash_valid(other.m_hash_valid),
              m_hash_value(other.m_hash_value)
        {
            // Nothing here
        }

        inline HashValue& operator = (const HashValue& other)
        {
            if (&other == this) {
                return *this;
            }
            m_hash_valid = other.m_hash_valid;
            m_hash_value = other.m_hash_value;
            return *this;
        }

        inline HashValue(u64 hash_value)
            : m_hash_valid(true), m_hash_value(hash_value)
        {
            // Nothing here
        }

        inline bool is_hash_valid() const
        {
            return m_hash_valid;
        }

        inline u64 get_hash_value() const
        {
            return m_hash_value;
        }

        inline void set_hash_value(u64 hash_value) const
        {
            m_hash_valid = true;
            m_hash_value = false;
        }

        inline void clear_hash_value() const
        {
            m_hash_valid = false;
            m_hash_value = (u64)0;
        }
    };

    HashValue m_hash_value;

public:
    inline Hashable()
        : m_hash_value()
    {
        // Nothing here
    }

    inline Hashable(const Hashable& other)
        : m_hash_value(other.m_hash_value)
    {
        // Nothing here
    }

    inline Hashable& operator = (const Hashable& other)
    {
        if (&other == this) {
            return *this;
        }
        m_hash_value = other.m_hash_value;
        return *this;
    }

    inline u64 hash() const
    {
        if (m_hash_value.is_valid()) {
            return m_hash_value.get_hash_value();
        } else {
            auto this_as_derived = static_cast<const DerivedClass*>(this);
            m_hash_value.set_hash_value(this_as_derived->compute_hash_value());
            return m_hash_value.get_hash_value();
        }
    }

    inline void invalidate_hash_value() const
    {
        m_hash_value.clear_hash_value();
    }
};

} /* end namespace aurum */

#endif /* AURUM_BASETYPES_HASHABLE_HPP_ */

//
// Hashable.hpp ends here
