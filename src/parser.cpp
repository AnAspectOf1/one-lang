#include "parser.h"
#include <chi/allocator.h>
#include <chi/dynamic.h>
#include <cstdio>

using namespace chi;
using namespace one;


String<> Parser::alphabet = "abcdefghijklmnopqrstuvwxyz";
String<> Parser::alphabetUpper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
String<> Parser::delimiters = ",\n";
String<> Parser::numeric = "0123456789";
String<> Parser::whitespace = " \t\r";


FilePos Parser::filePos() const {
	return FilePos( this->filename, this->stream->position() );
}

StatementList Parser::parse() {
	try {
		return this->parseStatements(false, false);
	}
	catch ( EndOfScopeException e ) {
		throw ParseException( this, "Stray closing parentheses ')' found" );
	}
}

/*Statement Parser::parseArgumentStatement() {
	SPtr<Statement> statement;
	char c = this->stream->readChar();

	if ( Parser::delimiters.contains(c) ) {
		this->stream->move(-1);
		throw EmptyStatementException();
	}
	else if ( Parser::alphabet.contains(c) || Parser::alphabetUpper.contains(c) ) {
		this->stream->move(-1);
		statement.alloc( this->parseIdentity() );
	}
	else if ( c == '"' )
		statement.alloc( this->parseString(true) );
	else if ( c == '\'' )
		statement.alloc( this->parseString(false) );
	else if ( this->numeric.contains( c ) ) {		
		this->stream->move(-1);
		statement.alloc( this->parseNumber() );
	}
	else if ( c == '#' )
		statement.alloc( this->parseFormat() );
	else if ( c == '(' ) {
		statement.alloc( this->parseScope() );
	}
	else if ( c == ')' )
		throw EndOfScopeException();
	else if ( c == '@' )
		throw ParseException( this, "Namespaces not allowed in argument list" );
	else if ( c == '$' )
		throw ParseException( this, "Definitions not allowed in argument list" );
	else
		throw ParseException( this, OS"Character '" + c + "' does not begin a statement" );

	return statement;
}*/

DefinitionStatement Parser::parseDefinition() {
	DefinitionStatement statement;
	statement.name.alloc( this->parseName() );

	char c = this->stream->readChar();
	if ( c == '[' )
		statement.params = this->parseParameters();
	else
		this->stream->move(-1);

	this->skipWhitespace();

	statement.body = this->parseStatement();

	return statement;
}

IdentityStatement Parser::parseIdentity() {
	IdentityStatement statement;
	
	SPtr<StringBase> name; name.alloc( this->parseName() );
	statement.names += name;

	char nextChar;
	while ( (nextChar = this->stream->readChar()) == '.' ) {
		
		name.alloc( this->parseName() );
		statement.names += name;
	}
	if ( nextChar != '[' )
		this->stream->move(-1);
	else {
		statement.args = this->parseStatements( false, true );
		for ( Size i = 0; i < statement.args.count(); i++ ) {
			StatementType arg_type = statement.args[i]->type();

			if ( arg_type == StatementType_Definition )
				throw ParseException( FilePos( this->filename, statement.args[i]->pos ), "Definitions not allowed in argument list" );
			if ( arg_type == StatementType_Namespace )
				throw ParseException( FilePos( this->filename, statement.args[i]->pos ), "Namespaces not allowed in argument list" );
		}
	}

	return statement;
}

/*NamespaceStatement Parser::parseNamespace() {
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
}*/

Parameter Parser::parseParameter() {
	Parameter param;

	param.name.alloc( this->parseName() );
	
	this->skipWhitespace();
	char c = this->stream->readChar();
	// If next char indicates a name, parse it as the definition type
	if ( Parser::alphabet.contains(c) || Parser::alphabetUpper.contains(c) ) {
		this->stream->move(-1);
		param.type_name.alloc( this->parseName() );
	}
	else
		this->stream->move(-1);
	
	return param;
}

LinkedList<Parameter> Parser::parseParameters() {
	LinkedList<Parameter> params;
	char c;

	do {
		this->skipWhitespace();
		c = this->stream->readChar();
		if ( !Parser::delimiters.contains(c) ) {
			this->stream->move(-1);

			params += this->parseParameter();
			c = this->stream->readChar();

			if ( c == ']' )
				break;
			if ( !Parser::delimiters.contains(c) )
				throw ParseException( this, OS"Invalid character '" + c + "' found in parameter list" );
		}
	}
	while ( true );

	return params;
}

ScopeStatement Parser::parseScope() {
	ScopeStatement statement;
	
	try {
		statement.contents = this->parseStatements( true, false );
	}
	catch ( EndOfScopeException& e ) {}

	return statement;
}

SPtr<Statement> Parser::parseStatement() {
	SPtr<Statement> statement;
	char c = this->stream->readChar();

	if ( Parser::delimiters.contains(c) ) {
		this->stream->move(-1);
		throw EmptyStatementException();
	}
	else if ( c == '$' )
		statement.alloc( this->parseDefinition() );
	else if ( c == '#' )
		statement.alloc( this->parseFormat() );
	else if ( Parser::alphabet.contains(c) || Parser::alphabetUpper.contains(c) ) {
		this->stream->move(-1);
		statement.alloc( this->parseIdentity() );
	}
	//else if ( c == '@' )
	//	statement.alloc( this->parseNamespace() );
	else if ( c == '"' )
		statement.alloc( this->parseString(true) );
	else if ( c == '\'' )
		statement.alloc( this->parseString(false) );
	else if ( this->numeric.contains( c ) ) {		
		this->stream->move(-1);
		statement.alloc( this->parseNumber() );
	}
	else if ( c == '(' ) {
		statement.alloc( this->parseScope() );
	}
	else if ( c == ')' )
		throw EndOfScopeException();
	else if ( c == ']' )
		throw EndOfListException();
	else
		throw ParseException( this, OS"Character '" + c + "' does not begin a statement" );

	return statement;
}

StatementList Parser::parseStatements( bool in_scope, bool in_list ) {
	StatementList stats;
	do {
		try {
			this->skipWhitespace();
		}
		catch ( EndOfStreamException e ) {
			break;
		}
		try {
			stats += this->parseStatement();
		}
		catch ( EmptyStatementException e ) {}
		catch ( EndOfStreamException e ) {
			throw ParseException( this, "Unexpected end of file" );
		}
		catch ( EndOfScopeException& e ) {
			if ( in_scope )	break;
			// else
			throw ParseException( this, "Stray closing parentheses ')'" );
		}
		catch ( EndOfListException& e ) {
			if ( in_list )	break;
			throw ParseException( this, "Stray closing square bracket ']'" );
		}
		try {
			this->skip( Parser::whitespace );	// Skip everything except whitespaces
			// Move over delimiter
			char c = this->stream->readChar();
			if ( c == ')' ) {
				if ( in_scope )	break;
				// else
				throw ParseException( this, "Stray closing parentheses ')'" );
			}
			if ( c == ']' ) {
				if ( in_list )	break;
				throw ParseException( this, "Stray closing square bracket ']'" );
			}
			if ( !Parser::delimiters.contains(c) )
				throw ParseException( this, OS"Missing delimiter or newline after statement, found '" + c + "' instead" );
		}
		catch ( EndOfStreamException e ) {
			break;
		}
	}
	while ( true );

	return stats;
}

FormatStatement Parser::parseFormat() {
	FormatStatement statement;
	statement.format.alloc<DynamicString>();

	this->skipWhitespace();
	char c = this->stream->readChar();
	while( !Parser::whitespace.contains(c) ) {
		*statement.format += c;
		c = this->stream->readChar();
	}

	if ( *statement.format != "utf8" )	throw ParseException( this, OS"Format \"" + *statement.format + "\" is not supported" );
	
	this->skipWhitespace();
	statement.statement = this->parseStatement();
	
	return statement;
}

DynamicString Parser::parseName() {
	char c = this->stream->readChar();
	if ( !Parser::alphabet.contains(c) && !Parser::alphabetUpper.contains(c) )
		throw ParseException( this, OS"First character of a name should be in the alphabet, not '" + c + '\'' );

	DynamicString name( 1, c );

	c = this->stream->readChar();
	while ( Parser::alphabet.contains(c) || Parser::alphabetUpper.contains(c) || Parser::numeric.contains(c) ) {
		name += c;

		try {
			c = this->stream->readChar();
		}
		catch ( EndOfStreamException& e ) {
			return name;
		}
	}
	this->stream->move(-1);

	return name;
}

NumberStatement Parser::parseNumber() {
	return NumberStatement();
}

StringStatement Parser::parseString( bool double_quoted ) {
	StringStatement statement;
	statement.string.alloc<DynamicString>();

	char delimiter = double_quoted ? '"' : '\'';
	char c;
	while ( (c = this->stream->readChar() ) != delimiter ) {
		
		if ( c != '\\' )
			statement.string->append( c );
		else {
			c = this->stream->readChar();
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
	else	throw ParseException( this, (String<>)"Invalid special character in string literal: \"\\" + c + '\"' );
}

void Parser::skip( const StringBase& chars ) {
	char c;

	do	{
		c = this->stream->readChar();
		// If comment
		if ( c == ';' ) {
			// Skip everything until newline
			do { c = this->stream->readChar(); }
			while ( c != '\n' );
			c = this->stream->readChar();
		}
	}
	while ( chars.contains( c ) );

	this->stream->move(-1);
}

void Parser::skipWhitespace() {
	this->skip( Parser::whitespace );
}
