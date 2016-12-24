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
	sub_context.index.alloc( Indexer( &statement->contents ).index( context.index ) );

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
	//printf("Composing ident. %s\n", name->ptr());

	const Index* definition_index;
	const Definition* definition = context.index->findDefinition( *name, &definition_index );
	if ( definition == 0 )
		throw ParseException( statement->pos, OS"Definition \"" + *name + "\" not found" );

	//if ( definition != 0 )
	//	printf("Definition %s found in index %p\n", definition->name->ptr(), definition_index);

	// Check if the same number of arguments are given than that there are parameters
	if ( definition->params.count() != statement->args.count() ) {
		// TODO: Create some nice chi library functions
		char param_count[4], arg_count[4];
		sprintf( param_count, "%d", definition->params.count() );
		sprintf( arg_count, "%d", statement->args.count() );

		throw ParseException( statement->pos, OS"Definition \"" + name->ptr() + "\" takes " + param_count + " parameter" + (definition->params.count() != 1 ? "s" : "") + " but " + arg_count + " argument" + (statement->args.count() != 1 ? "s are" : " is") + " given" );
	}

	// Compose with the argument list as the new index
	Context sub_context( &context );
	sub_context.index.alloc( Index( definition_index, context.index, ArrayMap<SPtr<Definition>>( definition->params.count() ) ) );
	Map<SPtr<Definition>>& argument_index = sub_context.index->arguments;

	// Check for each parameter if the given argument is of the correct type and add them to a map
	for ( Size i = 0; i < definition->params.count(); i++ ) {
		Parameter param = definition->params.at(i).value;
		CSPtr<Statement> arg = statement->args[i];
		CSPtr<Statement> evaluated = arg;

		if ( param.has_type ) {
			evaluated = context.evaluateStatement( param.type, evaluated );
			if ( evaluated == 0 )			
				throw ParseException( arg->pos, OS"Argument not of requested type \"" + *param.type_name + '"' );
		}

		// Add the argument to the argument index as a definition that has no parameters
		SPtr<Definition> arg_def; arg_def.alloc( Definition(
			definition->pos.move( param.name_pos ),
			evaluated
		) );
		arg_def->name.alloc( definition->params.at(i).key );
		argument_index.at(i) = MapEntry<SPtr<Definition>>( definition->params.at(i).key, arg_def );
	}

	//printf("Composing identity \"%s\" with new index %p.\n", name->ptr(), sub_context.index.ptr());
	// Compose the definition's body with the index from where is was found
	return this->composeStatement( sub_context, definition->body );
}

void Composer::composeString( const Context& context, const StringStatement* statement ) {
	this->stream.writeString( *statement->string );
}
