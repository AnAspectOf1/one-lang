#ifndef _ONE_COMPOSER_H
#define _ONE_COMPOSER_H

#include "exception.h"
#include "statement.h"
#include "stream.h"
#include <chi/buffer.h>
#include <chi/dynamic.h>
#include <chi/linked.h>


namespace one {

	class UnsupportedFormatException : public Exception {
		chi::String<> format;

	public:
		UnsupportedFormatException( const chi::StringBase& format ) : format(format) {}
	};

	// The composer context holds information about the current context the Composer is composing for.
	// Most of its members are shared pointers (SPtr) because many contexts are mostly similar and thus hold a lot of the same values.
	class ComposerContext {
		ComposerContext* parent;

	public:
		ComposerContext( ComposerContext* parent = 0 ) : parent(parent) {}

		// Formats to use for strings and numbers.
		chi::SPtr<chi::StringBase> string_format;
		chi::SPtr<chi::StringBase> number_format;
		
		// The index for the local context.
		chi::SPtr<Index> index;

		chi::SPtr<Statement> findDefinition( const chi::StringBase& name );
	};

	class Composer {
		const StatementList& document;

		chi::DynamicBuffer composeFormat( const ComposerContext& context, const FormatStatement* statement );
		chi::DynamicBuffer composeStatement( const ComposerContext& context, const Statement* statement );
		chi::DynamicBuffer composeStatements( const ComposerContext& context, const StatementList& statements );
		chi::DynamicBuffer composeString( const chi::StringBase& format, const StringStatement* statement );

	public:
		Composer( const StatementList& document );

		chi::DynamicBuffer compose();
	};
}

#endif//_ONE_COMPOSER_H
