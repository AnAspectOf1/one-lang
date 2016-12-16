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
		static chi::String<> alphabet;
		static chi::String<> alphabetUpper;

		ContextStatement parseContext();
		DefinitionStatement parseDefinition();
		IdentityStatement parseIdentity();
		StatementList parseStatements();
		FormatStatement parseFormat();
		DynamicString parseName();
		NumberStatement parseNumber( char firstChar );
		chi::SPtr<Statement> parseStatement( char firstChar );
		StringStatement parseString( bool double_quoted = true );
		char skip( const chi::StringBase& chars );
		char skipWhitespace();

	public:
		Parser( ReadStream& stream ) : stream( &stream ) {}

		unsigned int currentLine() const;
		unsigned int currentColumn() const;

		StatementList parse();
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
