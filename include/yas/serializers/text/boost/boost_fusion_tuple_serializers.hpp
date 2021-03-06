
// Copyright (c) 2010-2016 niXman (i dot nixman dog gmail dot com). All
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

#ifndef _yas__text__boost_fusion_tuple_serializer_hpp
#define _yas__text__boost_fusion_tuple_serializer_hpp

#include <yas/detail/config/config.hpp>

#if defined(YAS_SERIALIZE_BOOST_TYPES)
#include <yas/serializers/serializer.hpp>
#include <yas/detail/type_traits/selector.hpp>
#include <yas/detail/io/serialization_exception.hpp>
#include <yas/detail/preprocessor/preprocessor.hpp>

#include <yas/serializers/detail/boost_fusion_containers_for_each.hpp>

#include <boost/fusion/tuple.hpp>

#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>

namespace yas {
namespace detail {

#ifdef BOOST_FUSION_HAS_VARIADIC_VECTOR

/***************************************************************************/

template<typename... Types>
struct serializer<
	 type_prop::not_a_pod
	,ser_method::use_internal_serializer
	,archive_type::text
	,boost::fusion::tuple<Types...>
> {
	template<typename Archive>
	static Archive& save(Archive& ar, const boost::fusion::tuple<Types...> &tuple) {
		ar & (std::uint32_t)sizeof...(Types);
		boost::fusion::for_each(tuple, ofusion_sequence_apply<Archive>(ar));

		return ar;
	}

	template<typename Archive>
	static Archive& load(Archive& ar, boost::fusion::tuple<Types...> &tuple) {
		std::uint32_t size = 0;
		ar & size;
		if ( size != sizeof...(Types) )
			YAS_THROW_BAD_SIZE_ON_DESERIALIZE("fusion::tuple");
		boost::fusion::for_each(tuple, ifusion_sequence_apply<Archive>(ar));

		return ar;
	}
};

/***************************************************************************/

#else // BOOST_FUSION_HAS_VARIADIC_VECTOR

/***************************************************************************/

#define YAS__TEXT__GENERATE_EMPTY_SERIALIZE_BOOST_FUSION_TUPLE_SPEC() \
	template<> \
	struct serializer<type_prop::type_prop::not_a_pod,ser_method::use_internal_serializer, \
		archive_type::text, boost::fusion::tuple<> > \
	{ \
		template<typename Archive> \
		static Archive& save(Archive& ar, const boost::fusion::tuple<> &) { \
			ar & (std::uint32_t)0; \
			\
			return ar; \
		} \
		\
		template<typename Archive> \
		static Archive& load(Archive& ar, boost::fusion::tuple<> &) { \
			std::uint32_t size = 0; \
			ar & size; \
			if ( size != 0 ) \
				YAS_THROW_BAD_SIZE_ON_DESERIALIZE("fusion::tuple, expected size == 0"); \
			\
			return ar; \
		} \
	};

#define YAS__TEXT__GENERATE_SERIALIZE_BOOST_FUSION_TUPLE_FUNCTION_SPEC(unused, count, unused2) \
	template<YAS_PP_ENUM_PARAMS(YAS_PP_INC(count), typename T)> \
	struct serializer<type_prop::type_prop::not_a_pod,ser_method::use_internal_serializer, \
		archive_type::text, boost::fusion::tuple<YAS_PP_ENUM_PARAMS(YAS_PP_INC(count), T)> > \
	{ \
		template<typename Archive> \
		static Archive& save(Archive& ar, const boost::fusion::tuple<YAS_PP_ENUM_PARAMS(YAS_PP_INC(count), T)>& tuple) { \
			ar & (std::uint32_t)YAS_PP_INC(count); \
			boost::fusion::for_each(tuple, ofusion_sequence_apply<Archive>(ar)); \
			\
			return ar; \
		} \
		\
		template<typename Archive> \
		static Archive& load(Archive& ar, boost::fusion::tuple<YAS_PP_ENUM_PARAMS(YAS_PP_INC(count), T)>& tuple) { \
			std::uint32_t size = 0; \
			ar & size; \
			if ( size != YAS_PP_INC(count) ) \
				YAS_THROW_BAD_SIZE_ON_DESERIALIZE("fusion::tuple"); \
			boost::fusion::for_each(tuple, ifusion_sequence_apply<Archive>(ar)); \
			\
			return ar; \
		} \
	};

#define YAS__TEXT__GENERATE_SERIALIZE_BOOST_FUSION_TUPLE_FUNCTIONS_SPEC(count) \
	YAS__TEXT__GENERATE_EMPTY_SERIALIZE_BOOST_FUSION_TUPLE_SPEC() \
	YAS_PP_REPEAT( \
		count, \
		YAS__TEXT__GENERATE_SERIALIZE_BOOST_FUSION_TUPLE_FUNCTION_SPEC, \
		~ \
	)

/***************************************************************************/

YAS__TEXT__GENERATE_SERIALIZE_BOOST_FUSION_TUPLE_FUNCTIONS_SPEC(FUSION_MAX_VECTOR_SIZE)

/***************************************************************************/

#endif // BOOST_FUSION_HAS_VARIADIC_VECTOR

} // namespace detail
} // namespace yas

#endif // defined(YAS_SERIALIZE_BOOST_TYPES)

#endif // _yas__text__boost_fusion_tuple_serializer_hpp
