#ifndef _ONE_PARSER_H
#define _ONE_PARSER_H

#include "exception.h"
#include "statement.h"
#include "stream.h"
#include "tracker_stream.h"
#include <chi/ptr.h>
#include <chi/linked.h>
#include <chi/string.h>


namespace one {

	class Parser {
	private:
		char _stringSpecialChar( char c );

	protected:
		TrackerStream stream;

		static chi::String<> whitespace;
		static chi::String<> numeric;

	public:
		Parser( ReadStream& stream ) : stream( &stream ) {}

		chi::Linked parse();
		FormatStatement parseFormat();
		NumberStatement parseNumber( char firstChar );
		chi::ManPtr<Statement> parseStatement( char firstChar );
		StringStatement parseString( bool double_quoted = true );
		char skip( const chi::String<>& chars );
		char skipWhitespace();

		unsigned int currentLine() const;
		unsigned int currentColumn() const;
	};

	class ParseException {
	public:
		const unsigned int line, column;
		const chi::String<> msg;

		ParseException( const Parser& parser, const char* message ) : line(parser.currentLine()), column(parser.currentColumn()), msg(message) {}
		ParseException( const Parser& parser, const chi::String<>& message ) : line(parser.currentLine()), column(parser.currentColumn()), msg(message) {}

		const chi::String<>& message() const	{ return this->msg; }
	};
}

#endif//_ONE_PARSER_H
