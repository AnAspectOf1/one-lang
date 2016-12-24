#ifndef _ONE_CONTEXT_H
#define _ONE_CONTEXT_H

#include "index.h"
#include "statement.h"
#include "type.h"
#include <chi/ptr.h>
#include <chi/string.h>


namespace one {

	// The composer context holds information about the current context the Composer is composing for.
	// Most of its members are shared pointers (SPtr) because many contexts are mostly similar and thus hold a lot of the same values.
	class Context {
		const Context* parent;

	public:
		Context() : parent(0) {}
		Context( const Context* parent ) : parent(parent), string_format(parent->string_format), number_format(parent->number_format), space(parent->space), index(parent->index) {}

		chi::CSPtr<chi::StringBase> filename;

		// Formats to use for strings and numbers
		chi::SPtr<chi::StringBase> string_format;
		chi::SPtr<chi::StringBase> number_format;

		// The namespace we are in
		chi::Array<chi::SPtr<chi::StringBase>> space;
		
		// The index for the local context or the argument index for the local definition
		chi::SPtr<Index> index;

		chi::CSPtr<Statement> evaluateDefinition( const Type& type, chi::CSPtr<Statement> statement, const Index& argument_index ) const;
		chi::CSPtr<Statement> evaluateIdentity( const Type& type, chi::CSPtr<IdentityStatement> statement ) const;
		chi::CSPtr<Statement> evaluateScope( const Type& type, chi::CSPtr<ScopeStatement> statement ) const;
		chi::CSPtr<Statement> evaluateStatement( const Type& type, chi::CSPtr<Statement> statement ) const;
	};
}


#endif
