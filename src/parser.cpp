#include "parser.h"

using namespace chi;
using namespace one;


String<> Parser::whitespace = " \t\r\n";

Linked Parser::parse() {
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
	Statement* statement;
	try {
		if ( firstChar == '"' ) {
			statement = new StringLiteralStatement;
			*statement = this->parseStringLiteral(true);
		}
	}
	catch (one::Exception e) {
		delete statement;
		throw e;
	}
	catch (chi::Exception e) {
		delete statement;
		throw e;
	}

	return statement;
}

StringLiteralStatement Parser::parseStringLiteral( bool double_quoted ) {
	StringLiteralStatement statement;

	char delimiter = double_quoted ? '"' : '\'';
	char character;
	while ( (character = this->stream.readChar() ) != delimiter ) {
		if ( delimiter != '\\' )
			statement.string.append( character );
		else {
			character = this->stream.readChar();
			statement.string.append( '?' );
		}
	}

	return statement;
}

char Parser::skipWhitespace() {
	char character;

	while ( this->whitespace.contains( character = this->stream.readChar() ) ) {}

	return character;
}
