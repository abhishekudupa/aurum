// BZip2Filter.cpp ---
//
// Filename: BZip2Filter.cpp
// Author: Abhishek Udupa
// Created: Tue Aug 11 15:00:56 2015 (-0400)
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

#include <string.h>

#include "../allocators/MemoryManager.hpp"

#include "BZip2Filter.hpp"
#include "AurumIOException.hpp"

#ifdef AURUM_CFG_HAVE_BZIP2_

namespace aurum {
namespace io {

namespace aa = aurum::allocators;

namespace detail_ {

BZip2FilterData::BZip2FilterData(u32 scratch_buffer_size)
    : m_scratch_buffer_size(scratch_buffer_size > sc_min_scratch_buffer_size ?
                            scratch_buffer_size : sc_min_scratch_buffer_size),
      m_scratch_buffer((u08*)aa::allocate_raw(m_scratch_buffer_size))
{
    m_bzip_stream.bzalloc = aa::allocate_fun_for_compressioni32;
    m_bzip_stream.bzfree = aa::deallocate_fun_for_compression;
    m_bzip_stream.opaque = nullptr;
    m_bzip_stream.next_in = nullptr;
    m_bzip_stream.avail_in = 0;
    m_bzip_stream.next_out = nullptr;
    m_bzip_stream.avail_out = 0;
}

BZip2FilterData::~BZip2FilterData()
{
    aa::deallocate_raw(m_scratch_buffer, m_scratch_buffer_size);
}

} /* end namespace detail_ */

// implementation of BZip2InputFilter
BZip2InputFilter::BZip2InputFilter(std::streambuf* chained_buffer, u64 buffer_size)
    : BZip2FilterData(), SequentialInputFilterBase(chained_buffer, buffer_size)
{
    auto status = BZ2_bzDecompressInit(&m_bzip_stream, 0, 0);
    if (status != BZ_OK) {
        throw AurumIOException((std::string)"Error initializing BZip2 stream!");
    }
}

BZip2InputFilter::~BZip2InputFilter()
{
    BZ2_bzDecompressEnd(&m_bzip_stream);
}

inline u64 BZip2InputFilter::refill_buffer()
{
    u64 bytes_from_chained_buf = 0;
    int inflate_status;

    while (true) {
        if (m_bzip_stream.avail_in <= 0) {
            bytes_from_chained_buf =
                m_chained_buffer->sgetn((char_type*)m_scratch_buffer, m_scratch_buffer_size);
            if (bytes_from_chained_buf == 0) {
                return 0;
            }
            m_bzip_stream.next_in = (char*)m_scratch_buffer;
            m_bzip_stream.avail_in = bytes_from_chained_buf;
        }

        // we now have data
        m_bzip_stream.next_out = (char*)m_buffer;
        m_bzip_stream.avail_out = m_buffer_size;

        inflate_status = BZ2_bzDecompress(&m_bzip_stream);

        if (m_bzip_stream.avail_out < m_buffer_size) {
            if (inflate_status == BZ_OK || inflate_status == BZ_STREAM_END) {
                return (m_buffer_size - m_bzip_stream.avail_out);
            } else {
                throw AurumIOException((std::string)"Error when decompressing BZip2 stream!");
            }
        } else {
            continue;
        }
    }
}

BZip2InputFilter::int_type BZip2InputFilter::underflow()
{
    if (gptr() < egptr()) {
        return (*(gptr()));
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

std::streamsize BZip2InputFilter::xsgetn(char_type* s, std::streamsize count)
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

BZip2InputFilter::int_type BZip2InputFilter::pbackfail(int_type c)
{
    throw AurumIOException((std::string)"Attempted to pushback too many characters " +
                           "into BZip2 input stream!");
}

// implementation of BZip2OutputFilter
BZip2OutputFilter::BZip2OutputFilter(std::streambuf* chained_buffer,
                                     u32 buffer_size,
                                     u32 scratch_buffer_size,
                                     u32 compression_work_factor,
                                     u32 compression_block_size)
    : BZip2FilterData(scratch_buffer_size),
      SequentialOutputFilterBase(chained_buffer, buffer_size)
{
    auto status = BZ2_bzCompressInit(&m_bzip_stream, compression_block_size,
                                     0, compression_work_factor);
    if (status != BZ_OK) {
        throw AurumIOException((std::string)"Error initializing BZip2 stream");
    }
}

BZip2OutputFilter::~BZip2OutputFilter()
{
    drain_buffer(false, true);
    BZ2_bzCompressEnd(&m_bzip_stream);
}

inline void BZip2OutputFilter::drain_buffer(bool sync, bool final_block)
{
    m_bzip_stream.avail_in = pptr() - (char_type*)m_buffer;
    m_bzip_stream.next_in = (char*)m_buffer;

    int flush_mode = BZ_RUN;

    if (final_block) {
        flush_mode = BZ_FINISH;
    } else if (sync) {
        flush_mode = BZ_FLUSH;
    }

    while (true) {
        m_bzip_stream.next_out = (char*)m_scratch_buffer;
        m_bzip_stream.avail_out = m_scratch_buffer_size;

        auto status = BZ2_bzCompress(&m_bzip_stream, flush_mode);

        if (flush_mode == BZ_RUN) {
            if (status != BZ_RUN_OK) {
                throw AurumIOException((std::string)"Error while compressing BZip2 stream!");
            }
            auto avail_in_scratch = m_scratch_buffer_size - m_bzip_stream.avail_out;
            m_chained_buffer->sputn((char_type*)m_scratch_buffer, avail_in_scratch);
            if (m_bzip_stream.avail_in == 0) {
                break;
            } else {
                continue;
            }
        } else if (flush_mode == BZ_FINISH) {
            if (status != BZ_FINISH_OK && status != BZ_STREAM_END) {
                throw AurumIOException((std::string)"Error while compressing BZip2 stream!");
            }
            auto avail_in_scratch = m_scratch_buffer_size - m_bzip_stream.avail_out;
            m_chained_buffer->sputn((char_type*)m_scratch_buffer, avail_in_scratch);

            if (status == BZ_STREAM_END) {
                break;
            } else {
                continue;
            }
        } else if (flush_mode == BZ_FLUSH) {
            if (status != BZ_RUN_OK && status != BZ_FLUSH_OK) {
                throw AurumIOException((std::string)"Error while compressing BZip2 stream!");
            }
            auto avail_in_scratch = m_scratch_buffer_size - m_bzip_stream.avail_out;
            m_chained_buffer->sputn((char_type*)m_scratch_buffer, avail_in_scratch);

            if (status == BZ_RUN_OK) {
                break;
            } else {
                continue;
            }
        }
    }

    // reset the put area
    setp((char_type*)m_buffer, (char_type*)(m_buffer + m_buffer_size));
}

BZip2OutputFilter::int_type BZip2OutputFilter::overflow(int_type ch)
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

    if (!traits_type::eq_int_type(ch, traits_type::eof())) {
        sputc((char_type)ch);
        return ch;
    } else {
        return 0;
    }
}

std::streamsize BZip2OutputFilter::xsputn(const char_type* s, std::streamsize n)
{
    u64 bytes_put = 0;
    overflow();

    while (bytes_put < (u64)n) {
        u64 put_available = epptr() - pptr();
        auto num_to_copy = std::min(put_available, n - bytes_put);
        memcpy(pptr() , s + bytes_put, num_to_copy);
        bytes_put += num_to_copy;
        setp(pptr() + num_to_copy, epptr());
        overflow();
    }

    return bytes_put;
}

BZip2OutputFilter::int_type BZip2OutputFilter::sync()
{
    drain_buffer(true, false);
    return m_chained_buffer->pubsync();
}

} /* end namespace io */
} /* end namespace aurum */

#endif /* AURUM_CFG_HAVE_BZIP2_ */

//
// BZip2Filter.cpp ends here
