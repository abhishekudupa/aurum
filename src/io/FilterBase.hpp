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

class FilterBase : public AurumObject<FilterBase>,
                   public std::streambuf
{
protected:
    std::streambuf* m_piped_buffer;

public:
    FilterBase(std::streambuf* piped_buffer);
    FilterBase(const FilterBase& other);
    FilterBase(FilterBase&& other);
    virtual ~FilterBase();

    FilterBase& operator = (const FilterBase& other);
    FilterBase& operator = (FilterBase&& other);

    std::streambuf* get_piped_buffer() const;

    // the protected functions are as in std::streambuf
};

class CheckedFilterBase : public FilterBase
{
private:
    inline void throw_unsupported_op_exception(const std::string& op_name) const;

public:
    using FilterBase::FilterBase;
    virtual ~CheckedFilterBase();

    CheckedFilterBase& operator = (const CheckedFilterBase& other);
    CheckedFilterBase& operator = (CheckedFilterBase&& other);

    // calling any of these overrides results in an exception being
    // thrown unless further overriden by a subclass
    virtual void imbue(const std::locale& loc) override;
    virtual std::streambuf* setbuf(char_type* s, std::streamsize n) override;
    virtual pos_type seekoff(off_type offset, std::ios_base::seekdir dir,
                             std::ios_base::openmode which =
                             std::ios_base::in | std::ios_base::out) override;
    virtual pos_type seekpos(pos_type pos,
                             std::ios_base::openmode which =
                             std::ios_base::in | std::ios_base::out) override;
    virtual int sync() override;
    virtual std::streamsize showmanyc() override;
    virtual int_type underflow() override;
    virtual int_type uflow() override;
    virtual std::streamsize xsgetn(char_type* s, std::streamsize count) override;
    virtual std::streamsize xsputn(const char_type* s, std::streamsize count) override;
    virtual int_type overflow(int_type ch = traits_type::eof());
    virtual int_type pbackfail(int_type c = traits_type::eof());
};

} /* end namespace io */
} /* end namespace aurum */

#endif /* AURUM_IO_FILTER_BASE_HPP_ */

//
// FilterBase.hpp ends here
