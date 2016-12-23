#include "composer.h"

#include "exception.h"
#include "file.h"
#include "parser.h"
#include <chi/dynamic.h>

using namespace chi;
using namespace one;


Composer::Composer( WriteStream& stream, const StatementList* document, const chi::StringBase& filename ) : document(document), stream(stream), filename(filename) {}

void Composer::compose() {
	Context context;

	context.filename.alloc( this->filename );

	context.index.alloc( Indexer( this->document ).index() );

	this->composeStatements( context, *this->document );
}

void Composer::composeScope( const Context& context, const ScopeStatement* statement ) {
	Context sub_context( &context );

	// Index all definitions of our sub-context before we compose our sub-context
	sub_context.index.alloc( Indexer( &statement->contents ).index() );

	this->composeStatements( sub_context, statement->contents );
}

void Composer::composeStatement( const Context& context, const Statement* statement ) {
	if ( statement->type() == StatementType_Definition )
		{}	// Do nothing
	else if ( statement->type() == StatementType_Format )
		this->composeFormat( context, (const FormatStatement*)statement );
	else if ( statement->type() == StatementType_Identity )
		this->composeIdentity( context, (const IdentityStatement*)statement );
	else if ( statement->type() == StatementType_Scope )
		this->composeScope( context, (const ScopeStatement*)statement );
	else if ( statement->type() == StatementType_String )
		this->composeString( context, (const StringStatement*)statement );
	else
		ONE_ASSERT( true, "Unrecognized statement type encountered: %d", statement->type() );
}

void Composer::composeStatements( const Context& context, const StatementList& statements ) {
	for ( Size i = 0; i < statements.count(); i++ ) {
		this->composeStatement( context, statements[i] );
	}
}

void Composer::composeFormat( const Context& context, const FormatStatement* statement ) {
	Context sub_context( &context );
	sub_context.string_format = statement->format;

	// TODO: If format is a string format, give warning when statement is not of type string. Do the same for number formats.
	this->composeStatement( sub_context, statement->statement );
}

void Composer::composeIdentity( const Context& context, const IdentityStatement* statement ) {	
	CSPtr<StringBase> name = statement->names.last();
	printf("Identity: %s\n", name->ptr());

	CSPtr<DefinitionStatement> definition = context.findDefinition( *name );
	if ( definition == 0 )
		throw ParseException( statement->pos, OS"Definition \"" + *name + "\" not found" );

	// Check if the same number of arguments are given than that there are parameters
	if ( definition->params.count() != statement->args.count() ) { printf( "%d %d\n", definition->params.count(), statement->args.count() );
		throw ParseException( statement->pos, "Not the same amount of arguments given as that there are parameters" ); }

	// Check for each parameter if the given argument is of the correct type and add them to a map
	Map<CSPtr<DefinitionStatement>> argument_index;
	for ( Size i = 0; i < definition->params.count(); i++ ) {
		Parameter param = definition->params[i];
		CSPtr<Statement> arg = statement->args[i];
		CSPtr<Statement> evaluated = arg;

		if ( param.type_name != 0 ) {
			try {
				evaluated = context.evaluateStatement( context.findType( *param.type_name ), evaluated );
			}
			catch ( TypeNotFoundException& ) {
				throw ParseException( arg->pos.move( param.type_name_pos ), OS"No definition was found for type name \"" + *param.type_name + '"' );
			}
			if ( evaluated == 0 )
				throw ParseException( arg->pos, OS"Argument not of type \"" + *param.type_name + '\"' );
		}

		DefinitionStatement arg_def;
		arg_def.pos = definition->pos.move( param.name_pos );
		arg_def.name = param.name;
		arg_def.name_pos = param.name_pos;
		arg_def.body = evaluated;
		argument_index.add( *param.name, CSPtr<DefinitionStatement>::allocNew( arg_def ) );
	}

	// Compose with the argument list as the new index
	Context sub_context( &context );
	sub_context.index.alloc();
	sub_context.index->definitions = argument_index;

	return this->composeStatement( sub_context, definition->body );
}

void Composer::composeString( const Context& context, const StringStatement* statement ) {
	this->stream.writeString( *statement->string );
}
