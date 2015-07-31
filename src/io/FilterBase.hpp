// FilterBase.hpp ---
//
// Filename: FilterBase.hpp
// Author: Abhishek Udupa
// Created: Fri Jul 31 16:32:46 2015 (-0400)
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

#if !defined AURUM_IO_FILTER_BASE_HPP_
#define AURUM_IO_FILTER_BASE_HPP_

#include <type_traits>

#include "../basetypes/AurumTypes.hpp"

namespace aurum {
namespace io {

template <typename DerivedType>
class FilterBase : public AurumObject<aurum::io::FilterBase<DerivedType> >
{
    static_assert(std::is_base_of<std::streambuf, DerivedType>::value,
                  "FilterBase<> can only be subclassed by classes "
                  "which derive from std::streambuf<>");

protected:
    std::streambuf* m_downstream_buffer;

public:
    inline FilterBase()
        : m_downstream_buffer(nullptr)
    {
        // Nothing here
    }

    inline FilterBase(std::streambuf* downstream_buffer)
        : m_downstream_buffer(downstream_buffer)
    {
        // Nothing here
    }

    inline FilterBase(const FilterBase& other)
        : m_downstream_buffer(other.m_downstream_buffer)
    {
        // Nothing here
    }

    inline FilterBase(FilterBase&& other)
        : FilterBase()
    {
        std::swap(m_downstream_buffer, other.m_downstream_buffer);
    }

    inline ~FilterBase()
    {
        // Nothing here
    }

    inline FilterBase& operator = (const FilterBase& other)
    {
        if (&other == this) {
            return *this;
        }
        m_downstream_buffer = other.m_downstream_buffer;
        return *this;
    }

    inline FilterBase& operator = (FilterBase&& other)
    {
        if (&other == this) {
            return *this;
        }
        m_downstream_buffer = other.m_downstream_buffer;
        other.m_downstream_buffer = nullptr;
        return *this;
    }
};

} /* end namespace io */
} /* end namespace aurum */

#endif /* AURUM_IO_FILTER_BASE_HPP_ */

//
// FilterBase.hpp ends here
