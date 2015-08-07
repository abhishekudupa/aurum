// AurumStreamBuffer.hpp ---
//
// Filename: AurumStreamBuffer.hpp
// Author: Abhishek Udupa
// Created: Thu Aug  6 17:53:10 2015 (-0400)
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

#if !defined AURUM_IO_AURUM_STREAM_BUFFER_HPP_
#define AURUM_IO_AURUM_STREAM_BUFFER_HPP_

#include <streambuf>

#include "../basetypes/AurumTypes.hpp"

namespace aurum {
namespace io {

namespace detail_ {

class AurumStreamBufferBase : public AurumObject<AurumStreamBufferBase>,
                              public std::streambuf
{
private:
    typedef std::streambuf BaseType;

public:
    using BaseType::BaseType;
    virtual ~AurumStreamBufferBase();

    // everything else is as expected of a stream buffer
};

} /* end namespace detail_ */

class AurumInputStream : public AurumStreamBufferBase
{
public:

};

} /* end namespace io */
} /* end namespace aurum */

#endif /* AURUM_IO_AURUM_STREAM_BUFFER_HPP_ */

//
// AurumStreamBuffer.hpp ends here
