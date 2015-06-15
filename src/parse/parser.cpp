// ****************************************************************************
// dynamo/parse/parser.cpp
//
// Implementation of parser firewall object.
// 
// Copyright Chris Glover 2015
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// ****************************************************************************

#if defined(_MSC_VER)
# pragma warning(disable: 6326 6244 4503)
#endif

#include "dynamo/parse/parser.hpp"
#include "dynamo/parse/parser_def.hpp"

namespace dynamo { namespace parse
{
	// Include at least one instantiation of the parser in the lib.
	boost::optional<
		ast::statement_list
	> parser::parse(std::string const& s) const
	{
		return parse(s.data(), s.data() + s.length());
	}

	boost::optional<
		ast::statement_list
	> parser::parse(char const* first, char const* last) const
	{
		return parse<char const*>(first, last);
	}
}}