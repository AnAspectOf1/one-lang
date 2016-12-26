#include "context.h"

using namespace qi;
using namespace one;


CSPtr<Statement> Context::evaluateDefinition( const Type& type, CSPtr<Statement> body, const Index& argument_index ) const {
	Context sub_context( this );
	sub_context.index.alloc( argument_index );

	return sub_context.evaluateStatement( type, body );
}

CSPtr<Statement> Context::evaluateIdentity( const Type& type, CSPtr<IdentityStatement> statement ) const {
	const Index* definition_index;
	CSPtr<Definition> definition = this->index->findDefinition( *statement->names.last(), &definition_index );
	//printf( "Evaluating ident. def. \"%s\".\n", definition->name->ptr() );


	// If this identity turns out to be of the definition 'type', this is the statement to which we evaluate
	if ( type.is_def() && definition == type.def )
		return statement;

	// Otherwise we expand the definition
	Index argument_index( definition_index, this->index );
	argument_index.arguments.grow( definition->params.count() );
	for ( Size i = 0; i < definition->params.count(); i++ ) {

		CSPtr<Statement> eval_stat = this->evaluateStatement( definition->params.at(i).value.type, statement->args[i] );
		if ( eval_stat == 0 )
			return 0;

		SPtr<Definition> def; def.alloc( Definition(
			definition->pos.move( definition->params.at(i).value.name_pos ),
			eval_stat
		) );

		argument_index.arguments.at(i) = MapEntry<SPtr<Definition>>( definition->params.at(i).key, def );
	}

	return this->evaluateDefinition( type, definition->body, argument_index );
}

CSPtr<Statement> Context::evaluateLabel( const Type& type, CSPtr<LabelStatement> statement ) const {
	 return this->evaluateStatement( type, statement->body );
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
	if ( statement->type() == StatementType_Label )
		return this->evaluateLabel( type, statement.cast<LabelStatement>() );
	if ( statement->type() == StatementType_Scope )
		return this->evaluateScope( type, statement.cast<ScopeStatement>() );
	if ( statement->type() == StatementType_Number )
		return type.is_num() ? statement : 0;
	if ( statement->type() == StatementType_String )
		return type.is_str() ? statement : 0;
	// Else, when the statement can't be evaluated any further, the 0 pointer describes that no deeper definition could be found
	return 0;
}
