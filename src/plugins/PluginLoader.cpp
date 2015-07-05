// PluginLoader.cpp ---
// Filename: PluginLoader.cpp
// Author: Abhishek Udupa
// Created: Sun Jul  5 13:33:24 2015 (-0400)
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

#include <dlfcn.h>

#include "PluginLoader.hpp"

namespace aurum {
namespace plugins {

PluginBase::PluginBase(void* plugin_handle)
    : m_plugin_handle(plugin_handle)
{
    // Nothing here
}

PluginBase::~PluginBase()
{
    // Nothing here
}

void* PluginBase::get_plugin_handle() const
{
    return m_plugin_handle;
}

PluginBase* PluginLoader::load_plugin(const std::string& path_to_plugin,
                                      void* plugin_arg,
                                      bool do_lazy_binding)
{
    void* plugin_handle = dlopen(path_to_plugin.c_str(),
                                 do_lazy_binding ? RTLD_LAZY : RTLD_NOW);
    if (plugin_handle == nullptr) {
        throw PluginException((std::string)"Could not load plugin: " + path_to_plugin);
    }

    // lookup the aurum_plugin_init_(void*, void*) function
    void* init_function = dlsym(plugin_handle, "aurum_plugin_init_");
    if (init_function == nullptr) {
        throw PluginException((std::string)"Could not find initialization function " +
                              "\"aurum_plugin_init_(void*, void*)\" in plugin: " +
                              path_to_plugin);
    }
    // ensure that the deinit function exists as well
    void* deinit_function = dlsym(plugin_handle, "aurum_plugin_deinit_");
    if (deinit_function == nullptr) {
        throw PluginException((std::string)"Could not find de-initialization function " +
                              "\"aurum_plugin_deinit_(PluginBase*)\" in plugin: " +
                              path_to_plugin);
    }

    typedef PluginBase* (*InitFuncType)(void*, void*);

    InitFuncType typed_init_function = (InitFuncType)init_function;
    auto retval = typed_init_function(plugin_handle, plugin_arg);

    return retval;
}

void PluginLoader::unload_plugin(PluginBase* plugin_ptr)
{
    auto plugin_handle = plugin_ptr->get_plugin_handle();

    void* deinit_function = dlsym(plugin_handle, "aurum_plugin_deinit_");
    if (deinit_function == nullptr) {
        throw PluginException((std::string)"Could not find de-initialization function " +
                              "\"aurum_plugin_deinit_(PluginBase*)\" in loaded plugin!");
    }
    typedef void (*DeInitFuncType)(PluginBase*);
    DeInitFuncType typed_deinit_function = (DeInitFuncType)deinit_function;
    typed_deinit_function(plugin_ptr);

    dlclose(plugin_handle);
}

PluginException::PluginException() noexcept
    : AurumException()
{
    // Nothing here
}

PluginException::PluginException(const std::string& exception_info) noexcept
    : AurumException((std::string)"PluginException: " + exception_info)
{
    // Nothing here
}

PluginException::PluginException(std::string&& exception_info) noexcept
    : AurumException(std::move(exception_info))
{
    // Nothing here
}

PluginException::PluginException(const PluginException& other) noexcept
    : AurumException(other.get_exception_info())
{
    // Nothing here
}

PluginException::PluginException(PluginException&& other) noexcept
    : AurumException(std::move(other.get_exception_info()))
{
    // Nothing here
}

PluginException::~PluginException() noexcept
{
    // Nothing here
}

PluginException& PluginException::operator = (const PluginException& other) noexcept
{
    if (&other == this) {
        return *this;
    }
    m_exception_info = other.m_exception_info;
    return *this;
}

PluginException& PluginException::operator = (PluginException&& other) noexcept
{
    if (&other == this) {
        return *this;
    }

    m_exception_info = std::move(other.m_exception_info);
    return *this;
}

} /* end namespace plugins */
} /* end namespace aurum */

//
// PluginLoader.cpp ends here
