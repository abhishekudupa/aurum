// PluginLoader.hpp ---
// Filename: PluginLoader.hpp
// Author: Abhishek Udupa
// Created: Sun Jul  5 13:26:09 2015 (-0400)
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

#if !defined AURUM_PLUGINS_PLUGIN_LOADER_HPP_
#define AURUM_PLUGINS_PLUGIN_LOADER_HPP_

#include "../basetypes/AurumTypes.hpp"
#include "../basetypes/Downcastable.hpp"

namespace aurum {
namespace plugins {

class PluginBase : public AurumObject<PluginBase>, public Downcastable<PluginBase>
{
private:
    void* m_plugin_handle;

public:
    PluginBase(void* plugin_handle);
    virtual ~PluginBase();

    void* get_plugin_handle() const;
};

class PluginLoader
{
public:
    static PluginBase* load_plugin(const std::string& path_to_plugin,
                                   void* plugin_arg = nullptr,
                                   bool do_lazy_binding = true);

    static void unload_plugin(PluginBase* plugin_ptr);
};

class PluginException : public AurumException
{
public:
    PluginException() noexcept;
    PluginException(const std::string& exception_info) noexcept;
    PluginException(std::string&& exception_info) noexcept;
    PluginException(const PluginException& other) noexcept;
    PluginException(PluginException&& other) noexcept;
    virtual ~PluginException() noexcept;

    PluginException& operator = (const PluginException& other) noexcept;
    PluginException& operator = (PluginException&& other) noexcept;
};

} /* end namespace plugins */
} /* end namespace aurum */

#endif /* AURUM_PLUGINS_PLUGIN_LOADER_HPP_ */

//
// PluginLoader.hpp ends here
