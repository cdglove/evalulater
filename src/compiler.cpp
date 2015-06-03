// ****************************************************************************
// evalulater/compiler.cpp
//
// Implementation of Compiler for evalulater syntax.  
//
// Originally based on Boost.Spirit calc6 example
//
// Copyright Chris Glover 2015
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// ****************************************************************************

#include "evalulater/compiler.hpp"
#include "evalulater/ast.hpp"
#include "evalulater/vm.hpp"

#include <boost/foreach.hpp>

namespace evalulater
{
	std::vector<vm::byte_code> compiler::compile(ast::expression const& x)
	{
		std::vector<vm::byte_code> code;
		ast_visitor visitor(code, error_handler);
		visitor(x);
		return code;
	}

	void compiler::compile(ast::expression const& x, std::vector<vm::byte_code>& out_code)
	{
		ast_visitor visitor(out_code, error_handler);
		visitor(x);
	}

	void compiler::ast_visitor::operator()(ast::nil) const
	{ 
		BOOST_ASSERT(0);
	}

	void compiler::ast_visitor::operator()(float f) const
	{
		code.push_back(vm::op_flt);
		code.push_back(f);
	}

	void compiler::ast_visitor::operator()(ast::binary_op const& x) const
	{
		boost::apply_visitor(*this, x.right);
		switch (x.operator_)
		{
		case ast::bop_add:		code.push_back(vm::op_add); break; 
		case ast::bop_subtract:	code.push_back(vm::op_sub); break;	
		case ast::bop_multiply:	code.push_back(vm::op_mul); break;
		case ast::bop_divide:	code.push_back(vm::op_div); break;
		default: BOOST_ASSERT(0); break;
		}
	}

	void compiler::ast_visitor::operator()(ast::unary_op const& x) const
	{
		boost::apply_visitor(*this, x.right);
		switch (x.operator_)
		{
		case ast::uop_negative:	code.push_back(vm::op_neg); break; 
		case ast::uop_positive:								break;	
		case ast::uop_not:		code.push_back(vm::op_not); break;
		default: BOOST_ASSERT(0); break;
		}
	}

	void compiler::ast_visitor::operator()(ast::intrinsic_op const& x) const
	{
		BOOST_FOREACH(ast::term const& arg, x.args)
		{
			boost::apply_visitor(*this, arg);
		}
		
		switch (x.intrinsic)
		{
		case ast::iop_add:		code.push_back(vm::op_add); break; 
		case ast::iop_subtract: code.push_back(vm::op_sub); break;	
		case ast::iop_multiply: code.push_back(vm::op_mul); break;
		case ast::iop_divide:   code.push_back(vm::op_div); break;
		case ast::iop_pow:		code.push_back(vm::op_pow); break; 
		case ast::iop_abs:		code.push_back(vm::op_abs); break; 
		default: BOOST_ASSERT(0); break;
		}
	}

	void compiler::ast_visitor::operator()(ast::expression const& x) const
	{
		boost::apply_visitor(*this, x.first);
		BOOST_FOREACH(ast::term const& oper, x.rest)
		{
			boost::apply_visitor(*this, oper);
		}
	}

	void compiler::ast_visitor::operator()(ast::identifier const& x) const
	{}
}