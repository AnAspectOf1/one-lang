#ifndef _ONE_PARSER_H
#define _ONE_PARSER_H

#include "exception.h"
#include "file.h"
#include "statement.h"
#include <qi/dynamic.h>
#include <qi/exception.h>
#include <qi/ptr.h>
#include <qi/stream.h>
#include <qi/string.h>
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
		qi::ReadSeekStream* stream;
		qi::CSPtr<qi::String<>> filename;

		static qi::String<> delimiters;
		static qi::String<> whitespace;
		static qi::String<> numeric;
		static qi::String<> alphabet;
		static qi::String<> alphabetUpper;

		DefinitionStatement parseDefinition();
		IdentityStatement parseIdentity();
		IncludeStatement parseInclude();
		LabelStatement parseLabel();
		ParameterStatement parseParameter();
		qi::LinkedList<ParameterStatement> parseParameters();
		ScopeStatement parseScope();
		StatementList parseStatements( bool in_scope );
		FormatStatement parseFormat();
		qi::DynamicString parseName();
		NumberStatement parseNumber();
		qi::SPtr<Statement> parseStatement();
		StringStatement parseString( bool double_quoted = true );
		void skip( const qi::StringBase& chars );
		void skipWhitespace();

	public:
		template <class S>
		Parser( qi::ReadSeekStream* stream, const S& filename ) : stream( stream ) {
			static_assert( std::is_base_of<qi::StringBase, S>::value, "Argument filename must derive from StringBase" );
			this->filename.alloc( filename );
		}
		Parser( qi::ReadSeekStream* stream, const char* filename = "" ) : stream( stream ) {
			this->filename.alloc( qi::String<>( filename ) );
		}

		FilePos filePos( unsigned int back = 0 ) const;

		StatementList parse();
	};

	class ParseException : public Exception {
	protected:
		const qi::String<> msg;

	public:
		const FilePos file_pos;

		ParseException( const Parser* parser, const char* message, qi::Size back = 1 ) : msg(message), file_pos(parser->filePos(back)) {}
		ParseException( const Parser* parser, const qi::StringBase& message, qi::Size back = 1 ) : msg(message), file_pos(parser->filePos(back)) {}
		ParseException( const FilePos& file_pos, const qi::StringBase& message ) : msg(message), file_pos(file_pos) {}
		ParseException( const FilePos& file_pos, const char* message ) : msg(message), file_pos(file_pos) {}
		ParseException( qi::ReadSeekStream* file, qi::CSPtr<qi::StringBase>& filename, qi::Size pos, const qi::StringBase& message ) : msg(message), file_pos( file, filename, pos ) {}

		const qi::StringBase& message() const	{ return this->msg; }
	};
}

#endif//_ONE_PARSER_H
