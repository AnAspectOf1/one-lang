#ifndef _ONE_STATEMENT_H
#define _ONE_STATEMENT_H

#include <chi/linked.h>
#include <chi/ptr.h>
#include <chi/string.h>
#include <vector>


namespace one {

	class Statement;

	struct Parameter {
		chi::CSPtr<chi::StringBase> name, type_name;
		unsigned int name_pos, type_name_pos;
	};

	enum StatementType {
		StatementType_Break,            // A break
		StatementType_Definition,       // A new definition
		StatementType_Identity,         // A value based on an existing definition
		StatementType_Else,             // An else statement
		StatementType_Format,           // A format specifier
		StatementType_If,               // An if statement
		StatementType_Include,          // Inclusion of some file
		StatementType_Loop,             // A loop statement
		StatementType_Math,             // Math mode
		StatementType_Namespace,        // Declaration of namespace
		StatementType_Number,           // A value based on a literal number
		StatementType_Scope,            // A list of statements in a subcontext
		StatementType_String            // A value based on a string
	};

	class Statement {
		StatementType _type;

	public:
		unsigned int pos;	// The position in the file where is was found

		Statement( StatementType type ) : _type(type) {}

		StatementType type() const	{ return this->_type; }

		virtual ~Statement() {}
	};

	typedef chi::LinkedList<chi::CSPtr<Statement>> StatementList;
	// TODO: The above definition of StatementList seems to give segmentations faults. Resolve this.
	// In the meantime, use the following as a replacement:
	/*class StatementList : public chi::ArrayBase<chi::CSPtr<Statement>> {
	protected:
		std::vector<chi::CSPtr<Statement>> vector;

	public:
		~StatementList() { printf( "DDDDD StatementList %d\n", this->vector.size() ); }

		chi::CSPtr<Statement>& at( chi::Size index ) override	{ return this->vector[ index ]; }
		const chi::CSPtr<Statement>& at( chi::Size index ) const override	{ return this->vector[ index ]; }

		chi::Size count() const	{ return this->vector.size(); }

		chi::CSPtr<Statement>* ptr() override	{ return &this->vector[0]; }
		const chi::CSPtr<Statement>* ptr() const override	{ return &this->vector[0]; }

		void grow( chi::Size increment ) override {
			this->vector.reserve( this->count() + increment );
			for ( chi::Size i = 0; i < increment; i++ ) {		
				this->vector.push_back( chi::CSPtr<Statement>() );
			}
		}

		void shrink( chi::Size decrement ) override {
			for ( chi::Size i = 0; i < decrement; i++ ) {		
				this->vector.pop_back();
			}
		}
	};*/

	class BreakStatement : public Statement {
	public:
		BreakStatement() : Statement( StatementType_Break ) {}
	};

	class DefinitionStatement : public Statement {
	public:
		chi::SPtr<chi::StringBase> name;
		unsigned int name_pos;
		chi::LinkedList<Parameter> params;
		chi::SPtr<Statement> body;

		DefinitionStatement() : Statement( StatementType_Definition ) {}
	};

	class IdentityStatement : public Statement {
	public:
		chi::LinkedList<chi::SPtr<chi::StringBase>> names;
		unsigned int name_pos;
		StatementList args;

		IdentityStatement() : Statement( StatementType_Identity ) {}
	};

	class NamespaceStatement : public Statement {
	public:
		chi::LinkedList<chi::SPtr<chi::StringBase>> names;
		unsigned int name_pos;

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

	class ScopeStatement : public Statement {
	public:
		StatementList contents;

		ScopeStatement() : Statement( StatementType_Scope ) {}
	};
}


#endif
