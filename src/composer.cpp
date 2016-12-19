#include "composer.h"

#include "exception.h"
#include <chi/dynamic.h>

using namespace chi;
using namespace one;


Composer::Composer( WriteStream& stream, const StatementList* document ) : stream(stream), document(document) {}

void Composer::compose() {
	ComposerContext context;

	context.index = Indexer( this->document ).index();

	this->composeStatements( context, *this->document );
}

void Composer::composeScope( const ComposerContext& context, const ScopeStatement* statement ) {
	ComposerContext sub_context( &context );

	// Index all definitions of our sub-context before we compose our sub-context
	sub_context.index = Indexer( &statement->contents ).index();

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

	CSPtr<Statement> definition = context.findDefinition( *name );
	if ( !definition.allocated() ) {
		ONE_ASSERT( true, "No definition %s exist in that context", name->ptr() );	
	}

	this->composeStatement( context, definition );
}

void Composer::composeString( const ComposerContext& context, const StringStatement* statement ) {
	this->stream.writeString( *statement->string );
}

CSPtr<Statement> ComposerContext::findDefinition( const StringBase& name ) const {
	//ONE_ASSERT( names.count() == 0, "The names in ComposerContext::findDefinition can not be empty" );
	//ONE_ASSERT( names.count() == 1, "Namespaces are not implemented yet" );
	
	const ComposerContext* context = this;

	CSPtr<Statement> definition;
	// First search all defintions in current context
	while ( context != 0 ) {
		Size i = context->index.definitions.findKey( name );
		if ( i == (Size)-1 ) // If not found, continue searching in parent context
			context = context->parent;
		else {
			definition = context->index.definitions.at(i).value;
			break;
		}
	}

	return definition;
}
