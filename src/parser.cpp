#include "parser.h"
#include <chi/allocator.h>
#include <chi/dynamic.h>
#include <cstdio>

using namespace chi;
using namespace one;


String<> Parser::alphabet = "abcdefghijklmnopqrstuvwxyz";
String<> Parser::alphabetUpper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
String<> Parser::numeric = "0123456789";
String<> Parser::whitespace = " \t\r\n";

unsigned int Parser::currentLine() const {
	return this->stream.newlinesRead() + 1;
}

unsigned int Parser::currentColumn() const {
	return this->stream.curLineRead() + 1;
}

StatementList Parser::parse() {
	return this->parseStatements();
}

ContextStatement Parse::parseContext() {
	ContextStatement statement;
char c;
	do {
		try {
			c = this->skipWhitespace();
		}
		catch ( EndOfStreamException e ) {
			return statement;
		}

		statement.context += this->parseStatement( c );

		try {
			c = this->skip( OS" \t\r" );	// Skip everything except whitespaces
			if ( c != ',' && c != '\n' )
				throw ParseException( *this, "Statement has too many components" );
		}
		catch ( EndOfStreamException e ) {
			break;
		}
	}
	while ( true );

	return statement;
}

DefinitionStatement Parser::parseDefinition() {
	DefinitionStatement statement;
	statement.name.alloc( this->parseName() );
	statement.statement.alloc( this->parseStatement(this->stream.readChar()) );
	return statement;
}

IdentityStatement Parser::parseIdentity( char firstChar ) {
	IdentityStatement statement;
	statement.name.alloc( this->parseName().prepend( firstChar ) );
	return statement;
}

NamespaceStatement Parser::parseNamespace() {
	NamespaceStatement statement;
	char c = this->skipWhitespace();

	DynamicString name;
	while ( !Parser::whitespace.contains(c) ) {
		while ( c != '.' ) {
			if ( Parser::alphabet.contains(c) || Parser::alphabetUpper.contains(c) )
				name += c;
			else if ( Parser::whitespace.contains(c) ) {
				statement.names += name;
				return statement;
			}
			else
				throw ParseException( *this, OS"Found invalid character '" + c + "' in namespace" );
		}

		statement.names += name;
		name = DynamicString();
	}

	return statement;
}

StatementList Parser::parseStatements() {
	char c;
	StatementList document;
	do {
		try {
			c = this->skipWhitespace();
		}
		catch ( EndOfStreamException e ) {
			return document;
		}

		document += this->parseStatement( c );

		try {
			c = this->skip( OS" \t\r" );	// Skip everything except whitespaces
			if ( c != ',' && c != '\n' )
				throw ParseException( *this, "Statement has too many components" );
		}
		catch ( EndOfStreamException e ) {
			break;
		}
	}
	while ( true );

	return document;
}

SPtr<Statement> Parser::parseStatement( char firstChar ) {
	SPtr<Statement> statement;
	try {
		if ( firstChar == '#' )
			statement.alloc( this->parseFormat() );
		else if ( Parser::alphabet.contains(firstChar) || Parser::alphabetUpper.contains(firstChar) )
			statement.alloc( this->parseIdentity(firstChar) );
		else if ( firstChar == '@' )
			statement.alloc( this->parseNamespace() );
		else if ( firstChar == '"' )
			statement.alloc( this->parseString(true) );
		else if ( firstChar == ''' )
			statement.alloc( this->parseString(false) );
		else if ( this->numeric.contains( firstChar ) )
			statement.alloc( this->parseNumber(firstChar) );
		else if ( firstChar == ')' )
			throw ParseException( *this, "Found stray closing parentheses ')'" );
		else
			throw ParseException( *this, OS"Character '" + firstChar + "' does not begin a statement" );
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
	statement.statement = this->parseStatement( c );
	
	return statement;
}

DynamicString Parser::parseName() {
	char c = this->stream.readChar();
	if ( !Parser::alphabet.contains(c) && !Parser::alphabetUpper.contains(c) )
		throw ParseException( *this, OS"First character of a name should be in the alphabet, not '" + c + ''' );

	DynamicString name( 1, c );

	c = this->stream.readChar();
	while ( Parser::alphabet.contains(c) || Parser::alpabetUpper.contains(c) || Parser::numeric.contains(c) ) {
		name += c;

		c = this->stream.readChar();
	}
	if ( !Parser::whitespace.contains(c) )
		throw ParseException( *this, OS"Name not terminated with whitespace but whith character '" + c + ''' );

	return name;
}

NumberStatement Parser::parseNumber( char firstChar ) {
	return NumberStatement();
}

StringStatement Parser::parseString( bool double_quoted ) {
	StringStatement statement;
	statement.string.alloc<DynamicString>();

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
	
	printf( "Parsed: %s %d %p %d\n", statement.string->ptr(), sizeof(DynamicString), statement.string.ptr(), sizeof(StringStatement) );
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

char Parser::skip( const StringBase& chars ) {
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
