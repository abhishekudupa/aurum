// ZLibFilter.cpp ---
// Filename: ZLibFilter.cpp
// Author: Abhishek Udupa
// Created: Sun Aug  9 17:09:48 2015 (-0400)
//
// Copyright (c) 2013, Abhishek Udupa, University of Pennsylvania
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

#include <string.h>

#include "../allocators/MemoryManager.hpp"

#include "ZLibFilter.hpp"
#include "AurumIOException.hpp"

#ifdef AURUM_CFG_HAVE_ZLIB_

namespace aurum {
namespace io {

namespace detail_ {

ZLibFilterData::ZLibFilterData(u32 scratch_buffer_size)
    : m_scratch_buffer_size(scratch_buffer_size > sc_min_scratch_buffer_size ?
                            scratch_buffer_size : sc_min_scratch_buffer_size),
      m_scratch_buffer((u08*)aa::allocate_raw(m_scratch_buffer_size))
{
    m_zlib_stream.opaque = nullptr;
    m_zlib_stream.zalloc = aa::allocate_fun_for_compression32;
    m_zlib_stream.zfree = aa::deallocate_fun_for_compression;
    m_zlib_stream.avail_in = 0;
    m_zlib_stream.next_in = nullptr;
    m_zlib_stream.avail_out = 0;
    m_zlib_stream.next_out = nullptr;
}

ZLibFilterData::~ZLibFilterData()
{
    aa::deallocate_raw(m_scratch_buffer, m_scratch_buffer_size);
}

} /* end namespace detail_ */

namespace aa = aurum::allocators;

ZLibInputFilter::ZLibInputFilter(std::streambuf* chained_buffer, u64 buffer_size)
    : ZLibFilterData(), SequentialInputFilterBase(chained_buffer, buffer_size)
{
    auto status = inflateInit2(&m_zlib_stream, sc_default_compression_windows_bits + 32);
    if (status != Z_OK) {
        throw AurumIOException((std::string)"Error initializing ZLib stream!");
    }
}

ZLibInputFilter::~ZLibInputFilter()
{
    inflateEnd(&m_zlib_stream);
}

inline u64 ZLibInputFilter::refill_buffer()
{
    u64 bytes_from_chained_buf = 0;
    int inflate_status;

    while (true) {
        if (m_zlib_stream.avail_in <= 0) {
            bytes_from_chained_buf =
                m_chained_buffer->sgetn((char_type*)m_scratch_buffer, m_scratch_buffer_size);
            if (bytes_from_chained_buf == 0) {
                return 0;
            }
            m_zlib_stream.next_in = m_scratch_buffer;
            m_zlib_stream.avail_in = bytes_from_chained_buf;
        }

        // we're now guaranteed to have some data for the
        // decompressor to process
        m_zlib_stream.next_out = m_buffer;
        m_zlib_stream.avail_out = m_buffer_size;

        inflate_status = inflate(&m_zlib_stream, Z_NO_FLUSH);

        if (m_zlib_stream.avail_out < m_buffer_size) {
            if (inflate_status == Z_OK || inflate_status == Z_STREAM_END) {
                return (m_buffer_size - m_zlib_stream.avail_out);
            } else {
                throw AurumIOException((std::string)"Error when decompressing ZLib stream!");
            }
        } else if (inflate_status == Z_BUF_ERROR) {
            continue;
        } else {
            throw AurumIOException((std::string)"Error when decompressing ZLib stream!");
        }
    }
}

ZLibInputFilter::int_type ZLibInputFilter::underflow()
{
    if (gptr() < egptr()) {
        return *(gptr());
    }

    auto refilled_bytes = refill_buffer();
    if (refilled_bytes == 0) {
        setg((char_type*)m_buffer, (char_type*)m_buffer, (char_type*)m_buffer);
        return traits_type::eof();
    } else {
        setg((char_type*)m_buffer, (char_type*)m_buffer,
             (char_type*)(m_buffer + refilled_bytes));
        return (*(gptr()));
    }
}

std::streamsize ZLibInputFilter::xsgetn(char_type* s, std::streamsize count)
{
    u64 num_left = count;
    u64 s_offset = 0;

    underflow();

    while (num_left > 0) {
        u64 num_available = egptr() - gptr();

        if (num_available == 0) {
            return s_offset;
        }

        auto num_to_copy = std::min(num_available, num_left);
        memcpy(s + s_offset, gptr(), num_to_copy);
        num_left -= num_to_copy;
        num_available -= num_to_copy;
        s_offset += num_to_copy;
        setg((char_type*)m_buffer, gptr() + num_to_copy, egptr());

        if (num_available == 0) {
            underflow();
        }
    }

    return s_offset;
}

ZLibInputFilter::int_type ZLibInputFilter::pbackfail(int_type c)
{
    throw AurumIOException((std::string)"Attempted to pushback too many characters " +
                           "into ZLib input stream!");
}

// implementation of ZLibOutputFilter
ZLibOutputFilter::ZLibOutputFilter(std::streambuf* chained_buffer, bool use_gzip_stream,
                                   i32 compression_level, u32 buffer_size,
                                   u32 scratch_buffer_size, u32 mem_level)
    : ZLibFilterData(scratch_buffer_size),
      SequentialOutputFilterBase(chained_buffer, buffer_size)
{
    // initialize the z_stream for compression
    auto status = deflateInit2(&m_zlib_stream, compression_level,
                               sc_default_compression_method,
                               sc_default_compression_windows_bits +
                               (use_gzip_stream ? 16 : 0),
                               mem_level, sc_default_compression_strategy);
    if (status != Z_OK) {
        throw AurumIOException((std::string)"Error initializing ZLib stream!");
    }
}

ZLibOutputFilter::~ZLibOutputFilter()
{
    drain_buffer(false, true);
    deflateEnd(&m_zlib_stream);
}

inline void ZLibOutputFilter::drain_buffer(bool sync, bool final_block)
{
    m_zlib_stream.avail_in = pptr() - (char_type*)m_buffer;
    m_zlib_stream.next_in = m_buffer;

    int flush_mode = Z_NO_FLUSH;
    if (final_block) {
        flush_mode = Z_FINISH;
    } else if (sync) {
        flush_mode = Z_SYNC_FLUSH;
    }

    int deflate_status;

    while (m_zlib_stream.avail_in > 0) {
        m_zlib_stream.next_out = m_scratch_buffer;
        m_zlib_stream.avail_out = m_scratch_buffer_size;

        deflate_status = deflate(&m_zlib_stream, flush_mode);

        if (deflate_status != Z_OK && deflate_status != Z_STREAM_END &&
            deflate_status != Z_BUF_ERROR) {
            throw AurumIOException((std::string)"Error while compressing ZLib stream!");
        }

        auto available_in_scratch = m_scratch_buffer_size - m_zlib_stream.avail_out;
        if (available_in_scratch > 0) {
            m_chained_buffer->sputn((char_type*)m_scratch_buffer, available_in_scratch);
        }
    }

    // reset the put area
    setp((char_type*)m_buffer, (char_type*)(m_buffer + m_buffer_size));
}

ZLibOutputFilter::int_type ZLibOutputFilter::overflow(int_type ch)
{
    if (pptr() < epptr()) {
        if (!traits_type::eq_int_type(ch, traits_type::eof())) {
            sputc((char_type)ch);
            return ch;
        } else {
            return 0;
        }
    }

    drain_buffer();

    // push out the entire put buffer into the chained streambuf
    // after compressing
    if (!traits_type::eq_int_type(ch, traits_type::eof())) {
        sputc((char_type)ch);
        return ch;
    } else {
        return 0;
    }
}

std::streamsize ZLibOutputFilter::xsputn(const char_type* s, std::streamsize n)
{
    u64 bytes_put = 0;
    overflow();

    while (bytes_put < (u64)n) {
        u64 put_available = epptr() - pptr();
        auto num_to_copy = std::min(put_available, n - bytes_put);
        memcpy(pptr(), s + bytes_put, num_to_copy);
        bytes_put += num_to_copy;
        setp(pptr() + num_to_copy, epptr());
        overflow();
    }

    return bytes_put;
}

ZLibOutputFilter::int_type ZLibOutputFilter::sync()
{
    drain_buffer(true, false);
    return m_chained_buffer->pubsync();
}

} /* end namespace io */
} /* end namespace aurum */

#endif /* AURUM_CFG_HAVE_ZLIB_ */

//
// ZLibFilter.cpp ends here
