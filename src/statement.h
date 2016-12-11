#ifndef _ONE_STATEMENT_H
#define _ONE_STATEMENT_H

#include <chi/allocator.h>
#include <chi/array.h>
#include <chi/linked.h>
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
		StatementType_DefValue,          // A value based on an existing definition
		StatementType_Else,             // An else statement
		StatementType_If,               // An if statement
		StatementType_Include,          // Inclusion of some file
		StatementType_Loop,             // A loop statement
		StatementType_Math,             // Math mode
		StatementType_Namespace,        // Declaration of namespace
		StatementType_NumLiteral,       // A value based on a literal number
		StatementType_StringLiteral,    // A value based on a string
	};

	class Statement {
		StatementType type;

	public:
		Statement( StatementType type ) : type(type) {}
	};

	class BreakStatement : public Statement {
	public:
		BreakStatement() : Statement( StatementType_Break ) {}
	};

	class StringLiteralStatement : public Statement {
	public:
		chi::String<chi::FutureAllocator<char>> string;

		StringLiteralStatement() : Statement( StatementType_Else ) {}
	};
}


#endif
