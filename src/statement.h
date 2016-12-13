#ifndef _ONE_STATEMENT_H
#define _ONE_STATEMENT_H

#include <chi/allocator.h>
#include <chi/array.h>
#include <chi/linked.h>
#include <chi/ptr.h>
#include <chi/string.h>


namespace one {

	class Statement;

	class Argument {
		chi::String<> name;
		chi::String<> def_name;
	};

	enum StatementType {
		StatementType_Break,            // A break
		StatementType_Definition,       // A declaration of a new definition
		StatementType_Identity,         // A value based on an existing definition
		StatementType_Else,             // An else statement
		StatementType_Format,           // A format specifier
		StatementType_If,               // An if statement
		StatementType_Include,          // Inclusion of some file
		StatementType_List,             // A list of statements
		StatementType_Loop,             // A loop statement
		StatementType_Math,             // Math mode
		StatementType_Namespace,        // Declaration of namespace
		StatementType_Number,           // A value based on a literal number
		StatementType_String            // A value based on a string
	};

	class Statement {
		StatementType _type;

	public:
		Statement( StatementType type ) : _type(type) {}

		StatementType type()	{ return this->_type; }
	};

	class BreakStatement : public Statement {
	public:
		BreakStatement() : Statement( StatementType_Break ) {}
	};

	class NumberStatement : public Statement {
	public:
		long long number;

		NumberStatement( long long number = 0 ) : Statement( StatementType_Number ), number(number) {}
	};

	class StringStatement : public Statement {
	public:
		chi::ManPtr<chi::StringBase> string;

		StringStatement() : Statement( StatementType_String ) {}
		StringStatement( const StringStatement& other ) : Statement( StatementType_String ), string( other.string ) {}
	};

	class FormatStatement : public Statement {
	public:
		chi::ManPtr<chi::StringBase> format;
		chi::ManPtr<Statement> literal;

		FormatStatement() : Statement( StatementType_Format ) {}
		FormatStatement( chi::StringBase& format, Statement& literal ) : Statement( StatementType_Format ), format(format), literal(literal) {}
	};
}


#endif
