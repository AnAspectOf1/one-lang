#ifndef _ONE_CONTEXT_H
#define _ONE_CONTEXT_H

#include "index.h"
#include "statement.h"
#include "type.h"
#include <qi/ptr.h>
#include <qi/string.h>


namespace one {

	// The composer context holds information about the current context the Composer is composing for.
	// Most of its members are shared pointers (SPtr) because many contexts are mostly similar and thus hold a lot of the same values.
	class Context {
		const Context* parent;

	public:
		Context() : parent(0) {}
		Context( const Context* parent ) : parent(parent), string_format(parent->string_format), number_format(parent->number_format), space(parent->space), index(parent->index) {}

		qi::CSPtr<qi::StringBase> filename;

		// Formats to use for strings and numbers
		qi::SPtr<qi::StringBase> string_format;
		qi::SPtr<qi::StringBase> number_format;

		// The namespace we are in
		qi::Array<qi::SPtr<qi::StringBase>> space;
		
		// The index for the local context or the argument index for the local definition
		qi::SPtr<Index> index;

		qi::CSPtr<Statement> evaluateDefinition( const Type& type, qi::CSPtr<Statement> statement, const Index& argument_index ) const;
		qi::CSPtr<Statement> evaluateIdentity( const Type& type, qi::CSPtr<IdentityStatement> statement ) const;
		qi::CSPtr<Statement> evaluateLabel( const Type& type, qi::CSPtr<LabelStatement> statement ) const;
		qi::CSPtr<Statement> evaluateScope( const Type& type, qi::CSPtr<ScopeStatement> statement ) const;
		qi::CSPtr<Statement> evaluateStatement( const Type& type, qi::CSPtr<Statement> statement ) const;
	};
}


#endif
