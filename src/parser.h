#ifndef _ONE_PARSER_H
#define _ONE_PARSER_H

#include "exception.h"
#include "statement.h"
#include "tracker_stream.h"
#include <chi/dynamic.h>
#include <chi/exception.h>
#include <chi/ptr.h>
#include <chi/stream.h>
#include <chi/string.h>


namespace one {

	class ParserException : public Exception {};
	class EmptyStatementException : public ParserException {};
	class EndOfScopeException : public ParserException {};

	class Parser {
	private:
		char _stringSpecialChar( char c );

	protected:
		TrackerStream stream;

		static chi::String<> delimiters;
		static chi::String<> whitespace;
		static chi::String<> numeric;
		static chi::String<> alphabet;
		static chi::String<> alphabetUpper;

		DefinitionStatement parseDefinition();
		IdentityStatement parseIdentity();
		Parameter parseParameter();
		chi::LinkedList<Parameter> parseParameters();
		ScopeStatement parseScope();
		StatementList parseStatements( bool local = false );
		FormatStatement parseFormat();
		chi::DynamicString parseName();
		NumberStatement parseNumber();
		chi::SPtr<Statement> parseStatement();
		StringStatement parseString( bool double_quoted = true );
		void skip( const chi::StringBase& chars );
		void skipWhitespace();

	public:
		Parser( chi::ReadStream* stream, chi::Seekable* seekable ) : stream( stream, seekable ) {}

		unsigned int currentLine() const;
		unsigned int currentColumn() const;

		StatementList parse();
	};

	class ParseException : public ParserException {
	public:
		const unsigned int line, column;
		const chi::String<> msg;

		ParseException( const Parser& parser, const char* message ) : line(parser.currentLine()), column(parser.currentColumn()), msg(message) {}
		ParseException( const Parser& parser, const chi::String<>& message ) : line(parser.currentLine()), column(parser.currentColumn()), msg(message) {}

		const chi::String<>& message() const	{ return this->msg; }
	};
}

#endif//_ONE_PARSER_H
