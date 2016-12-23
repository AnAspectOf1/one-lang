#include "context.h"

using namespace chi;
using namespace one;


CSPtr<Statement> Context::evaluateDefinition( const Type& type, CSPtr<Statement> body, const Index& argument_index ) const {
	Context sub_context( this );
	sub_context.index.alloc( argument_index );

	return sub_context.evaluateStatement( type, body );
}

CSPtr<Statement> Context::evaluateIdentity( const Type& type, CSPtr<IdentityStatement> statement ) const {
	const Index* definition_index;
	const Definition* definition = this->index->findDefinition( *statement->names.last(), &definition_index );

	// If this identity turns out to be of the definition 'type', this is the identity to which we evaluate
	if ( definition == type.def )
		return statement;

	// Otherwise we expand the definition
	Index argument_index( definition_index );
	argument_index.definitions.grow( definition->params.count() );
	for ( Size i = 0; i < definition->params.count(); i++ ) {

		CSPtr<Statement> eval_stat = this->evaluateStatement( definition->params.at(i).value.type, statement->args[i] );

		Definition def(
			definition->pos.move( definition->params.at(i).value.name_pos ),
			eval_stat
		);

		argument_index.definitions.at(i) = MapEntry<Definition>( definition->params.at(i).key, def );
	}

	return this->evaluateDefinition( type, definition->body, argument_index );
}

CSPtr<Statement> Context::evaluateScope( const Type& type, CSPtr<ScopeStatement> statement ) const {
	if ( statement->contents.size() == 0 )
		return 0;

	for ( Size i = 0; i < statement->contents.count(); i++ ) {
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
