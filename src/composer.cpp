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
	if ( definition == 0 ) {
		ONE_ASSERT( true, "No definition %s exist in that context", name->ptr() );	
	}

	// Check if the same number of arguments are given than that there are parameters
	if ( definition->params.count() != statement->args.count() )
		throw ParseException( FilePos( *context.filename, statement->name_pos + name->length() ), "Not the same amount of arguments given as that there are parameters" );

	// Check for each parameter if the given argument is of the correct type and add them to a map
	Map<CSPtr<DefinitionStatement>> argument_index;
	for ( Size i = 0; i < definition->params.count(); i++ ) {
		Parameter param = definition->params[i];
		CSPtr<Statement> arg = statement->args[i];

		if ( param.type_name->length() > 0 ) {
			if ( !context.complies( arg, *param.type_name ) )
				throw ParseException( FilePos( *context.filename, arg->pos ), OS"Argument not of type \"" + *param.type_name + "\"." );
		}

		DefinitionStatement arg_def;
		arg_def.pos = param.name_pos;
		arg_def.name = param.name;
		arg_def.name_pos = param.name_pos;
		arg_def.body = arg;
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


bool ComposerContext::complies( const Statement* statement, const StringBase& type_name ) const {

	// Expand scopes by evaluating its first element
	if ( statement->type() == StatementType_Scope ) {
		const ScopeStatement* scope = (const ScopeStatement*)statement;

		// If the scope is empty, it can never be evaluated to some other statement
		if ( scope->contents.count() == 0 )
			return false;
		
		return this->complies( scope->contents[0], type_name );
	}

	// Built-in string literal type
	if ( statement->type() == StatementType_String )
		return type_name == "str";

	// Built-in number literal type
	if ( statement->type() == StatementType_Number )
		return type_name == "num";

	// Check if identity's evaluated statement complies with the type's definition
	if ( statement->type() == StatementType_Identity ) {
		const IdentityStatement* identity = (const IdentityStatement*)statement;
		
		CSPtr<DefinitionStatement> type_definition = this->findDefinition( type_name );
		return this->complyCheckIdentity( this, type_definition, identity );
	}

	// There is no other statement that can statisfy a type check
	return false;
}

bool ComposerContext::complyCheckDefinition( const ComposerContext& context, const DefinitionStatement* type, const DefinitionStatement* statement, const Map<CSPtr<DefinitionStatement>>& argument_index ) const {
	ComposerContext sub_context( &context );
	sub_context.index.alloc();
	sub_context.index->definitions = argument_index;

	return this->complyCheckStatement( context, type, statement->body );
}

bool ComposerContext::complyCheckFormat( const ComposerContext& context, const DefinitionStatement* type, const FormatStatement* statement ) const {
	return this->complyCheckStatement( context, type, statement->statement );
}

bool ComposerContext::complyCheckIdentity( const ComposerContext& context, const DefinitionStatement* type, const IdentityStatement* statement ) const {
	CSPtr<DefinitionStatement> definition = this->findDefinition( *statement->names.last() );

	if ( definition == type )
		return true;

	Map<CSPtr<DefinitionStatement>> argument_index;
	for ( Size i = 0; i < definition->params.count(); i++ ) {
		DefinitionStatement def;
		def.pos = definition->params[i].name_pos;
		def.name = definition->params[i].name;
		def.name_pos = def.pos;
		def.body = statement->args[0];

		argument_index.add( *definition->params[i].name, CSPtr<DefinitionStatement>::allocNew( def ) );
	}

	return this->complyCheckDefinition( context, type, definition, argument_index );
}

bool ComposerContext::complyCheckScope( const ComposerContext& context, const DefinitionStatement* type, const ScopeStatement* statement ) const {
	if ( statement->contents.size() == 0 )
		return false;

	return this->complyCheckStatement( context, type, statement->contents[0] );
}

bool ComposerContext::complyCheckStatement( const ComposerContext& context, const DefinitionStatement* type, const Statement* statement ) const {

	if ( statement->type() == StatementType_Definition )
		return type == (const DefinitionStatement*)statement;
	//if ( statement->type() == StatementType_Format )
	//	return this->complyCheckFormat( context, (const FormatStatement*)statement );
	if ( statement->type() == StatementType_Identity )
		return this->complyCheckIdentity( context, type, (const IdentityStatement*)statement );
	if ( statement->type() == StatementType_Scope )
		return this->complyCheckScope( context, type, (const ScopeStatement*)statement );
	//else
		return false;
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
