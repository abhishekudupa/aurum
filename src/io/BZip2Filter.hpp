// BZip2Filter.hpp ---
//
// Filename: BZip2Filter.hpp
// Author: Abhishek Udupa
// Created: Tue Aug 11 13:29:25 2015 (-0400)
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

#if !defined AURUM_IO_BZIP2_FILTER_HPP_
#define AURUM_IO_BZIP2_FILTER_HPP_

#include <bzlib.h>

#include "FilterBase.hpp"

namespace aurum {
namespace io {

namespace detail_ {

class BZip2FilterData
{
protected:
    static constexpr u32 sc_default_scratch_buffer_size = 262144;
    static constexpr u32 sc_min_scratch_buffer_size = 65536;
    static constexpr u32 sc_default_compression_work_factor = 30;
    static constexpr u32 sc_default_compression_block_size = 9;

    bz_stream m_bzip_stream;
    u32 m_scratch_buffer_size;
    u08* m_scratch_buffer;

    BZip2FilterData(u32 scratch_buffer_size = sc_default_scratch_buffer_size);
    virtual ~BZip2FilterData();
};

} /* end namespace detail_ */

template <typename IOCategory>
class BZip2Filter
    : public detail_::BZip2FilterData,
      public std::conditional<std::is_same<IOCategory, Input>::value,
                              SequentialInputFilterBase, SequentialOutputFilterBase>::type
{
    static_assert(std::is_same<IOCategory, Input>::value ||
                  std::is_same<IOCategory, Output>::value,
                  "Class BZip2Filter can only be instantiated with Input or Output as "
                  "the IOCategory argument!");
};

class BZip2InputFilter
    : public detail_::BZip2FilterData,
      public SequentialInputFilterBase
{
private:
    inline u64 refill_buffer();

public:
    BZip2InputFilter(std::streambuf* chained_buffer,
                     u64 buffer_size = sc_default_buffer_size);
    BZip2InputFilter() = delete;
    virtual ~BZip2InputFilter();

protected:
    virtual int_type underflow() override;
    virtual std::streamsize xsgetn(char_type* s, std::streamsize count) override;
    virtual int_type pbackfail(int_type c = traits_type::eof()) override;
};

class BZip2OutputFilter
    : public detail_::BZip2FilterData,
      public SequentialOutputFilterBase
{
private:
    inline void drain_buffer(bool sync = false, bool final_block = false);

public:
    BZip2OutputFilter(std::streambuf* chained_buffer,
                      u32 buffer_size = sc_default_buffer_size,
                      u32 scratch_buffer_size = sc_default_scratch_buffer_size,
                      u32 compression_work_factor = sc_default_compression_work_factor,
                      u32 compression_block_size = sc_default_compression_block_size);
    BZip2OutputFilter() = delete;
    virtual ~BZip2OutputFilter();

protected:
    virtual std::streamsize xsputn(const char_type* s, std::streamsize n) override;
    virtual int_type overflow(int_type ch = traits_type::eof()) override;
    virtual int sync() override;
    void finalize();
};

template <>
class BZip2Filter<Input> : public BZip2InputFilter
{
public:
    using BZip2InputFilter::BZip2InputFilter;
};

template <>
class BZip2Filter<Output> : public BZip2OutputFilter
{
public:
    using BZip2OutputFilter::BZip2OutputFilter;
};

} /* end namespace io */
} /* end namespace aurum */

#endif /* AURUM_IO_BZIP2_FILTER_HPP_ */


//
// BZip2Filter.hpp ends here
