#ifndef _ONE_COMPOSER_H
#define _ONE_COMPOSER_H

#include "context.h"
#include "exception.h"
#include "indexer.h"
#include "statement.h"
#include <qi/buffer.h>
#include <qi/stream.h>
#include <qi/string.h>


namespace one {

	class UnsupportedFormatException : public Exception {
		qi::String<> format;

	public:
		UnsupportedFormatException( const qi::StringBase& format ) : format(format) {}
	};

	class Composer {
		const StatementList* document;
		qi::WriteStream& stream;
		qi::String<> filename;

		void composeFormat( const Context& context, const FormatStatement* statement );
		void composeIdentity( const Context& context, const IdentityStatement* statement );
		void composeLabel( const Context& context, const LabelStatement* statement );
		void composeScope( const Context& context, const ScopeStatement* statement );
		void composeStatement( const Context& context, const Statement* statement );
		void composeStatements( const Context& context, const StatementList& statements );
		void composeString( const Context& context, const StringStatement* statement );

	public:
		Composer( qi::WriteStream& stream, const StatementList* document, const qi::StringBase& filename = qi::String<>() );

		void compose();
	};
}

#endif//_ONE_COMPOSER_H
