#include "composer.h"

#include "exception.h"

using namespace chi;
using namespace one;


Composer::Composer( const StatementList& document ) : document(document) {}

DynamicBuffer Composer::compose() {
	ComposerContext context = { 0, "utf8", "uint8" };

	return this->composeStatements( context, this->document );
}

DynamicBuffer Composer::composeContext( const ComposerContext& context, const ContextStatement* statement ) {
	ComposerContext sub_context;
	sub_context.parent = &context;
	sub_context.string_format = context.string_format;
	sub_context.number_format = context.number_format;
	// Index all definitions of our sub-context before we compose our sub-context
	sub_context.index = Indexer( statement.context ).index();
	
	return this->composeStatements( sub_context, statement.context );
}

DynamicBuffer Composer::composeStatement( const ComposerContext& context, const Statement* statement ) {
	if ( statement->type() == StatementType_String )
		return this->composeString( context.string_format, (const StringStatement*)statement );
	if ( statement->type() == StatementType_Format )
		return this->composeFormat( context, (const FormatStatement*)statement );
	
	ONE_ASSERT( true, "Unrecognized statement type encountered: %d", statement->type() );
	return DynamicBuffer();
}

DynamicBuffer Composer::composeStatements( const ComposerContext& context, const StatementList& statements ) {
	DynamicBuffer buffer;

	for ( Size i = 0; i < statements.count(); i++ ) {
		SPtr<Statement> statement = statements[i];
		buffer += this->composeStatement( context, statement );
	}

	return buffer;
}

DynamicBuffer Composer::composeFormat( const ComposerContext& context, const FormatStatement* statement ) {
	ComposerContext sub_context = context;
	sub_context.parent = &context;
	sub_context.string_format = *statement->format;

	// TODO: If format is a string format, give warning when statement is not of type string. Do the same for number formats.
	return this->composeStatement( sub_context, statement->statement );
}

DynamicBuffer Composer::composeIdentity( const ComposerContext& context, const IdentityStatement* statement ) {
	SPtr<Statement> definition = context.index.findDefinition( statement->name );
	return this->composeStatement( definition );
}

DynamicBuffer Composer::composeString( const StringBase& format, const StringStatement* statement ) {
	// TODO: Implement other format capabilities.
	if ( format != "utf8" )	throw UnsupportedFormatException( format );

	DynamicBuffer buffer( statement->string->length() );
	
	for ( Size i = 0; i < buffer.count(); i++ ) {
		buffer[i] = (*statement->string)[i];
	}

	return buffer;
}


SPtr<Statement> ComposerContext::findDefinition( const StringBase& name ) {
	ComposerContext* context = this;

	while ( context != 0 ) {
		Size i = context->index.findKey( name );
		if ( i != (Size)-1 )
			return context->index.at(i).value;

		context = context.parent;
	}

	return 0;
}
