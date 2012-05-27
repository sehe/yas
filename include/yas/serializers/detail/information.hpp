
// Copyright (c) 2010-2012 niXman (i dot nixman dog gmail dot com)
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

#ifndef _yas__information_hpp__included_
#define _yas__information_hpp__included_

#include <stddef.h>
#include <string.h>
#include <stdexcept>
#include <sstream>

#include <yas/config/config.hpp>
#include <yas/tools/version.hpp>
#include <yas/serializers/detail/properties.hpp>
#include <yas/tools/boost_preprocessor/preprocessor.hpp>

namespace yas {

/***************************************************************************/

struct empty_archive: std::exception {
	const char* what() const throw() {return "archive is empty";}
};
struct bad_archive_information: std::exception {
	const char* what() const throw() {return "archive is corrupted or try to use with \"no_header\" flag";}
};
struct no_header: std::exception {
	const char* what() const throw() {return "you cannot use information functions with \"no_header\" flag";}
};

/***************************************************************************/

} // namespace yas

/***************************************************************************/

namespace yas {
namespace detail {

#pragma pack(push, 1)
union archive_header {
	archive_header()
		:as_char(0)
	{}
	archive_header(char c)
		:as_char(c)
	{}
	archive_header(unsigned char v, unsigned char t, unsigned char b)
	{ bits.version=v;bits.type=t;bits.bits=b; }

	struct {
		unsigned char version:4; // 0 ... 15
		unsigned char type:3; // binary, text, json
		unsigned char bits:1; // 0 - 32 bit, 1 - 64 bit
	} bits;
	char as_char;
};
#pragma pack(pop)

extern char __ALIGNMENT_ERROR__[sizeof(archive_header) == sizeof(char)?1:-1];

/***************************************************************************/

static const char yas_id[3] = {'y', 'a', 's'};
static const char full_header_size = sizeof(yas_id)+sizeof(archive_header);

/***************************************************************************/

#define YAS_WRITE_ARCHIVE_INFORMATION_SPECIALIZATION_IMPL(unused, idx, seq) \
	template<typename Ar> \
	struct archive_information<YAS_PP_SEQ_ELEM(idx, seq), e_direction::in, Ar> { \
		archive_information(Ar* ar, header_t::type op) \
			:header() \
		{ \
			if ( op == header_t::no_header ) {return;} \
			typename Ar::char_type raw_hdr[full_header_size]; \
			if ( ar->sgetn(raw_hdr, full_header_size) != full_header_size ) { \
				throw empty_archive(); \
			} \
			if ( memcmp(raw_hdr, yas_id, sizeof(yas_id)) ) { \
				throw bad_archive_information(); \
			} \
			header = *reinterpret_cast<archive_header*>(raw_hdr+sizeof(yas_id)); \
		} \
		\
		e_archive_type::type archive_type() const {return YAS_PP_SEQ_ELEM(idx, seq);} \
		int bits() const {return(header.bits.version)?header.bits.bits?64:32:(throw no_header());} \
		int version() const {return(header.bits.version)?header.bits.version:(throw no_header());} \
		\
		static const e_archive_type::type	_archive_type	= YAS_PP_SEQ_ELEM(idx, seq); \
		static const e_direction::type		_direction		= e_direction::in; \
		static const bool							_is_readable	= true; \
		static const bool							_is_writable	= false; \
		\
	private: \
		archive_header header; \
	}; \
	\
	template<typename Ar> \
	struct archive_information<YAS_PP_SEQ_ELEM(idx, seq), e_direction::out, Ar> { \
		archive_information(Ar* ar, header_t::type op) { \
			if ( op == header_t::no_header ) {return;} \
			static const archive_header hdr( \
				(unsigned char)archive_version, \
				(unsigned char)YAS_PP_SEQ_ELEM(idx, seq), \
				(unsigned char)YAS_PLATFORM_BITS_IS_64() \
			); \
			ar->sputn(yas_id, sizeof(yas_id)); \
			ar->sputc(hdr.as_char); \
		} \
		\
		e_archive_type::type archive_type() const {return YAS_PP_SEQ_ELEM(idx, seq);} \
		int bits() const {return YAS_PLATFORM_BITS();} \
		int version() const {return archive_version;} \
		\
		static const e_archive_type::type	_archive_type	= YAS_PP_SEQ_ELEM(idx, seq); \
		static const e_direction::type		_direction		= e_direction::in; \
		static const bool							_is_readable	= false; \
		static const bool							_is_writable	= true; \
	};

#define YAS_WRITE_ARCHIVE_INFORMATION_SPECIALIZATIONS(seq) \
	template<e_archive_type::type, e_direction::type, typename> \
	struct archive_information; \
	YAS_PP_REPEAT( \
		YAS_PP_SEQ_SIZE(seq), \
		YAS_WRITE_ARCHIVE_INFORMATION_SPECIALIZATION_IMPL, \
		seq \
	)

/***************************************************************************/

YAS_WRITE_ARCHIVE_INFORMATION_SPECIALIZATIONS(
	(e_archive_type::binary)
	(e_archive_type::text)
	(e_archive_type::json)
);

/***************************************************************************/

} // namespace detail
} // namespace yas

#endif // _yas__information_hpp__included_
