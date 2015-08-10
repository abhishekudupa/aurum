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

#include "../allocators/MemoryManager.hpp"

#include "FilterBase.hpp"
#include "AurumIOException.hpp"

namespace aurum {
namespace io {

namespace aa = aurum::allocators;

namespace detail_ {

IOFilterBase::IOFilterBase(std::streambuf* chained_buffer, u64 buffer_size)
    : std::streambuf(), m_buffer_size(buffer_size),
      m_buffer(buffer_size == 0 ? nullptr : (u08*)aa::allocate_raw(buffer_size)),
      m_chained_buffer(chained_buffer)
{
    // Nothing here
}

IOFilterBase::~IOFilterBase()
{
    // delete the buffer that we've allocated
    if (m_buffer != nullptr) {
        aa::deallocate_raw(m_buffer, m_buffer_size);
    }
}

std::streambuf* IOFilterBase::get_chained_buffer() const
{
    return m_chained_buffer;
}

void IOFilterBase::set_chained_buffer(std::streambuf* buffer)
{
    m_chained_buffer = buffer;
}

void IOFilterBase::throw_on_unsupported_operation(const std::string& operation_name) const
{
    throw AurumIOException((std::string)"operation " + operation_name +
                           " not supported on filter.");
}

} /* end namespace detail_ */

SequentialInputFilterBase::FilterBase(std::streambuf* chained_buffer, u64 buffer_size)
    : BaseType(chained_buffer, buffer_size)
{
    setg((char_type*)m_buffer, (char_type*)(m_buffer + m_buffer_size),
         (char_type*)(m_buffer + m_buffer_size));
}

SequentialInputFilterBase::~FilterBase()
{
    // Nothing here
}

void SequentialInputFilterBase::imbue(const std::locale& loc)
{
    throw_on_unsupported_operation("imbue()");
}

std::streambuf* SequentialInputFilterBase::setbuf(char_type* s, std::streamsize n)
{
    throw_on_unsupported_operation("setbuf()");
    return nullptr;
}

SequentialInputFilterBase::pos_type
SequentialInputFilterBase::seekoff(off_type offset,
                                   std::ios_base::seekdir dir,
                                   std::ios_base::openmode which)
{
    throw_on_unsupported_operation("seekoff()");
    return pos_type();
}

SequentialInputFilterBase::pos_type
SequentialInputFilterBase::seekpos(pos_type pos,
                                   std::ios_base::openmode which)
{
    throw_on_unsupported_operation("seekpos()");
    return pos_type();
}

std::streamsize SequentialInputFilterBase::xsputn(const char_type* s, std::streamsize count)
{
    throw_on_unsupported_operation("xsputn()");
    return std::streamsize();
}

SequentialInputFilterBase::int_type SequentialInputFilterBase::overflow(int_type ch)
{
    throw_on_unsupported_operation("overflow()");
    return int_type();
}

RandomAccessInputFilterBase::FilterBase(std::streambuf* chained_buffer, u64 buffer_size)
    : BaseType(chained_buffer, buffer_size)
{
    setg((char_type*)m_buffer, (char_type*)(m_buffer + m_buffer_size),
         (char_type*)(m_buffer + m_buffer_size));
}

RandomAccessInputFilterBase::~FilterBase()
{
    // Nothing here
}

void RandomAccessInputFilterBase::imbue(const std::locale& loc)
{
    throw_on_unsupported_operation("imbue()");
}

std::streambuf* RandomAccessInputFilterBase::setbuf(char_type* s, std::streamsize n)
{
    throw_on_unsupported_operation("setbuf()");
    return nullptr;
}

std::streamsize RandomAccessInputFilterBase::xsputn(const char_type* s, std::streamsize count)
{
    throw_on_unsupported_operation("xsputn()");
    return std::streamsize();
}

RandomAccessInputFilterBase::int_type RandomAccessInputFilterBase::overflow(int_type ch)
{
    throw_on_unsupported_operation("overflow()");
    return int_type();
}

SequentialOutputFilterBase::FilterBase(std::streambuf* chained_buffer, u64 buffer_size)
    : BaseType(chained_buffer, buffer_size)
{
    setp((char_type*)m_buffer, (char_type*)(m_buffer + m_buffer_size));
}

SequentialOutputFilterBase::~FilterBase()
{
    // Nothing here
}

void SequentialOutputFilterBase::imbue(const std::locale& loc)
{
    throw_on_unsupported_operation("imbue()");
}

std::streambuf* SequentialOutputFilterBase::setbuf(char_type* s, std::streamsize n)
{
    throw_on_unsupported_operation("setbuf()");
    return nullptr;
}

SequentialOutputFilterBase::pos_type
SequentialOutputFilterBase::seekoff(off_type offset,
                                    std::ios_base::seekdir dir,
                                    std::ios_base::openmode which)
{
    throw_on_unsupported_operation("seekoff()");
    return pos_type();
}

SequentialOutputFilterBase::pos_type
SequentialOutputFilterBase::seekpos(pos_type pos,
                                    std::ios_base::openmode which)
{
    throw_on_unsupported_operation("seekpos()");
    return pos_type();
}

std::streamsize SequentialOutputFilterBase::showmanyc()
{
    throw_on_unsupported_operation("showmanyc()");
    return std::streamsize();
}

std::streamsize SequentialOutputFilterBase::xsgetn(char_type* s, std::streamsize count)
{
    throw_on_unsupported_operation("xsgetn()");
    return std::streamsize();
}

SequentialOutputFilterBase::int_type SequentialOutputFilterBase::underflow()
{
    throw_on_unsupported_operation("underflow()");
    return int_type();
}

SequentialOutputFilterBase::int_type SequentialOutputFilterBase::uflow()
{
    throw_on_unsupported_operation("uflow()");
    return int_type();
}

RandomAccessOutputFilterBase::FilterBase(std::streambuf* chained_buffer, u64 buffer_size)
    : BaseType(chained_buffer, buffer_size)
{
    setp((char_type*)m_buffer, (char_type*)(m_buffer + m_buffer_size));
}

RandomAccessOutputFilterBase::~FilterBase()
{
    // Nothing here
}

void RandomAccessOutputFilterBase::imbue(const std::locale& loc)
{
    throw_on_unsupported_operation("imbue()");
}

std::streambuf* RandomAccessOutputFilterBase::setbuf(char_type* s, std::streamsize n)
{
    throw_on_unsupported_operation("setbuf()");
    return nullptr;
}

std::streamsize RandomAccessOutputFilterBase::showmanyc()
{
    throw_on_unsupported_operation("showmanyc()");
    return std::streamsize();
}

std::streamsize RandomAccessOutputFilterBase::xsgetn(char_type* s, std::streamsize count)
{
    throw_on_unsupported_operation("xsgetn()");
    return std::streamsize();
}

RandomAccessOutputFilterBase::int_type RandomAccessOutputFilterBase::underflow()
{
    throw_on_unsupported_operation("underflow()");
    return int_type();
}

RandomAccessOutputFilterBase::int_type RandomAccessOutputFilterBase::uflow()
{
    throw_on_unsupported_operation("uflow()");
    return int_type();
}

} /* end namespace io */
} /* end namespace aurum */

//
// FilterBase.cpp ends here
