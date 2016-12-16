#ifndef _ONE_STATEMENT_H
#define _ONE_STATEMENT_H

#include <chi/allocator.h>
#include <chi/array.h>
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
		StatementType_Definition,       // A new definition
		StatementType_Identity,         // A value based on an existing definition
		StatementType_Else,             // An else statement
		StatementType_Format,           // A format specifier
		StatementType_If,               // An if statement
		StatementType_Include,          // Inclusion of some file
		StatementType_Context,          // A list of statements in a subcontext
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

		StatementType type() const	{ return this->_type; }
	};

	// TODO: Create a linked list class for StatementContext
	typedef chi::Array<chi::SPtr<Statement>, chi::FutureAllocator<chi::SPtr<Statement>>> StatementList;

	class BreakStatement : public Statement {
	public:
		BreakStatement() : Statement( StatementType_Break ) {}
	};

	class DefinitionStatement : public Statement {
	public:
		chi::SPtr<chi::StringBase> name;
		// TODO: An argument list
		chi::SPtr<Statement> body;

		DefinitionStatement() : Statement( StatementType_Definition ) {}
	};

	class IdentityStatement : public Statement {
	public:
		chi::List<chi::SPtr<chi::StringBase>> names;

		IdentityStatement() : Statement( StatementType_Identity ) {}
	};

	class NamespaceStatement : public Statement {
	public:
		chi::Array<chi::SPtr<chi::StringBase>> names;

		NamespaceStatement() : Statement( StatementType_Namespace ) {}
	};

	class NumberStatement : public Statement {
	public:
		long long number;	// TODO: Import the BigInt class from my nctp project

		NumberStatement( long long number = 0 ) : Statement( StatementType_Number ), number(number) {}
	};

	class StringStatement : public Statement {
	public:
		chi::SPtr<chi::StringBase> string;

		StringStatement() : Statement( StatementType_String ) {}
		StringStatement( const StringStatement& other ) : Statement( StatementType_String ), string( other.string ) {}
	};

	class FormatStatement : public Statement {
	public:
		chi::SPtr<chi::StringBase> format;
		chi::SPtr<Statement> statement;

		FormatStatement() : Statement( StatementType_Format ) {}
	};

	class ContextStatement : public Statement {
	public:
		StatementList context;

		ContextStatement() : Statement( StatementType_Context ) {}
	};
}


#endif
