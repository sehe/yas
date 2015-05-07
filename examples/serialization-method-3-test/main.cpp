
// Copyright (c) 2010-2015 niXman (i dot nixman dog gmail dot com). All
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

#include <iostream>

#include <yas/mem_streams.hpp>
#include <yas/binary_iarchive.hpp>
#include <yas/binary_oarchive.hpp>
#include <yas/serializers/std_types_serializers.hpp>

/***************************************************************************/

struct type {
	type()
		:i(33)
		,d(.33)
		,s("33")
		,v({33, 33, 33})
	{}

	int i;
	double d;
	std::string s;
	std::vector<int> v;

	// one member-function as serializer/deserializer
	template<typename archive_type>
	void serialize(archive_type& ar) {
		ar & i
			& d
			& s
			& v;
	}
};

/***************************************************************************/

int main() {
	type t1;
	yas::mem_ostream os;
	yas::binary_oarchive<yas::mem_ostream> oa(os);
	oa & t1;

	type t2;
	yas::mem_istream is(os.get_intrusive_buffer());
	yas::binary_iarchive<yas::mem_istream> ia(is);
	ia & t2;

	type t3;
	if ( t2.i != t3.i || t2.d != t3.d || t2.s != t3.s || t2.v != t3.v )
		YAS_THROW_EXCEPTION(std::runtime_error, "bad value");
}

/***************************************************************************/
