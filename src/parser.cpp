#include "parser.h"
#include <chi/allocator.h>
#include <chi/dynamic.h>
#include <cstdio>

using namespace chi;
using namespace one;


String<> Parser::numeric = "0123456789";
String<> Parser::whitespace = " \t\r\n";

unsigned int Parser::currentLine() const {
	return this->stream.newlinesRead() + 1;
}

unsigned int Parser::currentColumn() const {
	return this->stream.curLineRead() + 1;
}

Linked Parser::parse() {
	char c;
	Linked document;
	do {
		try {
			c = this->skipWhitespace();
		}
		catch ( EndOfStreamException e ) {
			return document;
		}

		ManPtr<Statement> statement = this->parseStatement( c );
		document.append( statement );

		try {
			c = this->skip( " \t\r" );	// Skip everything except whitespaces
			if ( c != ',' && c != '\n' )
				throw ParseException( *this, "Statement has too many components" );
		}
		catch ( EndOfStreamException e ) {
			return document;
		}
	}
	while ( true );	
}

ManPtr<Statement> Parser::parseStatement( char firstChar ) {
	ManPtr<Statement> statement;
	try {
		if ( firstChar == '#' ) {
			statement.alloc<FormatStatement>();
			*statement = this->parseFormat();
		}
		if ( firstChar == '"' ) {
			statement.alloc<StringStatement>();
			*statement = this->parseString(true);
			return statement;
		}
		if ( this->numeric.contains( firstChar ) ) {
			statement.alloc<NumberStatement>();
			*statement = this->parseNumber(firstChar);
			return statement;
		}
		else
			throw ParseException( *this, (String<>)"Character '" + firstChar + "' does not begin a statement" );
	}
	catch (one::Exception e) {
	//	delete statement;
		throw e;
	}
	catch (chi::Exception e) {
	//	delete statement;
		throw e;
	}

	return statement;
}

FormatStatement Parser::parseFormat() {
	FormatStatement statement;
	statement.format.alloc<DynamicString>();

	char c = this->skipWhitespace();
	while( !Parser::whitespace.contains(c) ) {
		*statement.format += c;
		c = this->stream.readChar();
	}
	
	c = this->skipWhitespace();
	statement.literal = this->parseStatement( c );
	
	return statement;
}

NumberStatement Parser::parseNumber( char firstChar ) {
	return NumberStatement();
}

StringStatement Parser::parseString( bool double_quoted ) {
	StringStatement statement;
	statement.string.alloc<String<FutureAllocator<char>>>();

	char delimiter = double_quoted ? '"' : '\'';
	char c;
	while ( (c = this->stream.readChar() ) != delimiter ) {
		
		if ( c != '\\' )
			statement.string->append( c );
		else {
			c = this->stream.readChar();
			char special_c = this->_stringSpecialChar(c);
			statement.string->append( special_c );
		}
	}

	return statement;
}

char Parser::_stringSpecialChar( char c ) {
#define ONE_SPECIAL( FROM, TO ) \
	if ( c == FROM )	return TO
	ONE_SPECIAL( '\\', '\\' );
	ONE_SPECIAL( 's', ' ' );
	ONE_SPECIAL( 'n', '\n' );
	ONE_SPECIAL( 't', '\t' );
	ONE_SPECIAL( '"', '\"' );
	ONE_SPECIAL( '\'', '\'' );
#undef ONE_SPECIAL
	else	throw ParseException( *this, (String<>)"Invalid special character in string literal: \"\\" + c + '\"' );
}

char Parser::skip( const String<>& chars ) {
	char c;

	do	{
		c = this->stream.readChar();
		// If comment
		if ( c == ';' ) {
			// Skip everything until newline
			do { c = this->stream.readChar(); }
			while ( c != '\n' );
			c = this->stream.readChar();
		}
	}
	while ( chars.contains( c ) );

	return c;
}

char Parser::skipWhitespace() {
	return this->skip( Parser::whitespace );
}
