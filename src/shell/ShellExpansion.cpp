// ShellExpansion.cpp ---
//
// Filename: ShellExpansion.cpp
// Author: Abhishek Udupa
// Created: Sun Jun 21 14:57:03 2015 (-0400)
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

#include <unistd.h>
#include <sstream>
#include <iostream>

#include "../strutils/StrUtils.hpp"
#include "../allocators/MemoryManager.hpp"

#include "ShellExpansion.hpp"

namespace aurum {
namespace shell {

namespace ac = aurum::containers;
namespace aa = aurum::allocators;

std::string apply_shell_expansion(const std::string& the_string,
                                  const std::string& shell_command)
{
    int pipefds[2];

    auto const pipe_status = pipe(pipefds);

    if (pipe_status != 0) {
        throw AurumException((std::string)"Error: could not open pipe for shell expansion");
    }

    int read_fd = pipefds[0];
    int write_fd = pipefds[1];

    auto const fork_status = fork();

    if (fork_status == 0) {
        // code for child process.
        // we execute the shell and ask it to echo the argument string
        // but first redirect the output to the pipe

        write_fd = dup2(write_fd, STDOUT_FILENO);
        close(read_fd);

        if (write_fd < 0) {
            throw AurumException((std::string)"dup2() system called failed in shelll expansion");
        }

        std::ostringstream sstr;
        sstr << "/bin/echo " << the_string;
        auto&& echo_string = sstr.str();

        char* exec_args[4];

        exec_args[0] = aa::casted_allocate_raw_cleared<char>(shell_command.length() + 1);
        memcpy(exec_args[0], shell_command.c_str(), shell_command.length());

        exec_args[1] = aa::casted_allocate_raw_cleared<char>(3);
        exec_args[1][0] = '-';
        exec_args[1][1] = 'c';
        exec_args[1][2] = 0;

        exec_args[2] = aa::casted_allocate_raw_cleared<char>(echo_string.length() + 1);
        memcpy(exec_args[2], echo_string.c_str(), echo_string.length());

        exec_args[3] = nullptr;

        execv(shell_command.c_str(), exec_args);

    } else {
        // in the parent, just read from the pipe
        close(write_fd);
        std::ostringstream sstr;
        const u64 read_block_size = 1024;
        char read_buffer[read_block_size];

        u64 bytes_read;
        while ((bytes_read = read(read_fd, read_buffer, read_block_size - 1)) > 0) {
            read_buffer[bytes_read] = 0;
            sstr << read_buffer;
        }

        close(read_fd);

        return strutils::trim_copy(sstr.str());
    }

    // to avoid compiler warnings
    return "";
}

std::string apply_shell_expansion(const ac::Vector<std::string>& arg_strings,
                                  const std::string& shell_command)
{
    std::ostringstream sstr;
    const u64 num_strings = arg_strings.size();

    for (u64 i = 0; i < num_strings; ++i) {
        sstr << arg_strings[i];
        if (i != num_strings - 1) {
            sstr << " ";
        }
    }
    return apply_shell_expansion(sstr.str(), shell_command);
}

} /* end namespace shell */
} /* end namespace aurum */

//
// ShellExpansion.cpp ends here
