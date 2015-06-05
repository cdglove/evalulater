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
#include "evalulater/vm/byte_code.hpp"

#include <boost/foreach.hpp>

namespace evalulater
{
	vm::byte_code compiler::compile(ast::statement_list const& x)
	{
		vm::byte_code code("anonymous");
		ast_visitor visitor(code, error_handler);
		BOOST_FOREACH(ast::statement const& s, x)
		{
			visitor(s);
		}
		return code;
	}

	void compiler::compile(ast::statement_list const& x, vm::byte_code& out_code)
	{
		ast_visitor visitor(out_code, error_handler);
		BOOST_FOREACH(ast::statement const& s, x)
		{
			visitor(s);
		}
	}

	void compiler::ast_visitor::operator()(ast::nil) const
	{ 
		BOOST_ASSERT(0);
	}

	void compiler::ast_visitor::operator()(float f) const
	{
		code.push(vm::op_flt);
		code.push(f);
	}

	void compiler::ast_visitor::operator()(ast::binary_op const& x) const
	{
		boost::apply_visitor(*this, x.right);
		switch (x.operator_)
		{
		case ast::op_add:		code.push(vm::op_add); break; 
		case ast::op_subtract:	code.push(vm::op_sub); break;	
		case ast::op_multiply:	code.push(vm::op_mul); break;
		case ast::op_divide:	code.push(vm::op_div); break;
		default: BOOST_ASSERT(0); break;
		}
	}

	void compiler::ast_visitor::operator()(ast::unary_op const& x) const
	{
		boost::apply_visitor(*this, x.right);
		switch (x.operator_)
		{
		case ast::op_negative:	code.push(vm::op_neg); break; 
		case ast::op_positive:								break;	
		case ast::op_not:		code.push(vm::op_not); break;
		default: BOOST_ASSERT(0); break;
		}
	}

	void compiler::ast_visitor::operator()(ast::intrinsic_op const& x) const
	{
		BOOST_FOREACH(ast::operand const& arg, x.args)
		{
			boost::apply_visitor(*this, arg);
		}
		
		switch (x.intrinsic)
		{
		case ast::op_add:		code.push(vm::op_add); break; 
		case ast::op_subtract:  code.push(vm::op_sub); break;	
		case ast::op_multiply:  code.push(vm::op_mul); break;
		case ast::op_divide:    code.push(vm::op_div); break;
		case ast::op_pow:		code.push(vm::op_pow); break; 
		case ast::op_abs:		code.push(vm::op_abs); break; 
		default: BOOST_ASSERT(0); break;
		}
	}

	void compiler::ast_visitor::operator()(ast::expression const& x) const
	{
		boost::apply_visitor(*this, x.first);
		BOOST_FOREACH(ast::operand const& oper, x.rest)
		{
			boost::apply_visitor(*this, oper);
		}
	}

	void compiler::ast_visitor::operator()(ast::assignment const& x) const
	{
		(*this)(x.rhs);
		int const* slot = code.find_local_variable(x.lhs);
		if(slot == NULL)
		{
			slot = code.add_local_variable(x.lhs);
		}

		code.push(vm::op_store);
		code.push(*slot);
	}

	void compiler::ast_visitor::operator()(ast::statement const& x) const
	{
		boost::apply_visitor(*this, x);
	}

	void compiler::ast_visitor::operator()(ast::identifier const& x) const
	{
		int const* lcl_slot = code.find_local_variable(x);
		if(lcl_slot)
		{
			code.push(vm::op_load);
			code.push(*lcl_slot);
			return;
		}

		int const* ext_slot = code.find_external_ref(x);
		if(ext_slot == NULL)
		{
			// Check if we have a local of this variable.  If so
			// use that.  If not, assume its an extern.
			ext_slot = code.add_external_ref(x);
		}

		code.push(vm::op_loadc);
		code.push(*ext_slot);
		return;

	}
}