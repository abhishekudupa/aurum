// ShellUtils.cpp ---
//
// Filename: ShellUtils.cpp
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
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "../strutils/StrUtils.hpp"
#include "../allocators/MemoryManager.hpp"
#include "../basetypes/AurumErrors.hpp"

#include "ShellUtils.hpp"

namespace aurum {
namespace shell {

namespace ac = aurum::containers;
namespace aa = aurum::allocators;

std::string apply_shell_expansion(const std::string& the_string,
                                  const std::string& shell_command)
{
    ac::Vector<std::string> command_args(3);
    command_args[0] = shell_command;
    command_args[1] = "-c";
    command_args[2] = "echo " + the_string;
    auto&& out_err = execute_command(command_args);
    return strutils::trim_copy(out_err.first);
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

std::pair<std::string, std::string>
execute_command(const ac::Vector<std::string>& command_line)
{
    int stdout_fds[2];
    int stderr_fds[2];

    const u64 num_args = command_line.size();

    auto pipe_status = pipe(stdout_fds);

    if (pipe_status < 0) {
        throw AurumException((std::string)"Error: could not open pipe for execute_command()");
    }

    pipe_status = pipe(stderr_fds);

    if (pipe_status < 0) {
        throw AurumException((std::string)"Error: could not open pipe for execute_command()");
    }

    int stdout_read_fd = stdout_fds[0];
    int stdout_write_fd = stdout_fds[1];
    int stderr_read_fd = stderr_fds[0];
    int stderr_write_fd = stderr_fds[1];

    auto const child_pid = fork();

    if (child_pid == 0) {
        // what the child executes

        close(stdout_read_fd);
        close(stderr_read_fd);

        stdout_write_fd = dup2(stdout_write_fd, STDOUT_FILENO);
        stderr_write_fd = dup2(stderr_write_fd, STDERR_FILENO);

        if (stdout_write_fd < 0 || stderr_write_fd < 0) {
            throw AurumException((std::string)"dup2() system called failed in execute_command()");
        }

        char** exec_args;
        exec_args = aa::casted_allocate_raw_cleared<char*>(sizeof(char*) * (num_args + 1));

        for (u64 i = 0; i < num_args; ++i) {
            exec_args[i] = aa::casted_allocate_raw_cleared<char>(command_line[i].length() + 1);
            memcpy(exec_args[i], command_line[i].c_str(), command_line[i].length());
        }

        exec_args[num_args] = nullptr;
        execv(exec_args[0], exec_args);

    } else {
        // what the parent executes
        close(stdout_write_fd);
        close(stderr_write_fd);
        const u64 read_block_size = 1024;
        char read_buffer[read_block_size];

        std::ostringstream errstr;
        std::ostringstream outstr;

        fd_set select_fds;

        FD_ZERO(&select_fds);
        FD_SET(stdout_read_fd, &select_fds);
        FD_SET(stderr_read_fd, &select_fds);

        bool out_closed = false;
        bool err_closed = false;

        while (!out_closed || !err_closed) {
            int maxfd = 0;
            if (!out_closed && !err_closed) {
                maxfd = std::max(stdout_read_fd, stderr_read_fd) + 1;
            } else if (!out_closed) {
                maxfd = stdout_read_fd + 1;
            } else {
                maxfd = stderr_read_fd + 1;
            }

            auto const select_status = select(maxfd, &select_fds, nullptr, nullptr, nullptr);

            if (select_status < 0) {
                throw AurumException((std::string)"select() system call failed in execute_command()");
            }

            bool out_has_data = FD_ISSET(stdout_read_fd, &select_fds);
            bool err_has_data = FD_ISSET(stderr_read_fd, &select_fds);

            if (out_has_data) {
                auto const bytes_read = read(stdout_read_fd, read_buffer, read_block_size - 1);

                if (bytes_read < 0) {
                    throw AurumException((std::string)"read() system call failed in execute_command()");
                }

                read_buffer[bytes_read] = 0;
                outstr << read_buffer;

                if (bytes_read == 0) {
                    out_closed = true;
                }
            }

            if (err_has_data) {
                auto const bytes_read = read(stderr_read_fd, read_buffer, read_block_size - 1);

                if (bytes_read < 0) {
                    throw AurumException((std::string)"read() system call failed in execute_command()");
                }

                read_buffer[bytes_read] = 0;
                errstr << read_buffer;

                if (bytes_read == 0) {
                    err_closed = true;
                }
            }

            if (!out_closed) {
                FD_SET(stdout_read_fd, &select_fds);
            } else {
                FD_CLR(stdout_read_fd, &select_fds);
            }

            if (!err_closed) {
                FD_SET(stderr_read_fd, &select_fds);
            } else {
                FD_CLR(stderr_read_fd, &select_fds);
            }
        }

        close(stdout_read_fd);
        close(stderr_read_fd);

        int child_status;
        waitpid(child_pid, &child_status, 0);

        if (child_status != 0) {
            throw AurumException((std::string)"Subprocess exited with non-zero exit status: " +
                                 std::to_string(child_status) + " in call to execute_command()");
        }

        return std::make_pair(outstr.str(), errstr.str());
    }


    // unreachable code, but get the compiler to not complain
    return std::make_pair("", "");
}

} /* end namespace shell */
} /* end namespace aurum */

//
// ShellUtils.cpp ends here
