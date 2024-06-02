
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

#ifndef __yas__types__boost__boost_tuple_serializers_hpp
#define __yas__types__boost__boost_tuple_serializers_hpp

#if defined(YAS_SERIALIZE_BOOST_TYPES)
#include <yas/detail/tools/cast.hpp>
#include <yas/detail/tools/tuple_element_name.hpp>
#include <yas/detail/type_traits/type_traits.hpp>
#include <yas/detail/type_traits/serializer.hpp>
#include <yas/detail/io/serialization_exceptions.hpp>

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

namespace yas {
namespace detail {

/***************************************************************************/

template<std::size_t F, typename... Types>
struct serializer<
     type_prop::not_a_fundamental
    ,ser_case::use_internal_serializer
    ,F
    ,boost::tuple<Types...>
> {
    template<typename Archive>
    static Archive& save(Archive &ar, const boost::tuple<Types...> &tuple) {
        constexpr std::size_t tuple_size = boost::tuples::length<boost::tuple<Types...>>::value;
        __YAS_CONSTEXPR_IF ( F & options::binary ) {
            ar.write(__YAS_SCAST(std::uint8_t, tuple_size));
        } else if ( F & yas::text ) {
            ar.write(tuple_size);
        }

        __YAS_CONSTEXPR_IF ( F & yas::json ) { ar.write("[", 1); }

        apply<tuple_size>(ar, tuple);

        __YAS_CONSTEXPR_IF ( F & yas::json ) { ar.write("]", 1); }

        return ar;
    }
    template<typename Archive>
    static Archive& load(Archive &ar, boost::tuple<Types...> &tuple) {
        constexpr std::size_t tuple_size = boost::tuples::length<boost::tuple<Types...>>::value;
        __YAS_CONSTEXPR_IF ( F & options::binary ) {
            std::uint8_t size = 0;
            ar.read(size);
            if ( size != tuple_size ) {
                __YAS_THROW_WRONG_SIZE_ON_DESERIALIZE(boost::tuple);
            }
        } else if ( F & yas::text ) {
            std::uint32_t size = 0;
            ar.read(size);
            if ( size != tuple_size ) {
                __YAS_THROW_WRONG_SIZE_ON_DESERIALIZE(boost::tuple);
            }
        }

        __YAS_CONSTEXPR_IF ( F & yas::json ) {
            if ( 0 == tuple_size ) {
                __YAS_THROW_IF_WRONG_JSON_CHARS(ar, "[]");

                return ar;
            }

            __YAS_THROW_IF_WRONG_JSON_CHARS(ar, "[");
        }

        apply<tuple_size>(ar, tuple);

        __YAS_CONSTEXPR_IF ( F & yas::json ) {
            __YAS_THROW_IF_WRONG_JSON_CHARS(ar, "]");
        }

        return ar;
    }

private:
    template<std::size_t S, std::size_t I = 0, typename Archive, typename... Tp>
    static typename std::enable_if<I == S, Archive&>::type
    apply(Archive &ar, const boost::tuple<Tp...> &)
    { return ar; }
    template<std::size_t S, std::size_t I = 0, typename Archive, typename... Tp>
    static typename std::enable_if<I < S, Archive&>::type
    apply(Archive &ar, const boost::tuple<Tp...> &t) {
        using tuple_element_name_t = tuple_element_name<I>;
        ar & YAS_OBJECT_NVP(
             nullptr
            ,(tuple_element_name_t::c_str(), boost::get<I>(t))
        );

        __YAS_CONSTEXPR_IF ( (F & yas::json) && I+1 < S ) {
            ar.write(",", 1);
        }

        return apply<S, I+1>(ar, t);
    }

    template<std::size_t S, std::size_t I = 0, typename Archive, typename... Tp>
    static typename std::enable_if<I == S, Archive&>::type
    apply(Archive &ar, boost::tuple<Tp...> &)
    { return ar; }
    template<std::size_t S, std::size_t I = 0, typename Archive, typename... Tp>
    static typename std::enable_if<I < S, Archive&>::type
    apply(Archive &ar, boost::tuple<Tp...> &t) {
        using tuple_element_name_t = tuple_element_name<I>;
        ar & YAS_OBJECT_NVP(
             nullptr
            ,(tuple_element_name_t::c_str(), boost::get<I>(t))
        );

        __YAS_CONSTEXPR_IF ( (F & yas::json) && I+1 < S ) {
            __YAS_THROW_IF_WRONG_JSON_CHARS(ar, ",");
        }

        return apply<S, I+1>(ar, t);
    }
};

/***************************************************************************/

} // namespace detail
} // namespace yas

#endif // defined(YAS_SERIALIZE_BOOST_TYPES)

#endif // __yas__types__boost__boost_tuple_serializers_hpp
