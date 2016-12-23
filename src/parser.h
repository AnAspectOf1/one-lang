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
#include <type_traits>


namespace one {

	class ParseException;
	

	class ParserException : public Exception {};
	class EmptyStatementException : public ParserException {};
	class EndOfLineException : public ParserException {};
	class EndOfScopeException : public ParserException {};


	class Parser {
		friend ParseException;

	private:
		char _stringSpecialChar( char c );
		void stepBack();
		char readChar();

	protected:
		chi::ReadSeekStream* stream;
		chi::CSPtr<chi::String<>> filename;

		static chi::String<> delimiters;
		static chi::String<> whitespace;
		static chi::String<> numeric;
		static chi::String<> alphabet;
		static chi::String<> alphabetUpper;

		DefinitionStatement parseDefinition();
		IdentityStatement parseIdentity();
		ParameterStatement parseParameter();
		chi::LinkedList<ParameterStatement> parseParameters();
		ScopeStatement parseScope();
		StatementList parseStatements( bool in_scope );
		FormatStatement parseFormat();
		chi::DynamicString parseName();
		NumberStatement parseNumber();
		chi::SPtr<Statement> parseStatement();
		StringStatement parseString( bool double_quoted = true );
		void skip( const chi::StringBase& chars );
		void skipWhitespace();

	public:
		template <class S>
		Parser( chi::ReadSeekStream* stream, const S& filename ) : stream( stream ) {
			static_assert( std::is_base_of<chi::StringBase, S>::value, "Argument filename must derive from StringBase" );
			this->filename.alloc( filename );
		}
		Parser( chi::ReadSeekStream* stream, const char* filename = "" ) : stream( stream ) {
			this->filename.alloc( chi::String<>( filename ) );
		}

		FilePos filePos( unsigned int back = 0 ) const;

		StatementList parse();
	};

	class ParseException : public Exception {
	protected:
		const chi::String<> msg;

	public:
		const FilePos file_pos;

		ParseException( const Parser* parser, const char* message, chi::Size back = 1 ) : msg(message), file_pos(parser->filePos(back)) {}
		ParseException( const Parser* parser, const chi::StringBase& message, chi::Size back = 1 ) : msg(message), file_pos(parser->filePos(back)) {}
		ParseException( const FilePos& file_pos, const chi::StringBase& message ) : msg(message), file_pos(file_pos) {}
		ParseException( const FilePos& file_pos, const char* message ) : msg(message), file_pos(file_pos) {}
		ParseException( chi::ReadSeekStream* file, chi::CSPtr<chi::StringBase>& filename, chi::Size pos, const chi::StringBase& message ) : msg(message), file_pos( file, filename, pos ) {}

		const chi::StringBase& message() const	{ return this->msg; }
	};
}

#endif//_ONE_PARSER_H
