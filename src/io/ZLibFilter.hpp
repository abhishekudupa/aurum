// ZLibFilter.hpp ---
//
// Filename: ZLibFilter.hpp
// Author: Abhishek Udupa
// Created: Mon Aug  3 18:42:44 2015 (-0400)
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

#if !defined AURUM_IO_ZLIB_FILTER_HPP_
#define AURUM_IO_ZLIB_FILTER_HPP_

#include <zlib.h>

#include "FilterBase.hpp"

namespace aurum {
namespace io {

namespace detail_ {

// class for common defaults and common data for input and output zlib filters
class ZLibFilterData
{
protected:
    static constexpr u32 sc_default_scratch_buffer_size = 262144;
    static constexpr u32 sc_min_scratch_buffer_size = 65536;
    static constexpr i32 sc_default_compression_level = Z_BEST_COMPRESSION;
    static constexpr i32 sc_default_compression_method = Z_DEFLATED;
    static constexpr i32 sc_default_compression_windows_bits = 15;
    static constexpr i32 sc_default_compression_memlevel = 9;
    static constexpr i32 sc_default_compression_strategy = Z_DEFAULT_STRATEGY;

    z_stream m_zlib_stream;
    u32 m_scratch_buffer_size;
    u08* m_scratch_buffer;

    ZLibFilterData(u32 scratch_buffer_size = sc_default_scratch_buffer_size);
    ~ZLibFilterData();
};

} /* end namespace detail_ */

template <typename IOCategory>
class ZLibFilter
    : public detail_::ZLibFilterData,
      public std::conditional<std::is_same<IOCategory, Input>::value,
                              SequentialInputFilterBase, SequentialOutputFilterBase>::type
{
    static_assert(!(std::is_same<IOCategory, Input>::value ||
                    std::is_same<IOCategory, Output>::value),
                  "class ZLibFilter can only be instantiated with Input or Output as template "
                  "arguments!");
};

class ZLibInputFilter
    : public detail_::ZLibFilterData,
      public SequentialInputFilterBase
{
private:
    // helper routines
    inline u64 refill_buffer();

public:
    ZLibInputFilter(std::streambuf* chained_buffer,
                    u64 buffer_size = sc_default_buffer_size);
    ZLibInputFilter() = delete;
    virtual ~ZLibInputFilter();

protected:
    virtual int_type underflow() override;
    virtual std::streamsize xsgetn(char_type* s, std::streamsize count) override;
    virtual int_type pbackfail(int_type c = traits_type::eof()) override;
};

class ZLibOutputFilter
    : public detail_::ZLibFilterData,
      public SequentialOutputFilterBase
{
private:
    inline void drain_buffer(bool sync = false, bool last_block = false);

public:
    ZLibOutputFilter(std::streambuf* chained_buffer,
                     bool use_gzip_stream = false,
                     i32 compression_level = sc_default_compression_level,
                     u32 buffer_size = sc_default_buffer_size,
                     u32 scratch_buffer_size = sc_default_scratch_buffer_size,
                     u32 mem_level = sc_default_compression_memlevel);

    ZLibOutputFilter() = delete;
    virtual ~ZLibOutputFilter();

protected:
    virtual std::streamsize xsputn(const char_type* s, std::streamsize n) override;
    virtual int_type overflow(int_type ch = traits_type::eof()) override;
    virtual int sync() override;
};

template <>
class ZLibFilter<Input> : public ZLibInputFilter
{
public:
    using ZLibInputFilter::ZLibInputFilter;
};

template <>
class ZLibFilter<Output> : public ZLibOutputFilter
{
public:
    using ZLibOutputFilter::ZLibOutputFilter;
};

} /* end namespace io */
} /* end namespace aurum */

#endif /* AURUM_IO_ZLIB_FILTER_HPP_ */

//
// ZLibFilter.hpp ends here
