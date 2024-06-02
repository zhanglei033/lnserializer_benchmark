
// Copyright (c) 2010-2023 niXman (github dot nixman at pm dot me). All
// rights reserved.
//
// This file is part of YAS(https://github.com/niXman/yas) project.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#ifndef __yas__std_streams_hpp
#define __yas__std_streams_hpp

#include <yas/detail/tools/cast.hpp>
#include <yas/detail/tools/noncopyable.hpp>

#include <ostream>
#include <istream>

namespace yas {

/***************************************************************************/

struct std_ostream_adapter {
    YAS_NONCOPYABLE(std_ostream_adapter)

    std_ostream_adapter(std::ostream &os)
        :os(os)
    {}

    std::size_t write(const void *ptr, const std::size_t size) {
        os.write(__YAS_SCAST(const char*, ptr), size);

        return size;
    }

    void flush() {
        os.flush();
    }

private:
    std::ostream &os;
}; // struct std_ostream

/***************************************************************************/

struct std_istream_adapter {
    YAS_NONCOPYABLE(std_istream_adapter)

    std_istream_adapter(std::istream &is)
        :is(is)
        ,fsize(0)
    {
        std::streampos pos = is.tellg();
        is.seekg(0, std::ios::end);
        fsize = is.tellg() - pos;
        is.seekg(pos, std::ios::beg);
    }

    std::size_t read(void *ptr, const std::size_t size) {
        return __YAS_SCAST(std::size_t, is.read(__YAS_SCAST(char*, ptr), size).gcount());
    }

    std::size_t available() const { return fsize - is.tellg(); }
    bool empty() const { return is.peek() == EOF; }
    char peekch() const { return __YAS_SCAST(char, is.peek()); }
    char getch() { return __YAS_SCAST(char, is.get()); }
    void ungetch(char ch) { is.putback(ch); }

private:
    std::istream &is;
    std::size_t fsize;
}; // struct std_istream

/***************************************************************************/

} // ns yas

#endif // __yas__std_streams_hpp