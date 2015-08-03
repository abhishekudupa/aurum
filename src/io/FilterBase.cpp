// FilterBase.cpp ---
//
// Filename: FilterBase.cpp
// Author: Abhishek Udupa
// Created: Mon Aug  3 15:41:11 2015 (-0400)
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

#include "FilterBase.hpp"
#include "AurumIOException.hpp"

namespace aurum {
namespace io {

FilterBase::FilterBase(std::streambuf* piped_buffer)
    : m_piped_buffer(piped_buffer)
{
    // Nothing here
}

FilterBase::FilterBase(const FilterBase& other)
    : m_piped_buffer(other.m_piped_buffer)
{
    // Nothing here
}

FilterBase::FilterBase(FilterBase&& other)
    : FilterBase()
{
    std::swap(m_piped_buffer, other.m_piped_buffer);
}

FilterBase& FilterBase::operator = (const FilterBase& other)
{
    if (&other == this) {
        return *this;
    }

    m_piped_buffer = other.m_piped_buffer;
    return *this;
}

FilterBase& FilterBase::operator = (FilterBase&& other)
{
    if (&other == this) {
        return *this;
    }

    m_piped_buffer = other.m_piped_buffer;
    other.m_piped_buffer = nullptr;
    return *this;
}

std::streambuf* FilterBase::get_piped_buffer() const
{
    return m_piped_buffer;
}


// Implementation of CheckedFilterBase
inline void
CheckedFilterBase::throw_unsupported_op_exception(const std::string& op_name) const
{
    throw AurumIOException((std::string)"Filter does not support operation: " + op_name);
}

CheckedFilterBase::~CheckedFilterBase()
{
    // Nothing here
}

CheckedFilterBase& CheckedFilterBase::operator = (const CheckedFilterBase& other)
{
    return static_cast<CheckedFilterBase&>(FilterBase::operator=(other));
}

CheckedFilterBase& CheckedFilterBase::operator=(CheckedFilterBase&& other)
{
    return static_cast<CheckedFilterBase&>(FilterBase::operator=(std::move(other)));
}

void CheckedFilterBase::imbue(const std::locale& loc)
{
    throw_unsupported_op_exception("imbue()");
}

std::streambuf* CheckedFilterBase::setbuf(char_type* s, std::streamsize n)
{
    throw_unsupported_op_exception("setbuf()");
    return nullptr;
}

CheckedFilterBase::pos_type CheckedFilterBase::seekoff(off_type offset,
                                                       std::ios_base::seekdir dir,
                                                       std::ios_base::openmode which)
{
    throw_unsupported_op_exception("seekoff()");
    return pos_type();
}

CheckedFilterBase::pos_type CheckedFilterBase::seekpos(pos_type pos,
                                                       std::ios_base::openmode which)
{
    throw_unsupported_op_exception("seekpos()");
    return pos_type();
}

int CheckedFilterBase::sync()
{
    throw_unsupported_op_exception("sync()");
    return 0;
}

std::streamsize CheckedFilterBase::showmanyc()
{
    throw_unsupported_op_exception("showmanyc()");
    return 0;
}

CheckedFilterBase::int_type CheckedFilterBase::underflow()
{
    throw_unsupported_op_exception("underflow()");
    return 0;
}

CheckedFilterBase::int_type CheckedFilterBase::uflow()
{
    throw_unsupported_op_exception("uflow()");
    return 0;
}

std::streamsize CheckedFilterBase::xsgetn(char_type* s, std::streamsize count)
{
    throw_unsupported_op_exception("xsgetn()");
    return 0;
}

std::streamsize CheckedFilterBase::xsputn(const char_type* s, std::streamsize count)
{
    throw_unsupported_op_exception("xsputn()");
    return 0;
}

CheckedFilterBase::int_type CheckedFilterBase::overflow(int_type ch)
{
    throw_unsupported_op_exception("overflow()");
    return 0;
}

CheckedFilterBase::int_type CheckedFilterBase::pbackfail(int_type c)
{
    throw_unsupported_op_exception("pbackfail()");
    return 0;
}

} /* end namespace io */
} /* end namespace aurum */

//
// FilterBase.cpp ends here
