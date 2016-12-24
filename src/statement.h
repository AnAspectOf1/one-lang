#ifndef _ONE_STATEMENT_H
#define _ONE_STATEMENT_H

#include "file.h"
#include "parameter.h"
#include <chi/linked.h>
#include <chi/ptr.h>
#include <chi/string.h>
#include <vector>


namespace one {

	class ParameterStatement;
	class Statement;
	class StringStatement;



	enum StatementType {
		StatementType_Definition,       // A new definition
		StatementType_Format,           // A format specifier
		StatementType_Identity,         // A value based on an existing definition
		StatementType_Include,          // An include directive
		StatementType_Label,            // A definition and regular statement at the same time
		StatementType_Number,           // A value based on a literal number
		StatementType_Scope,            // A list of statements in a subcontext
		StatementType_String            // A value based on a string
	};

	class Statement {
		StatementType _type;

	public:
		FilePos pos;	// The file and position where it was found

		Statement( StatementType type ) : _type(type) {}

		StatementType type() const	{ return this->_type; }

		virtual ~Statement() {}

		virtual bool evaluates() const	{ return false; }

		chi::String<> typeName() const {
			static chi::String<> names[] = {
				"definition",
				"format",
				"identity",
				"include",
				"label",
				"number",
				"scope",
				"string"
			};

			return names[ this->_type ];
		}
	};

	typedef chi::LinkedList<chi::CSPtr<Statement>> StatementList;

	class DefinitionStatement : public Statement {
	public:
		chi::CSPtr<chi::StringBase> name;
		chi::Size name_pos;
		chi::LinkedList<ParameterStatement> params;
		chi::CSPtr<Statement> body;

		DefinitionStatement() : Statement( StatementType_Definition ) {}
	};

	class FormatStatement : public Statement {
	public:
		chi::SPtr<chi::StringBase> format;
		chi::SPtr<Statement> statement;

		FormatStatement() : Statement( StatementType_Format ) {}
	};

	class IdentityStatement : public Statement {
	public:
		chi::LinkedList<chi::SPtr<chi::StringBase>> names;
		StatementList args;

		IdentityStatement() : Statement( StatementType_Identity ) {}

		bool evaluates() const override	{ return true; }
	};

	class IncludeStatement : public Statement {
	public:
		chi::SPtr<StringStatement> filename;
		StatementList statements;

		IncludeStatement() : Statement( StatementType_Include ) {}
	};

	class LabelStatement : public Statement {
	public:
		chi::SPtr<chi::StringBase> name;
		chi::Size name_pos;
		chi::SPtr<Statement> body;

		LabelStatement() : Statement( StatementType_Label ) {}

		bool evaluates() const override	{ return true; }
	};

	/*class NamespaceStatement : public Statement {
	public:
		chi::LinkedList<chi::SPtr<chi::StringBase>> names;
		unsigned int name_pos;

		NamespaceStatement() : Statement( StatementType_Namespace ) {}
	};*/

	class NumberStatement : public Statement {
	public:
		long long number;	// TODO: Import the BigInt class from my nctp project

		NumberStatement( long long number = 0 ) : Statement( StatementType_Number ), number(number) {}

		bool evaluates() const override	{ return true; }
	};

	struct ParameterStatement {	// Not a real statement, but needs to be called something that is different from Parameter in definition.h
		chi::CSPtr<chi::StringBase> name, type_name;
		chi::Size name_pos, type_pos;
	};

	class ScopeStatement : public Statement {
	public:
		StatementList contents;

		ScopeStatement() : Statement( StatementType_Scope ) {}
	
		bool evaluates() const override	{ return true; }
	};

	class StringStatement : public Statement {
	public:
		chi::SPtr<chi::StringBase> string;

		StringStatement() : Statement( StatementType_String ) {}
		StringStatement( const StringStatement& other ) : Statement( StatementType_String ), string( other.string ) {}

		bool evaluates() const override	{ return true; }
	};
}


#endif
