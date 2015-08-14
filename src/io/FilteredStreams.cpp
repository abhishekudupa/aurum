// FilteredStreams.cpp ---
// Filename: FilteredStreams.cpp
// Author: Abhishek Udupa
// Created: Mon Aug 10 21:25:13 2015 (-0400)
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

#include "FilteredStreams.hpp"
#include "AurumIOException.hpp"

namespace aurum {
namespace io {

FilteredIStream::FilteredIStream(std::streambuf* stream_buffer)
    : FilteredInputStreamBase(stream_buffer)
{
    // Nothing here
}

FilteredIStream::FilteredIStream(const char* filename)
    : FilteredInputStreamBase(new std::filebuf())
{
    auto as_file_buf = static_cast<std::filebuf*>(m_chain_root);
    m_chain_root_owned = true;
    auto stat = as_file_buf->open(filename, ios_base::in);

    if (stat == nullptr) {
        throw AurumIOException((std::string)"Error opening file: " + filename +
                               " in read mode!");
    }
    return;
}

FilteredIStream::FilteredIStream(const std::string& filename)
    : FilteredIStream(filename.c_str())
{
    // Nothing here
}

FilteredIStream::~FilteredIStream()
{
    // Nothing here
}

FilteredOStream::FilteredOStream(std::streambuf* stream_buffer)
    : FilteredOutputStreamBase(stream_buffer)
{
    // Nothing here
}

FilteredOStream::FilteredOStream(const char* filename, bool append)
    : FilteredOutputStreamBase(new std::filebuf())
{
    auto as_file_buf = static_cast<std::filebuf*>(m_chain_root);
    m_chain_root_owned = true;

    auto mode = ios_base::out;
    if (!append) {
        mode = mode | ios_base::trunc;
    }

    auto stat = as_file_buf->open(filename, mode);

    if (stat == nullptr) {
        throw AurumIOException((std::string)"Error opening file: " + filename +
                               " in read mode!");
    }
    return;
}

FilteredOStream::FilteredOStream(const std::string& filename, bool append)
    : FilteredOStream(filename.c_str(), append)
{
    // Nothing here
}

FilteredOStream::~FilteredOStream()
{
    // Nothing here
}

} /* end namespace io */
} /* end namespace aurum */

//
// FilteredStreams.cpp ends here
