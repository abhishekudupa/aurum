// LZMAFilter.cpp ---
//
// Filename: LZMAFilter.cpp
// Author: Abhishek Udupa
// Created: Wed Aug 12 16:14:45 2015 (-0400)
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

#include <AurumConfig.h>

#ifdef AURUM_CFG_HAVE_LZMA_

#include <string.h>

#include "../allocators/MemoryManager.hpp"

#include "LZMAFilter.hpp"
#include "AurumIOException.hpp"

namespace aurum {
namespace io {

namespace aa = aurum::allocators;

namespace detail_ {

LZMAFilterData::LZMAFilterData(u32 scratch_buffer_size)
    : m_scratch_buffer_size(scratch_buffer_size > sc_min_scratch_buffer_size ?
                            scratch_buffer_size : sc_min_scratch_buffer_size),
      m_scratch_buffer((u08*)aa::allocate_raw(m_scratch_buffer_size))
{
    m_lzma_allocators.opaque = nullptr;
    m_lzma_allocators.alloc = aa::allocate_fun_for_compression64;
    m_lzma_allocators.free = aa::deallocate_fun_for_compression;

    m_lzma_stream = LZMA_STREAM_INIT;
    m_lzma_stream.allocator = &m_lzma_allocators;
}

LZMAFilterData::~LZMAFilterData()
{
    aa::deallocate_raw(m_scratch_buffer, m_scratch_buffer_size);
}

} /* end namespace detail_ */

LZMAInputFilter::LZMAInputFilter(std::streambuf* chained_buffer, u64 buffer_size,
                                 u64 mem_limit)
    : LZMAFilterData(), SequentialInputFilterBase(chained_buffer, buffer_size)
{
    // initialize the lzma stream
    auto status = lzma_stream_decoder(&m_lzma_stream, mem_limit, LZMA_CONCATENATED);
    if (status != LZMA_OK) {
        throw AurumIOException((std::string)"Error initializing LZMA input stream!");
    }
}

LZMAInputFilter::~LZMAInputFilter()
{
    lzma_end(&m_lzma_stream);
}

inline u64 LZMAInputFilter::refill_buffer()
{
    u64 bytes_from_chained_buf = 0;

    while (true) {
        if (m_lzma_stream.avail_in <= 0) {
            bytes_from_chained_buf =
                m_chained_buffer->sgetn((char_type*)m_scratch_buffer, m_scratch_buffer_size);
            if (bytes_from_chained_buf == 0) {
                return 0;
            }
            m_lzma_stream.avail_in = bytes_from_chained_buf;
            m_lzma_stream.next_in = m_scratch_buffer;
        }

        // we know that we now have data
        m_lzma_stream.avail_out = m_buffer_size;
        m_lzma_stream.next_out = m_buffer;

        auto status = lzma_code(&m_lzma_stream, LZMA_RUN);

        if (m_lzma_stream.avail_out < m_buffer_size) {
            if (status == LZMA_OK || status == LZMA_STREAM_END) {
                return (m_buffer_size - m_lzma_stream.avail_out);
            } else {
                throw AurumIOException((std::string)"Error when decompressing LZMA stream!");
            }
        } else {
            continue;
        }
    }
}

LZMAInputFilter::int_type LZMAInputFilter::underflow()
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

std::streamsize LZMAInputFilter::xsgetn(char_type* s, std::streamsize count)
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

LZMAInputFilter::int_type LZMAInputFilter::pbackfail(int_type c)
{
    throw AurumIOException((std::string)"Attempted to pushback too many characters " +
                           "into LZMA input stream!");
}

LZMAOutputFilter::LZMAOutputFilter(std::streambuf* chained_buffer,
                                   u32 buffer_size, u32 scratch_buffer_size,
                                   u32 compression_memory, u32 compression_preset_level,
                                   bool compression_use_extreme_preset)
    : LZMAFilterData(scratch_buffer_size),
      SequentialOutputFilterBase(chained_buffer, scratch_buffer_size)
{
    // initialize the stream for compression
    lzma_options_lzma options;
    if (lzma_lzma_preset(&options, (compression_preset_level |
                                    (compression_use_extreme_preset ?
                                     LZMA_PRESET_EXTREME : 0)))) {
        throw AurumIOException((std::string)"Error initializing options for lzma stream!");
    }

    lzma_filter filters[2];
    filters[0].id = LZMA_FILTER_LZMA2;
    filters[0].options = &options;
    filters[1].id = LZMA_VLI_UNKNOWN;
    filters[1].options = nullptr;

    auto status = lzma_stream_encoder(&m_lzma_stream, filters, LZMA_CHECK_CRC64);

    if (status != LZMA_OK) {
        throw AurumIOException((std::string)"Error initializing LZMA output stream!");
    }
}

LZMAOutputFilter::~LZMAOutputFilter()
{
    drain_buffer(false, true);
    lzma_end(&m_lzma_stream);
}

inline void LZMAOutputFilter::drain_buffer(bool sync, bool final_block)
{
    m_lzma_stream.avail_in = pptr() - (char_type*)m_buffer;
    m_lzma_stream.next_in = m_buffer;

    int flush_mode = LZMA_RUN;

    if (final_block) {
        flush_mode = LZMA_FINISH;
    } else if (sync) {
        flush_mode = LZMA_SYNC_FLUSH;
    }

    while (true) {
        m_lzma_stream.next_out = m_scratch_buffer;
        m_lzma_stream.avail_out = m_scratch_buffer_size;

        auto status = lzma_code(&m_lzma_stream, (lzma_action)flush_mode);

        if (flush_mode == LZMA_FINISH || flush_mode == LZMA_SYNC_FLUSH) {
            if (status != LZMA_OK && status != LZMA_STREAM_END) {
                throw AurumIOException((std::string)"Error while compression LZMA stream!");
            }
            auto avail_in_scratch = m_scratch_buffer_size - m_lzma_stream.avail_out;
            m_chained_buffer->sputn((char_type*)m_scratch_buffer, avail_in_scratch);
            if (status == LZMA_STREAM_END) {
                break;
            } else {
                continue;
            }
        } else if (flush_mode == LZMA_RUN) {
            if (status != LZMA_OK && status != LZMA_STREAM_END) {
                throw AurumIOException((std::string)"Error while compression LZMA stream!");
            }
            auto avail_in_scratch = m_scratch_buffer_size - m_lzma_stream.avail_out;
            m_chained_buffer->sputn((char_type*)m_scratch_buffer, avail_in_scratch);
            if (status == LZMA_STREAM_END || m_lzma_stream.avail_in == 0) {
                break;
            } else {
                continue;
            }
        }
    }

    // reset the put area
    setp((char_type*)m_buffer, (char_type*)(m_buffer + m_buffer_size));
}

LZMAOutputFilter::int_type LZMAOutputFilter::overflow(int_type ch)
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

std::streamsize LZMAOutputFilter::xsputn(const char_type* s, std::streamsize n)
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

LZMAOutputFilter::int_type LZMAOutputFilter::sync()
{
    drain_buffer(true, false);
    return m_chained_buffer->pubsync();
}


} /* end namespace io */
} /* end namespace aurum */

#endif /* AURUM_CFG_HAVE_LZMA_ */

//
// LZMAFilter.cpp ends here
