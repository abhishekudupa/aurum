// CartesianProductTests.cpp ---
//
// Filename: CartesianProductTests.cpp
// Author: Abhishek Udupa
// Created: Tue Jul  7 19:09:34 2015 (-0400)
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

#include <string>
#include <sstream>

#include "../../src/containers/Vector.hpp"
#include "../../src/containers/DList.hpp"
#include "../../src/ranges/CartesianProduct.hpp"
#include "../../src/stringification/Stringifiers.hpp"

#include <gtest/gtest.h>

using aurum::u32;
using aurum::u64;
namespace ar = aurum::ranges;
namespace ac = aurum::containers;
namespace as = aurum::stringification;

using testing::Types;

TEST(CartesianProductTest, Functional)
{
    ac::Vector<u64> iterable1 = {1, 2, 3, 4, 5};
    ac::Vector<std::string> iterable2 = {"one", "two", "three", "four"};
    ac::DList<float> iterable3 = {1.1f, 2.2f, 3.3f, 4.4f, 5.5f, 6.6f};

    std::ostringstream sstr;

    auto&& cp_object_le = ar::make_le_cartesian_product(iterable1, iterable2, iterable3);
    for (auto const& cp_tuple : cp_object_le) {
        as::Stringifier<typename std::decay<decltype(cp_tuple)>::type> stringifier;
        sstr << stringifier(cp_tuple) << std::endl;
    }

    EXPECT_EQ("<1, one, 1.100000>\n<2, one, 1.100000>\n<3, one, 1.100000>\n<4, one, 1.100000>\n<5, one, 1.100000>\n<1, two, 1.100000>\n<2, two, 1.100000>\n<3, two, 1.100000>\n<4, two, 1.100000>\n<5, two, 1.100000>\n<1, three, 1.100000>\n<2, three, 1.100000>\n<3, three, 1.100000>\n<4, three, 1.100000>\n<5, three, 1.100000>\n<1, four, 1.100000>\n<2, four, 1.100000>\n<3, four, 1.100000>\n<4, four, 1.100000>\n<5, four, 1.100000>\n<1, one, 2.200000>\n<2, one, 2.200000>\n<3, one, 2.200000>\n<4, one, 2.200000>\n<5, one, 2.200000>\n<1, two, 2.200000>\n<2, two, 2.200000>\n<3, two, 2.200000>\n<4, two, 2.200000>\n<5, two, 2.200000>\n<1, three, 2.200000>\n<2, three, 2.200000>\n<3, three, 2.200000>\n<4, three, 2.200000>\n<5, three, 2.200000>\n<1, four, 2.200000>\n<2, four, 2.200000>\n<3, four, 2.200000>\n<4, four, 2.200000>\n<5, four, 2.200000>\n<1, one, 3.300000>\n<2, one, 3.300000>\n<3, one, 3.300000>\n<4, one, 3.300000>\n<5, one, 3.300000>\n<1, two, 3.300000>\n<2, two, 3.300000>\n<3, two, 3.300000>\n<4, two, 3.300000>\n<5, two, 3.300000>\n<1, three, 3.300000>\n<2, three, 3.300000>\n<3, three, 3.300000>\n<4, three, 3.300000>\n<5, three, 3.300000>\n<1, four, 3.300000>\n<2, four, 3.300000>\n<3, four, 3.300000>\n<4, four, 3.300000>\n<5, four, 3.300000>\n<1, one, 4.400000>\n<2, one, 4.400000>\n<3, one, 4.400000>\n<4, one, 4.400000>\n<5, one, 4.400000>\n<1, two, 4.400000>\n<2, two, 4.400000>\n<3, two, 4.400000>\n<4, two, 4.400000>\n<5, two, 4.400000>\n<1, three, 4.400000>\n<2, three, 4.400000>\n<3, three, 4.400000>\n<4, three, 4.400000>\n<5, three, 4.400000>\n<1, four, 4.400000>\n<2, four, 4.400000>\n<3, four, 4.400000>\n<4, four, 4.400000>\n<5, four, 4.400000>\n<1, one, 5.500000>\n<2, one, 5.500000>\n<3, one, 5.500000>\n<4, one, 5.500000>\n<5, one, 5.500000>\n<1, two, 5.500000>\n<2, two, 5.500000>\n<3, two, 5.500000>\n<4, two, 5.500000>\n<5, two, 5.500000>\n<1, three, 5.500000>\n<2, three, 5.500000>\n<3, three, 5.500000>\n<4, three, 5.500000>\n<5, three, 5.500000>\n<1, four, 5.500000>\n<2, four, 5.500000>\n<3, four, 5.500000>\n<4, four, 5.500000>\n<5, four, 5.500000>\n<1, one, 6.600000>\n<2, one, 6.600000>\n<3, one, 6.600000>\n<4, one, 6.600000>\n<5, one, 6.600000>\n<1, two, 6.600000>\n<2, two, 6.600000>\n<3, two, 6.600000>\n<4, two, 6.600000>\n<5, two, 6.600000>\n<1, three, 6.600000>\n<2, three, 6.600000>\n<3, three, 6.600000>\n<4, three, 6.600000>\n<5, three, 6.600000>\n<1, four, 6.600000>\n<2, four, 6.600000>\n<3, four, 6.600000>\n<4, four, 6.600000>\n<5, four, 6.600000>\n", sstr.str());

    sstr.str("");

    auto cp_object_be = ar::make_be_cartesian_product(iterable1, iterable2, iterable3);
    for (auto const& cp_tuple : cp_object_be) {
        as::Stringifier<typename std::decay<decltype(cp_tuple)>::type> stringifier;
        sstr << stringifier(cp_tuple) << std::endl;
    }

    EXPECT_EQ("<1, one, 1.100000>\n<1, one, 2.200000>\n<1, one, 3.300000>\n<1, one, 4.400000>\n<1, one, 5.500000>\n<1, one, 6.600000>\n<1, two, 1.100000>\n<1, two, 2.200000>\n<1, two, 3.300000>\n<1, two, 4.400000>\n<1, two, 5.500000>\n<1, two, 6.600000>\n<1, three, 1.100000>\n<1, three, 2.200000>\n<1, three, 3.300000>\n<1, three, 4.400000>\n<1, three, 5.500000>\n<1, three, 6.600000>\n<1, four, 1.100000>\n<1, four, 2.200000>\n<1, four, 3.300000>\n<1, four, 4.400000>\n<1, four, 5.500000>\n<1, four, 6.600000>\n<2, one, 1.100000>\n<2, one, 2.200000>\n<2, one, 3.300000>\n<2, one, 4.400000>\n<2, one, 5.500000>\n<2, one, 6.600000>\n<2, two, 1.100000>\n<2, two, 2.200000>\n<2, two, 3.300000>\n<2, two, 4.400000>\n<2, two, 5.500000>\n<2, two, 6.600000>\n<2, three, 1.100000>\n<2, three, 2.200000>\n<2, three, 3.300000>\n<2, three, 4.400000>\n<2, three, 5.500000>\n<2, three, 6.600000>\n<2, four, 1.100000>\n<2, four, 2.200000>\n<2, four, 3.300000>\n<2, four, 4.400000>\n<2, four, 5.500000>\n<2, four, 6.600000>\n<3, one, 1.100000>\n<3, one, 2.200000>\n<3, one, 3.300000>\n<3, one, 4.400000>\n<3, one, 5.500000>\n<3, one, 6.600000>\n<3, two, 1.100000>\n<3, two, 2.200000>\n<3, two, 3.300000>\n<3, two, 4.400000>\n<3, two, 5.500000>\n<3, two, 6.600000>\n<3, three, 1.100000>\n<3, three, 2.200000>\n<3, three, 3.300000>\n<3, three, 4.400000>\n<3, three, 5.500000>\n<3, three, 6.600000>\n<3, four, 1.100000>\n<3, four, 2.200000>\n<3, four, 3.300000>\n<3, four, 4.400000>\n<3, four, 5.500000>\n<3, four, 6.600000>\n<4, one, 1.100000>\n<4, one, 2.200000>\n<4, one, 3.300000>\n<4, one, 4.400000>\n<4, one, 5.500000>\n<4, one, 6.600000>\n<4, two, 1.100000>\n<4, two, 2.200000>\n<4, two, 3.300000>\n<4, two, 4.400000>\n<4, two, 5.500000>\n<4, two, 6.600000>\n<4, three, 1.100000>\n<4, three, 2.200000>\n<4, three, 3.300000>\n<4, three, 4.400000>\n<4, three, 5.500000>\n<4, three, 6.600000>\n<4, four, 1.100000>\n<4, four, 2.200000>\n<4, four, 3.300000>\n<4, four, 4.400000>\n<4, four, 5.500000>\n<4, four, 6.600000>\n<5, one, 1.100000>\n<5, one, 2.200000>\n<5, one, 3.300000>\n<5, one, 4.400000>\n<5, one, 5.500000>\n<5, one, 6.600000>\n<5, two, 1.100000>\n<5, two, 2.200000>\n<5, two, 3.300000>\n<5, two, 4.400000>\n<5, two, 5.500000>\n<5, two, 6.600000>\n<5, three, 1.100000>\n<5, three, 2.200000>\n<5, three, 3.300000>\n<5, three, 4.400000>\n<5, three, 5.500000>\n<5, three, 6.600000>\n<5, four, 1.100000>\n<5, four, 2.200000>\n<5, four, 3.300000>\n<5, four, 4.400000>\n<5, four, 5.500000>\n<5, four, 6.600000>\n", sstr.str());

    sstr.str("");

    ac::Vector<double> iterable4;

    auto&& cp_object_empty = ar::make_le_cartesian_product(iterable1, iterable2, iterable3, iterable4);
    EXPECT_EQ(cp_object_empty.end(), cp_object_empty.begin());
}

//
// CartesianProductTests.cpp ends here
