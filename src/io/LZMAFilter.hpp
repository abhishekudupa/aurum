// LZMAFilter.hpp ---
//
// Filename: LZMAFilter.hpp
// Author: Abhishek Udupa
// Created: Wed Aug 12 14:47:36 2015 (-0400)
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

#if !defined AURUM_IO_LZMA_FILTER_HPP_
#define AURUM_IO_LZMA_FILTER_HPP_

#include <AurumConfig.h>

#ifdef AURUM_CFG_HAVE_LZMA_

#include <lzma.h>

#include "FilterBase.hpp"

namespace aurum {
namespace io {

namespace detail_ {

class LZMAFilterData
{
protected:
    static constexpr u32 sc_default_scratch_buffer_size = 262144;
    static constexpr u32 sc_min_scratch_buffer_size = 65536;
    // use upto 256 MB of memory
    static constexpr u32 sc_default_compression_memory = (1 << 28);
    static constexpr u32 sc_default_compression_preset_level = 9;
    static constexpr bool sc_default_compression_use_extreme_preset = true;

    lzma_allocator m_lzma_allocators;
    lzma_stream m_lzma_stream;
    u32 m_scratch_buffer_size;
    u08* m_scratch_buffer;

    LZMAFilterData(u32 scratch_buffer_size = sc_default_scratch_buffer_size);
    virtual ~LZMAFilterData();
};

} /* end namespace detail_ */

template <typename IOCategory>
class LZMAFilter
    : public detail_::LZMAFilterData,
      public std::conditional<std::is_same<IOCategory, Input>::value,
                              SequentialInputFilterBase, SequentialOutputFilterBase>::type
{
    static_assert(std::is_same<IOCategory, Input>::value ||
                  std::is_same<IOCategory, Output>::value,
                  "Class LZMAFilter can only be instantiated with Input or Output as "
                  "the IOCategory argument!");
};

class LZMAInputFilter
    : public detail_::LZMAFilterData,
      public SequentialInputFilterBase
{
private:
    inline u64 refill_buffer();

public:
    LZMAInputFilter(std::streambuf* chained_buffer, u64 buffer_size = sc_default_buffer_size,
                    u64 mem_limit = sc_default_compression_memory);
    LZMAInputFilter() = delete;
    virtual ~LZMAInputFilter();

protected:
    virtual int_type underflow() override;
    virtual std::streamsize xsgetn(char_type* s, std::streamsize count) override;
    virtual int_type pbackfail(int_type c = traits_type::eof()) override;
};

class LZMAOutputFilter
    : public detail_::LZMAFilterData,
      public SequentialOutputFilterBase
{
private:
    inline void drain_buffer(bool sync = false, bool final_block = false);

public:
    LZMAOutputFilter(std::streambuf* chained_buffer,
                     u32 buffer_size = sc_default_buffer_size,
                     u32 scratch_buffer_size = sc_default_scratch_buffer_size,
                     u32 compression_memory = sc_default_compression_memory,
                     u32 compression_preset_level = sc_default_compression_preset_level,
                     bool compression_use_extreme_preset =
                     sc_default_compression_use_extreme_preset);
    LZMAOutputFilter() = delete;
    virtual ~LZMAOutputFilter();

protected:
    virtual std::streamsize xsputn(const char_type* s, std::streamsize n) override;
    virtual int_type overflow(int_type ch = traits_type::eof()) override;
    virtual int sync() override;
};

template <>
class LZMAFilter<Input> : public LZMAInputFilter
{
public:
    using LZMAInputFilter::LZMAInputFilter;
};

template <>
class LZMAFilter<Output> : public LZMAOutputFilter
{
public:
    using LZMAOutputFilter::LZMAOutputFilter;
};

} /* end namespace io */
} /* end namespace aurum */

#endif /* AURUM_CFG_HAVE_LZMA_ */

#endif /* AURUM_IO_LZMA_FILTER_HPP_ */

//
// LZMAFilter.hpp ends here
