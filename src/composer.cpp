#include "composer.h"

#include "exception.h"
#include "file.h"
#include "parser.h"
#include <chi/dynamic.h>

using namespace chi;
using namespace one;


Composer::Composer( WriteStream& stream, const StatementList* document, const chi::StringBase& filename ) : document(document), stream(stream), filename(filename) {}

void Composer::compose() {
	ComposerContext context;
	context.filename.alloc( this->filename );

	context.index.alloc( Indexer( this->document ).index() );

	this->composeStatements( context, *this->document );
}

void Composer::composeScope( const ComposerContext& context, const ScopeStatement* statement ) {
	ComposerContext sub_context( &context );

	// Index all definitions of our sub-context before we compose our sub-context
	sub_context.index.alloc( Indexer( &statement->contents ).index() );

	this->composeStatements( sub_context, statement->contents );
}

void Composer::composeStatement( const ComposerContext& context, const Statement* statement ) {
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

void Composer::composeStatements( const ComposerContext& context, const StatementList& statements ) {
	for ( Size i = 0; i < statements.count(); i++ ) {
		this->composeStatement( context, statements[i] );
	}
}

void Composer::composeFormat( const ComposerContext& context, const FormatStatement* statement ) {
	ComposerContext sub_context( &context );
	sub_context.string_format = statement->format;

	// TODO: If format is a string format, give warning when statement is not of type string. Do the same for number formats.
	this->composeStatement( sub_context, statement->statement );
}

void Composer::composeIdentity( const ComposerContext& context, const IdentityStatement* statement ) {	
	CSPtr<StringBase> name = statement->names.last();

	CSPtr<DefinitionStatement> definition = context.findDefinition( *name );
	if ( definition == 0 )
		throw ParseException( statement->pos, OS"Definition \"" + *name + "\" not found" );

	// Check if the same number of arguments are given than that there are parameters
	if ( definition->params.count() != statement->args.count() ) {
		throw ParseException( statement->pos, "Not the same amount of arguments given as that there are parameters" ); }

	// Check for each parameter if the given argument is of the correct type and add them to a map
	Map<CSPtr<DefinitionStatement>> argument_index;
	for ( Size i = 0; i < definition->params.count(); i++ ) {
		Parameter param = definition->params[i];
		CSPtr<Statement> arg = statement->args[i];
		CSPtr<Statement> evaluated = arg;

		if ( param.type_name != 0 ) {
			try {
				evaluated = context.evaluateStatement( Type( &context, *param.type_name ), evaluated );
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
	ComposerContext sub_context( &context );
	sub_context.index.alloc();
	sub_context.index->definitions = argument_index;

	return this->composeStatement( sub_context, definition->body );
}

void Composer::composeString( const ComposerContext& context, const StringStatement* statement ) {
	this->stream.writeString( *statement->string );
}


CSPtr<Statement> ComposerContext::evaluate( CSPtr<Statement>& statement, const Type& type ) const {

	return 0;
}

CSPtr<Statement> ComposerContext::evaluateDefinition( const Type& type, CSPtr<DefinitionStatement> statement, const Map<CSPtr<DefinitionStatement>>& argument_index ) const {
	ComposerContext sub_context( this );
	sub_context.index.alloc();
	sub_context.index->definitions = argument_index;

	return sub_context.evaluateStatement( type, statement->body );
}

CSPtr<Statement> ComposerContext::evaluateIdentity( const Type& type, CSPtr<IdentityStatement> statement ) const {
	CSPtr<DefinitionStatement> definition = this->findDefinition( *statement->names.last() );

	// If this identity turns out to be of the definition 'type', this is the identity to which we evaluate
	if ( definition == type.def )
		return statement;

	// Otherwise we expand the definition
	Map<CSPtr<DefinitionStatement>> argument_index;
	for ( Size i = 0; i < definition->params.count(); i++ ) {
		DefinitionStatement def;
		def.pos = definition->pos.move( definition->params[i].name_pos );
		def.name = definition->params[i].name;
		def.name_pos = def.pos.pos;
		def.body = statement->args[0];

		argument_index.add( *definition->params[i].name, CSPtr<DefinitionStatement>::allocNew( def ) );
	}

	return this->evaluateDefinition( type, definition, argument_index );
}

CSPtr<Statement> ComposerContext::evaluateScope( const Type& type, CSPtr<ScopeStatement> statement ) const {
	if ( statement->contents.size() == 0 )
		return 0;

	for ( Size i = 0; i < statement->contents.size(); i++ ) {
		CSPtr<Statement> content = statement->contents[i];
		// The first line in a scope that is of the following types get evaluated
		if ( content->type() == StatementType_Identity || content->type() == StatementType_Number || content->type() == StatementType_String )
			return this->evaluateStatement( type, content );
	}

	// If no line has been found of those types, we give up
	return 0;
}

CSPtr<Statement> ComposerContext::evaluateStatement( const Type& type, CSPtr<Statement> statement ) const {

	if ( statement->type() == StatementType_Identity )
		return this->evaluateIdentity( type, statement.cast<IdentityStatement>() );
	if ( statement->type() == StatementType_Scope )
		return this->evaluateScope( type, statement.cast<ScopeStatement>() );
	if ( statement->type() == StatementType_Number )
		return type.is_num() ? statement : 0;
	if ( statement->type() == StatementType_String )
		return type.is_str() ? statement : 0;
	// Else, when the statement can't be evaluated any further, the 0 pointer describes that no deeper definition could be found
	return 0;
}

CSPtr<DefinitionStatement> ComposerContext::findDefinition( const StringBase& name ) const {
	//ONE_ASSERT( names.count() == 0, "The names in ComposerContext::findDefinition can not be empty" );
	//ONE_ASSERT( names.count() == 1, "Namespaces are not implemented yet" );
	
	const ComposerContext* context = this;

	CSPtr<DefinitionStatement> definition;
	// First search all defintions in current context
	while ( context != 0 ) {
		Size i = context->index->definitions.findKey( name );
		if ( i == (Size)-1 ) // If not found, continue searching in parent context
			context = context->parent;
		else {
			definition = context->index->definitions.at(i).value;
			break;
		}
	}

	return definition;
}
