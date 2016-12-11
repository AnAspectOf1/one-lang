#ifndef _ONE_PARSER_H
#define _ONE_PARSER_H

#include "exception.h"
#include "statement.h"
#include "stream.h"
#include <chi/linked.h>
#include <chi/string.h>


namespace one {

	class ParseException : public Exception {
		chi::String<> msg;		

	public:
		ParseException( const char* message ) : msg(message) {}
		ParseException( const chi::String<>& message ) : msg(message) {}

		const chi::String<>& message() const	{ return this->msg; }
	};

	class Parser {
	protected:
		ReadStream& stream;
		static chi::String<> whitespace;

	public:
		Parser( ReadStream& stream ) : stream( stream ) {}

		chi::Linked parse();
		Statement* parseStatement( char firstChar );
		StringLiteralStatement parseStringLiteral( bool double_quoted = true );
		char skipWhitespace();
	};
}

#endif//_ONE_PARSER_H
