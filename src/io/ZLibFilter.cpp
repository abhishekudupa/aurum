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

#include "../allocators/MemoryManager.hpp"

#include "ZLibFilter.hpp"
#include "AurumIOException.hpp"

namespace aurum {
namespace io {

namespace aa = aurum::allocators;

namespace detail_ {

ZLibFilterBase::ZLibFilterBase(bool is_input,
                               u32 chunk_size = sc_default_chunk_size,
                               bool use_gzip_wrapper = false,
                               i32 compression_level = sc_default_compression_level)
    : m_is_input(is_input), m_is_final_block(false),
      m_decompressing_block(false), m_compressing_block(false),
      m_scratch_buffer_size(chunk_size < sc_min_chunk_size ? sc_min_chunk_size : chunk_size),
      m_current_output_buffer_size(0),
      m_scratch_buffer((u08*)aa::allocate_raw(m_scratch_buffer_size)),
      m_current_output_buffer(nullptr)
{
    m_zlib_stream.zalloc = nullptr;
    m_zlib_stream.zfree = nullptr;
    m_zlib_stream.opaque = nullptr;
    int status;

    if (m_is_input) {
        // auto detect window bits from the stream
        status = inflateInit2(&m_zlib_stream, 32);
    } else {
        status = deflateInit2(&m_zlib_stream, compression_level, sc_default_compression_method,
                              sc_default_compression_window_bits + (use_gzip_wrapper ? 16 : 0),
                              sc_default_compression_memlevel, sc_default_compression_strategy);
    }
    if (status != Z_OK) {
        throw AurumIOException((std::string)"Failed to initialize ZLib structures!");
    }
}

ZLibFilterBase::~ZLibFilterBase()
{
    finalize();
}

void ZLibFilterBase::finalize()
{
    if (m_is_input) {
        inflateEnd(&m_zlib_stream);
    } else {
        deflateEnd(&m_zlib_stream);
    }
    m_is_final_block = true;
}

// helpers

inline void ZLibFilterBase::check_input() const
{
    if (!m_is_input) {
        throw AurumIOException((std::string)"Attempted to perform input on a ZLib output filter!");
    }
}

inline void ZLibFilterBase::check_output() const
{
    if (m_is_input) {
        throw AurumIOException((std::string)"Attempted to perform output on a ZLib input filter!");
    }
}

inline void ZLibFilterBase::check_begin_compression() const
{
    check_output();

    if (m_compressing_block) {
        throw AurumIOException((std::string)"Attempted to begin block compression on " +
                               "ZLib output filter, but previous block has not completed " +
                               "compression!");
    }

    if (m_is_final_block) {
        throw AurumIOException((std::string)"Attempted to perform output on a ZLib output " +
                               "filter that has already encoded its final block!");
    }
}

inline void ZLibFilterBase::check_begin_decompression() const
{
    check_input();

    if (m_decompressing_block) {
        throw AurumIOException((std::string)"Attempted to begin block decompression on " +
                               "ZLib input filter, but previous block has not completed " +
                               "decompression!");
    }

    if (m_is_final_block) {
        throw AurumIOException((std::string)"Attempted to perform input on a ZLib input " +
                               "filter that has already decoded its final block!");
    }
}

inline void ZLibFilterBase::check_continue_compression() const
{
    check_output();

    if (!m_compressing_block) {
        throw AurumIOException((std::string)"Attempted to continue compression on ZLib " +
                               "output filter, but no block is currently being compressed!");
    }
}

inline void ZLibFilterBase::check_continue_decompression() const
{
    check_input();

    if (!m_decompressing_block) {
        throw AurumIOException((std::string)"Attempted to continue decompression on ZLib " +
                               "input filter, but no block is currently being decompressed!");
    }
}

u32 ZLibFilterBase::begin_block_compression(u08* input_block, u32 block_size,
                                            bool is_final_block)
{
    check_begin_compression();

    m_compressing_block = true;
    m_is_final_block = is_final_block;

    m_zlib_stream.next_in = input_block;
    m_zlib_stream.avail_in = block_size;

    return continue_block_compression();
}

u32 ZLibFilterBase::continue_block_compression()
{
    check_continue_compression();

    m_zlib_stream.next_out = m_scratch_buffer;
    m_zlib_stream.avail_out = m_scratch_buffer_size;

    auto status = deflate(&m_zlib_stream, m_is_final_block ? Z_FINISH : Z_NO_FLUSH);

    if (status == Z_STREAM_ERROR) {
        throw AurumIOException((std::string)"Error while compressing ZLib stream!");
    }

    auto have_in_scratch_buffer = m_scratch_buffer_size - m_zlib_stream.avail_out;
    if (have_in_scratch_buffer < m_scratch_buffer_size) {
        m_compressing_block = false;
    }

    return have_in_scratch_buffer;
}

u32 ZLibFilterBase::begin_block_decompression(u08* output_block, u32 block_size,
                                              u32 avail_in_scratch_buffer)
{
    check_begin_decompression();

    m_decompressing_block = true;

    m_zlib_stream.next_in = m_scratch_buffer;
    m_zlib_stream.avail_in = avail_in_scratch_buffer;

    m_current_output_buffer = output_block;
    m_current_output_buffer_size = block_size;

    return continue_block_decompression();
}

u32 ZLibFilterBase::continue_block_decompression()
{
    check_continue_decompression();

    m_zlib_stream.next_out = m_current_output_buffer;
    m_zlib_stream.avail_out = m_current_output_buffer_size;

    auto status = inflate(&m_zlib_stream, Z_NO_FLUSH);

    if (status == Z_STREAM_ERROR) {
        throw AurumIOException((std::string)"Error while decompressing ZLib stream!");
    }

    auto have_in_output_buffer = m_current_output_buffer_size - m_zlib_stream.avail_out;
    if (have_in_output_buffer < m_current_output_buffer_size) {
        m_decompressing_block = false;
        m_current_output_buffer = nullptr;
        m_current_output_buffer_size = 0;
    }

    return have_in_output_buffer;
}


} /* end namespace detail_ */

} /* end namespace io */
} /* end namespace aurum */

//
// ZLibFilter.cpp ends here
