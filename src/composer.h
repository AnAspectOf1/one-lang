#ifndef _ONE_COMPOSER_H
#define _ONE_COMPOSER_H

#include "context.h"
#include "exception.h"
#include "indexer.h"
#include "statement.h"
#include <chi/buffer.h>
#include <chi/stream.h>
#include <chi/string.h>


namespace one {

	class UnsupportedFormatException : public Exception {
		chi::String<> format;

	public:
		UnsupportedFormatException( const chi::StringBase& format ) : format(format) {}
	};

	class Composer {
		const StatementList* document;
		chi::WriteStream& stream;
		chi::String<> filename;

		void composeFormat( const Context& context, const FormatStatement* statement );
		void composeIdentity( const Context& context, const IdentityStatement* statement );
		void composeLabel( const Context& context, const LabelStatement* statement );
		void composeScope( const Context& context, const ScopeStatement* statement );
		void composeStatement( const Context& context, const Statement* statement );
		void composeStatements( const Context& context, const StatementList& statements );
		void composeString( const Context& context, const StringStatement* statement );

	public:
		Composer( chi::WriteStream& stream, const StatementList* document, const chi::StringBase& filename = chi::String<>() );

		void compose();
	};
}

#endif//_ONE_COMPOSER_H
