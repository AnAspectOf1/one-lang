#include "parser.h"

using namespace chi;
using namespace one;


String<> Parser::whitespace = " \t\r\n";

Linked Parser::parse() { printf( "parse\n" );
	char firstChar;
	Linked document;
	do {
		try {
			firstChar = this->skipWhitespace();
		}
		catch ( EndOfStreamException e ) {
			return document;
		}

		Statement* statement = this->parseStatement( firstChar );
		document.append( statement );
	}
	while ( true );	
}

Statement* Parser::parseStatement( char firstChar ) {
	try {
		printf( "char: %c\n", firstChar ); 
		if ( firstChar == '"' ) {
			StringLiteralStatement* statement = new StringLiteralStatement;
			*statement = this->parseStringLiteral(true);
			printf( "string2 statement: %s %d\n", statement->string.ptr(), statement->string.length() );
			return statement;
		}
		else
			ONE_ASSERT( true, "Character '%c' does not begin a statement", firstChar );
	}
	catch (one::Exception e) {
	//	delete statement;
		throw e;
	}
	catch (chi::Exception e) {
	//	delete statement;
		throw e;
	}

	return 0;
}

StringLiteralStatement Parser::parseStringLiteral( bool double_quoted ) {
	StringLiteralStatement statement;

	char delimiter = double_quoted ? '"' : '\'';
	char character;
	while ( (character = this->stream.readChar() ) != delimiter ) {
		
		if ( character != '\\' )
			statement.string.append( character );
		else {
			character = this->stream.readChar();
			statement.string.append( '?' );
		}
	}

	return statement;
}

char Parser::skipWhitespace() {
	char c;

	do	{ c = this->stream.readChar(); }
	while ( c == ' ' || c == '\t' || c == '\r' || c == '\n' /*this->whitespace.contains( character )*/ );

	return c;
}
