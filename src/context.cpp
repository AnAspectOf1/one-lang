#include "context.h"

using namespace chi;
using namespace one;


CSPtr<Statement> Context::evaluateDefinition( const Type& type, CSPtr<DefinitionStatement> statement, const Map<CSPtr<DefinitionStatement>>& argument_index ) const {
	Context sub_context( this );
	sub_context.index.alloc();
	sub_context.index->definitions = argument_index;

	return sub_context.evaluateStatement( type, statement->body );
}

CSPtr<Statement> Context::evaluateIdentity( const Type& type, CSPtr<IdentityStatement> statement ) const {
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

CSPtr<Statement> Context::evaluateScope( const Type& type, CSPtr<ScopeStatement> statement ) const {
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

CSPtr<Statement> Context::evaluateStatement( const Type& type, CSPtr<Statement> statement ) const {

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

CSPtr<DefinitionStatement> Context::findDefinition( const StringBase& name ) const {
	//ONE_ASSERT( names.count() == 0, "The names in Context::findDefinition can not be empty" );
	//ONE_ASSERT( names.count() == 1, "Namespaces are not implemented yet" );
	
	const Context* context = this;

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

Type Context::findType( const StringBase& name ) const {

	if ( name == "str" )
		return Type( true );
	else if ( name == "num" )
		return Type( false );
	
	const DefinitionStatement* def = this->findDefinition( name );
	if ( def == 0 )
			throw TypeNotFoundException();
	return Type( def );
}
