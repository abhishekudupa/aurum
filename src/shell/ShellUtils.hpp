// ShellUtils.hpp ---
//
// Filename: ShellUtils.hpp
// Author: Abhishek Udupa
// Created: Sun Jun 21 14:53:14 2015 (-0400)
//
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

#if !defined AURUM_SHELL_SHELL_UTILS_HPP_
#define AURUM_SHELL_SHELL_UTILS_HPP_

#include "../basetypes/AurumTypes.hpp"
#include "../containers/Vector.hpp"

namespace aurum {
namespace shell {

namespace ac = aurum::containers;

extern std::string apply_shell_expansion(const std::string& the_string,
                                         const std::string& shell_command = "/bin/sh");

extern std::string apply_shell_expansion(const ac::Vector<std::string>& arg_strings,
                                         const std::string& shell_command = "/bin/sh");

// execute a command and obtain its stdout and stderr
// the first element of the returned value is the stdout
// the second element is the stderr
extern std::pair<std::string, std::string>
execute_command(const ac::Vector<std::string>& command_line);

} /* end namespace shell */
} /* end namespace aurum */

#endif /* AURUM_SHELL_SHELL_UTILS_HPP_ */

//
// ShellUtils.hpp ends here
