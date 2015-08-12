// ZLibFilterTests.cpp ---
//
// Filename: ZLibFilterTests.cpp
// Author: Abhishek Udupa
// Created: Mon Aug 10 12:17:22 2015 (-0400)
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

#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <unistd.h>
#include <AurumConfig.h>

#ifdef AURUM_CFG_HAVE_LZMA_

#include "../../src/io/LZMAFilter.hpp"

#include <gtest/gtest.h>

using aurum::u32;
using aurum::u64;
using aurum::i32;
using aurum::i64;
using aurum::u08;

namespace lzma_testing {

static const char* test_file_name = "lzma_test.xz";

static constexpr u32 max_test_string_len = 128;
static constexpr u32 max_num_strings = 65536;

std::default_random_engine random_generator;
std::uniform_int_distribution<u08> alpha_distribution(0, 25);
std::uniform_int_distribution<u32> strlen_distribution(1, max_test_string_len - 1);

static inline std::string generate_random_string()
{
    char temp_buffer[max_test_string_len];
    auto str_len = strlen_distribution(random_generator);
    for (u32 i = 0; i < str_len; ++i) {
        temp_buffer[i]  = 'a' + alpha_distribution(random_generator);
    }
    temp_buffer[str_len] = '\0';
    return std::string(temp_buffer);
}

static inline void make_test_file(std::ostringstream& sstr)
{
    std::filebuf fbuf;
    fbuf.open(test_file_name, std::ios_base::out | std::ios_base::trunc);
    aurum::io::LZMAOutputFilter lzma_filter(&fbuf);
    std::ostream fstr(&lzma_filter);

    for (u32 i = 0; i < max_num_strings; ++i) {
        auto&& cur_string = generate_random_string();
        sstr << cur_string << "\n";
        fstr << cur_string << "\n";
    }

    // let the destruction sequence do its thing
}

static inline void read_test_file(std::ostringstream& sstr)
{
    std::filebuf fbuf;
    fbuf.open(test_file_name, std::ios_base::in);
    aurum::io::LZMAInputFilter lzma_filter(&fbuf);
    std::istream fstr(&lzma_filter);
    char temp_buffer[1025];

    while(!fstr.eof()) {
        fstr.read(temp_buffer, 1024);
        temp_buffer[fstr.gcount()] = '\0';
        sstr << temp_buffer;
    }

    // let the destruction sequence do its thing
}

} /* end namespace bzip2_testing */

TEST(LZMAFilter, Functional)
{
    std::ostringstream sstr;
    std::ostringstream cmpstr;
    lzma_testing::make_test_file(sstr);
    lzma_testing::read_test_file(cmpstr);

    EXPECT_EQ(sstr.str(), cmpstr.str());

    unlink(lzma_testing::test_file_name);
}

#endif /* AURUM_CFG_HAVE_BZIP2_ */

//
// ZLibFilterTests.cpp ends here
