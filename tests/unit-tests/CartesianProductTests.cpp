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
#include <iostream>

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

    auto&& cp_object = ar::make_le_cartesian_product(iterable1, iterable2, iterable3);
    for (auto const& cp_tuple : cp_object) {
        as::Stringifier<typename std::decay<decltype(cp_tuple)>::type> stringifier;
        std::cout << stringifier(cp_tuple) << std::endl;
    }
}

//
// CartesianProductTests.cpp ends here
