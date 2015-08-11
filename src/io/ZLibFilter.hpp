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

class ZLibFilterBase
{
protected:
    // constants governing buffer/chunk sizes
    static constexpr u32 sc_default_chunk_size = 65536;
    static constexpr u32 sc_min_chunk_size = 4096;
    // constants affecting compression levels
    static constexpr i32 sc_default_compression_level = Z_BEST_COMPRESSION;
    static constexpr i32 sc_default_compression_method = Z_DEFLATED;
    static constexpr i32 sc_default_compression_window_bits = 15;
    static constexpr i32 sc_default_compression_memlevel = 9;
    static constexpr i32 sc_default_compression_strategy = Z_DEFAULT_STRATEGY;

private:
    bool m_is_input;
    bool m_stream_finished;
    z_stream m_zlib_stream;
    std::streambuf* m_io_buffer;
    u32 m_scratch_buffer_size;
    u08* m_scratch_buffer;

    inline void check_input() const;
    inline void check_output() const;

protected:
    ZLibFilterBase(bool is_input, std::streambuf* io_buffer,
                   bool use_gzip_wrapper = false,
                   u32 chunk_size = sc_default_chunk_size,
                   i32 compression_level = sc_default_compression_level);

    ZLibFilterBase() = delete;
    ZLibFilterBase(const ZLibFilterBase& other) = delete;
    ZLibFilterBase(ZLibFilterBase&& other) = delete;
    ZLibFilterBase& operator = (const ZLibFilterBase& other) = delete;
    ZLibFilterBase& operator = (ZLibFilterBase&& other) = delete;

    virtual ~ZLibFilterBase();

    u64 read_bytes(u08* output_buffer, u64 output_buffer_size);
    u64 write_bytes(u08* input_buffer, u64 input_buffer_size, bool flush_stream = false);
    inline void finalize();
};

class ZLibFilterBase
{
protected:
    // constants governing buffer/chunk sizes
    static constexpr u32 sc_default_chunk_size = 65536;
    static constexpr u32 sc_min_chunk_size = 4096;
    // constants affecting compression levels
    static constexpr i32 sc_default_compression_level = Z_BEST_COMPRESSION;
    static constexpr i32 sc_default_compression_method = Z_DEFLATED;
    static constexpr i32 sc_default_compression_window_bits = 15;
    static constexpr i32 sc_default_compression_memlevel = 9;
    static constexpr i32 sc_default_compression_strategy = Z_DEFAULT_STRATEGY;

private:
    // the stream is private
    z_stream m_zlib_stream;

    // helpers
    inline void check_input() const;
    inline void check_output() const;
    inline void check_continue_decompression() const;
    inline void check_continue_compression() const;
    inline void check_begin_decompression() const;
    inline void check_begin_compression() const;

protected:
    bool m_is_input;
    bool m_is_final_block;
    bool m_decompressing_block;
    bool m_compressing_block;
    bool m_compressing_block_in_sync_mode;
    u32 m_scratch_buffer_size;
    u32 m_current_output_buffer_size;
    u08* m_scratch_buffer;
    u08* m_current_output_buffer;

    // initializes the stream
    ZLibFilterBase() = delete;
    ZLibFilterBase(bool is_input, u32 chunk_size, bool use_gzip_wrapper,
                   i32 compression_level);
    virtual ~ZLibFilterBase();

    // compression
    // returns the number of bytes available in the scratch buffer
    // retval < m_buffer_size implies that we're done
    u32 begin_block_compression(u08* input_block, u32 block_size,
                                bool do_sync = false,
                                bool is_final_block = false);

    // returns the number of bytes available for consumption in the scratch buffer
    // retval < m_buffer_size implies we're done
    // requires: input_block not be modified since last invocation of begin_block_compression
    u32 continue_block_compression();

    // decompression
    // pushes data into output block from scratch buffer
    // retval < block_size implies that scratch buffer is completely decompressed
    // and can be filled with new data.
    u32 begin_block_decompression(u08* output_block, u32 block_size, u32 avail_in_scratch_buffer);

    // continues the decompression of scratch buffer
    // requires: scratch buffer not modified since last invocation of begin_block_decompression
    // retval < block_size implies that scratch buffer is completely decompressed
    // and can be filled with new data.
    u32 continue_block_decompression();

    void finalize();
};

} /* end namespace detail_ */

template <typename IOCategory>
class ZLibFilter
    : public detail_::ZLibFilterBase,
      public std::conditional<std::is_same<IOCategory, Input>::value,
                              SequentialInputFilterBase, SequentialOutputFilterBase>::type
{
    static_assert(!(std::is_same<IOCategory, Input>::value ||
                    std::is_same<IOCategory, Output>::value),
                  "class ZLibFilter can only be instantiated with Input or Output as template "
                  "arguments!");
};

class ZLibInputFilter
    : public detail_::ZLibFilterBase,
      public SequentialInputFilterBase
{
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
    : public detail_::ZLibFilterBase,
      public SequentialOutputFilterBase
{
private:
    inline void do_overflow(bool sync_compression = false, bool final_block = false);

public:
    ZLibOutputFilter(std::streambuf* chained_buffer,
                     i32 compression_level = sc_default_compression_level,
                     bool use_gzip_stream = true,
                     u64 buffer_size = sc_default_buffer_size,
                     u32 chunk_size = sc_default_chunk_size);
    ZLibOutputFilter() = delete;
    virtual ~ZLibOutputFilter();

protected:
    virtual std::streamsize xsputn(const char_type* s, std::streamsize n) override;
    virtual int_type overflow(int_type ch = traits_type::eof()) override;
    virtual int sync() override;
    void finalize();
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
