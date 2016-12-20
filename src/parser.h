#ifndef _ONE_PARSER_H
#define _ONE_PARSER_H

#include "exception.h"
#include "file.h"
#include "statement.h"
#include <chi/dynamic.h>
#include <chi/exception.h>
#include <chi/ptr.h>
#include <chi/stream.h>
#include <chi/string.h>


namespace one {

	class ParseException;
	

	class ParserException : public Exception {};
	class EmptyStatementException : public ParserException {};
	class EndOfListException : public ParserException {};
	class EndOfScopeException : public ParserException {};


	class Parser {
		friend ParseException;

	private:
		char _stringSpecialChar( char c );

	protected:
		chi::ReadSeekStream* stream;
		chi::String<> filename;

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
		StatementList parseStatements( bool in_scope, bool in_list );
		FormatStatement parseFormat();
		chi::DynamicString parseName();
		NumberStatement parseNumber();
		chi::SPtr<Statement> parseStatement();
		StringStatement parseString( bool double_quoted = true );
		void skip( const chi::StringBase& chars );
		void skipWhitespace();

	public:
		Parser( chi::ReadSeekStream* stream, const chi::StringBase& filename ) : stream( stream ), filename(filename) {}
		Parser( chi::ReadSeekStream* stream, const char* filename = "" ) : stream( stream ), filename(filename) {}

		FilePos filePos() const;

		StatementList parse();
	};

	class ParseException : public ParserException {
	protected:
		const chi::String<> msg;

	public:
		const FilePos file_pos;

		ParseException( const Parser* parser, const char* message, unsigned int back = 1 ) : msg(message), file_pos(parser->filename, parser->stream->position() - back) {}
		ParseException( const Parser* parser, const chi::StringBase& message, unsigned int back = 1 ) : msg(message), file_pos(parser->filename, parser->stream->position() - back) {}
		ParseException( const FilePos& file_pos, const chi::StringBase& message ) : msg(message), file_pos(file_pos) {}
		ParseException( const FilePos& file_pos, const char* message ) : msg(message), file_pos(file_pos) {}
		ParseException( const chi::StringBase& filename, unsigned int pos, const chi::StringBase& message ) : msg(message), file_pos( filename, pos ) {}

		const chi::StringBase& message() const	{ return this->msg; }
	};
}

#endif//_ONE_PARSER_H
