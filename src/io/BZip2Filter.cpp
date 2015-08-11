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

namespace aurum {
namespace io {

namespace aa = aurum::allocators;

namespace detail_ {

BZip2FilterBase::BZip2FilterBase(bool is_input,
                                 u32 chunk_size,
                                 u32 compression_work_factor,
                                 u32 compression_block_size)
    : m_is_input(is_input), m_is_final_block(false),
      m_decompressing_block(false), m_compressing_block(false),
      m_compressing_block_in_sync_mode(false),
      m_scratch_buffer_size(chunk_size > sc_min_chunk_size ? chunk_size : sc_min_chunk_size),
      m_current_output_buffer_size(0),
      m_scratch_buffer((u08*)aa::allocate_raw(m_scratch_buffer_size)),
      m_current_output_buffer(nullptr)
{
    m_bzip_stream.bzalloc = aa::allocate_fun_for_compressioni32;
    m_bzip_stream.bzfree = aa::deallocate_fun_for_compression;
    m_bzip_stream.opaque = nullptr;

    i32 status;
    if (m_is_input) {
        status = BZ2_bzDecompressInit(&m_bzip_stream, 0, 0);
    } else {
        status = BZ2_bzCompressInit(&m_bzip_stream, compression_block_size, 0,
                                    compression_work_factor);
    }

    if (status != BZ_OK) {
        throw AurumIOException((std::string)"Error initializing BZip2 data structures!");
    }
}

BZip2FilterBase::~BZip2FilterBase()
{
    finalize();
}

void BZip2FilterBase::finalize()
{
    if (m_is_input) {
        BZ2_bzDecompressEnd(&m_bzip_stream);
    } else {
        BZ2_bzCompressEnd(&m_bzip_stream);
    }
    m_is_final_block = true;
}

// helpers
inline void BZip2FilterBase::check_input() const
{
    if (!m_is_input) {
        throw AurumIOException((std::string)"Attempted to perform input on a BZip2 output filter!");
    }
}

inline void BZip2FilterBase::check_output() const
{
    if (m_is_input) {
        throw AurumIOException((std::string)"Attempted to perform output on a BZip2 input filter!");
    }
}

inline void BZip2FilterBase::check_begin_compression() const
{
    check_output();

    if (m_compressing_block) {
        throw AurumIOException((std::string)"Attempted to begin block compression on " +
                               "BZip2 output filter, but previous block has not completed " +
                               "compression!");
    }
    if (m_is_final_block) {
        throw AurumIOException((std::string)"Attempted to perform output on a BZip2 output " +
                               "filter that has already encoded its final block!");
    }
}

inline void BZip2FilterBase::check_begin_decompression() const
{
    check_input();

    if (m_decompressing_block) {
        throw AurumIOException((std::string)"Attempted to begin block decompression on " +
                               "BZip2 input filter, but previous block has not completed " +
                               "decompression!");
    }

    if (m_is_final_block) {
        throw AurumIOException((std::string)"Attempted to perform input on a BZip2 input " +
                               "filter that has already decoded its final block!");
    }
}

inline void BZip2FilterBase::check_continue_compression() const
{
    check_output();

    if (!m_compressing_block) {
        throw AurumIOException((std::string)"Attempted to continue compression on BZip2 " +
                               "output filter, but no block is currently being compressed!");
    }
}

inline void BZip2FilterBase::check_continue_decompression() const
{
    check_input();

    if (!m_decompressing_block) {
        throw AurumIOException((std::string)"Attempted to continue decompression on BZip2 " +
                               "input filter, but no block is currently being decompressed!");
    }
}

u64 BZip2FilterBase::begin_block_compression(u08 *input_block, u64 block_size,
                                             bool do_sync, bool is_final_block)
{
    check_begin_compression();

    m_compressing_block = true;
    m_is_final_block = is_final_block;
    m_compressing_block_in_sync_mode = do_sync;

    m_bzip_stream.next_in = (char*)input_block;
    m_bzip_stream.avail_in = block_size;

    return continue_block_compression();
}

u64 BZip2FilterBase::continue_block_compression()
{
    check_continue_compression();

    m_bzip_stream.next_out = (char*)m_scratch_buffer;
    m_bzip_stream.avail_out = m_scratch_buffer_size;

    i32 action = BZ_RUN;

    if (m_is_final_block) {
        action = BZ_FINISH;
    } else if (m_compressing_block_in_sync_mode) {
        action = BZ_FLUSH;
    }

    auto status = BZ2_bzCompress(&m_bzip_stream, action);

    if (m_compressing_block_in_sync_mode) {
        if (status == BZ_RUN_OK) {
            m_compressing_block_in_sync_mode = false;
            m_compressing_block = false;
        } else if (status != BZ_FLUSH_OK) {
            throw AurumIOException((std::string)"Error while compressing BZip2 stream!");
        }
    } else if (m_is_final_block) {
        if (status == BZ_STREAM_END) {
            m_compressing_block = false;
        } else if (status != BZ_FINISH_OK) {
            throw AurumIOException((std::string)"Error while compressing BZip2 stream!");
        }
    } else {
        if (status == BZ_RUN_OK) {
            if (m_bzip_stream.avail_out > 0) {
                m_compressing_block = false;
            }
        } else {
            throw AurumIOException((std::string)"Error while compressing BZip2 stream!");
        }
    }

    auto have_in_scratch_buffer = m_scratch_buffer_size - m_bzip_stream.avail_out;
    return have_in_scratch_buffer;
}

u64 BZip2FilterBase::begin_block_decompression(u08* output_block, u32 block_size,
                                               u32 avail_in_scratch_buffer)
{
    check_begin_decompression();

    m_decompressing_block = true;

    m_bzip_stream.next_in = (char*)m_scratch_buffer;
    m_bzip_stream.avail_in = avail_in_scratch_buffer;

    m_current_output_buffer = output_block;
    m_current_output_buffer_size = block_size;

    return continue_block_decompression();
}

u64 BZip2FilterBase::continue_block_decompression()
{
    check_continue_decompression();

    m_bzip_stream.next_out = (char*)m_current_output_buffer;
    m_bzip_stream.avail_out = m_current_output_buffer_size;

    auto status = BZ2_bzDecompress(&m_bzip_stream);

    if (status == BZ_STREAM_END) {
        m_decompressing_block = false;
        m_is_final_block = true;
    } else if (status == BZ_OK) {
        if (m_bzip_stream.avail_in == 0) {
            m_decompressing_block = false;
            m_current_output_buffer = nullptr;
            m_current_output_buffer_size = 0;
        }
    } else {
        throw AurumIOException((std::string)"Error while decompressing BZip2 stream!");
    }

    auto have_in_output_buffer = m_bzip_stream.avail_out;
    return have_in_output_buffer;
}

} /* end namespace detail_ */

// implementation of BZip2InputFilter
BZip2InputFilter::BZip2InputFilter(std::streambuf* chained_buffer, u64 buffer_size)
    : BZip2FilterBase(true), SequentialInputFilterBase(chained_buffer, buffer_size)
{
    // Nothing here
}

BZip2InputFilter::~BZip2InputFilter()
{
    // Nothing here
}

BZip2InputFilter::int_type BZip2InputFilter::underflow()
{
    if (gptr() < egptr()) {
        return (*(gptr()));
    }

    // are we finished with the stream?
    if (m_is_final_block && !m_decompressing_block) {
        setg((char_type*)m_buffer, (char_type*)m_buffer, (char_type*)m_buffer);
        return traits_type::eof();
    }

    u64 num_available = 0;
    // gptr >= egptr(), so we need to refill the buffer
    if (m_decompressing_block) {
        num_available = continue_block_decompression();
    } else {
        auto const avail_in_scratch_buffer =
            m_chained_buffer->sgetn((char_type*)m_scratch_buffer, m_scratch_buffer_size);
        num_available = begin_block_decompression(m_buffer, m_buffer_size,
                                                  avail_in_scratch_buffer);
    }

    setg((char_type*)m_buffer, (char_type*)m_buffer, (char_type*)(m_buffer + num_available));
    if (num_available == 0) {
        return traits_type::eof();
    } else {
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
                                     u64 buffer_size, u32 chunk_size,
                                     u32 compression_work_factor,
                                     u32 compression_block_size)
    : BZip2FilterBase(false, chunk_size, compression_work_factor, compression_block_size),
      SequentialOutputFilterBase(chained_buffer, buffer_size)
{
    // Nothing here
}

BZip2OutputFilter::~BZip2OutputFilter()
{
    if (!m_is_final_block) {
        finalize();
    }
}

inline void BZip2OutputFilter::do_overflow(bool sync_compression, bool final_block)
{
    auto available_in_buffer = pptr() - (char_type*)m_buffer;
    auto available_in_scratch = begin_block_compression(m_buffer, available_in_buffer,
                                                        sync_compression, final_block);

    while (available_in_scratch == m_scratch_buffer_size) {
        m_chained_buffer->sputn((char_type*)m_scratch_buffer, available_in_scratch);
        available_in_scratch = continue_block_compression();
    }

    m_chained_buffer->sputn((char_type*)m_scratch_buffer, available_in_scratch);

    setp((char_type*)m_buffer, (char_type*)(m_buffer + m_buffer_size));
}

BZip2OutputFilter::int_type BZip2OutputFilter::overflow(int_type ch)
{

    // has the stream already been finished?
    if (m_is_final_block) {
        setp((char_type*)m_buffer, (char_type*)m_buffer);
        return traits_type::eof();
    }

    if (pptr() < epptr()) {
        if (!traits_type::eq_int_type(ch, traits_type::eof())) {
            sputc((char_type)ch);
            return ch;
        } else {
            return 0;
        }
    }

    do_overflow();

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

void BZip2OutputFilter::finalize()
{
    do_overflow(false, true);
    BZip2FilterBase::finalize();
}

BZip2OutputFilter::int_type BZip2OutputFilter::sync()
{
    do_overflow(true, false);
    return m_chained_buffer->pubsync();
}

} /* end namespace io */
} /* end namespace aurum */

//
// BZip2Filter.cpp ends here
