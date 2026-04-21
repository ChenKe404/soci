//
// Copyright (C) 2016 Maciej Sobczak
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef SOCI_TYPE_WRAPPERS_H_INCLUDED
#define SOCI_TYPE_WRAPPERS_H_INCLUDED

#include <cstdint>
#include <string>
#include <vector>

namespace soci
{

// These wrapper types can be used by the application
// with 'into' and 'use' elements in order to guide the library
// in selecting specialized methods for binding and transferring data;
// if the target database does not provide any such specialized methods,
// it is expected to handle these wrappers as equivalent
// to their contained field types.

struct xml_type
{
    std::string value;
};

struct long_string
{
    std::string value;
};

struct binary : public std::vector<uint8_t>
{
    using super = std::vector<uint8_t>;

    inline binary(const std::initializer_list<uint8_t>& list) : super(list)
    {}

    template<typename ..._Args>
    inline binary(_Args&&... args)  : super(std::forward<_Args>(args)...)
    {}
};

} // namespace soci

#endif // SOCI_TYPE_WRAPPERS_H_INCLUDED
