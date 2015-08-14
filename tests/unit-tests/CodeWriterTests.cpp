// CodeWriterTests.cpp ---
//
// Filename: CodeWriterTests.cpp
// Author: Abhishek Udupa
// Created: Fri Aug 14 14:13:44 2015 (-0400)
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

#include <iostream>
#include <fstream>
#include <istream>
#include <ostream>

#include <unistd.h>

#include "../../src/io/CodeWriter.hpp"
#include "../../src/io/BZip2Filter.hpp"

#include <gtest/gtest.h>

using aurum::u32;
using aurum::u64;
using aurum::i32;
using aurum::i64;
using aurum::u08;
using aurum::i08;

namespace aio = aurum::io;

namespace code_writer_tests {

static constexpr const char* file_name = "code_writer_test.cpp";
static constexpr const char* bzip_file_name = "code_writer_test.bz2";

static inline void make_test_file(bool use_compression)
{
    aio::CodeWriter fstr(use_compression ? bzip_file_name : file_name);
    if (use_compression) {
        fstr.push<aio::BZip2Filter>();
    }

    fstr << "int main(int argc, char* argv)" << std::endl;
    fstr << "{" << std::endl;
    fstr.increment_indent();
    fstr << "std::cout << \"Hello World!\" << std::endl;" << std::endl;
    fstr.decrement_indent();
    fstr << "}" << std::endl;
}

} /* end namespace code_writer_tests */

TEST(CodeWriterTests, Basic)
{
    code_writer_tests::make_test_file(false);
    code_writer_tests::make_test_file(true);

    char temp_buffer[2048];

    std::ifstream cpp_str(code_writer_tests::file_name);
    cpp_str.read(temp_buffer, 2047);

    temp_buffer[cpp_str.gcount()] = '\0';

    EXPECT_EQ("int main(int argc, char* argv)\n{\n    std::cout << \"Hello World!\" << std::endl;\n}\n", std::string(temp_buffer));

    aio::FilteredIStream bz2_stream(code_writer_tests::bzip_file_name);
    bz2_stream.push<aio::BZip2Filter>();

    bz2_stream.read(temp_buffer, 2047);
    temp_buffer[bz2_stream.gcount()] = '\0';
    EXPECT_EQ("int main(int argc, char* argv)\n{\n    std::cout << \"Hello World!\" << std::endl;\n}\n", std::string(temp_buffer));

    unlink(code_writer_tests::file_name);
    unlink(code_writer_tests::bzip_file_name);
}


//
// CodeWriterTests.cpp ends here
