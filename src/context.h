#ifndef _ONE_CONTEXT_H
#define _ONE_CONTEXT_H

#include <chi/array.h>


namespace one {

	class Statement;

	typedef chi::Array<Statement> Document;

	enum StatementType {
		StatementType_Break,            // A break
		StatementType_Definition,       // A declaration of a new definition
		StatementType_Else,             // An else statement
		StatementType_If,               // An if statement
		StatementType_Include,          // Inclusion of some file
		StatementType_Loop,             // A loop statement
		StatementType_Math,             // Math mode
		StatementType_Namespace,        // Declaration of namespace
		StatementType_Value             // A value
	};

	class Statement {
		StatementType type;
	};

	class Break : public Statement {
	public:
		BreakStatement() : type( StatementType_Break ), data(0) {}
	};

	class DefinitionStatement : public Statement {
		chi::String name;
	};
}


#endif
