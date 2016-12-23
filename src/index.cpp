#include "index.h"

using namespace chi;
using namespace one;


const Definition* Index::findDefinition( const StringBase& name, const Index** index ) const {
	//ONE_ASSERT( names.count() == 0, "The names in Context::findDefinition can not be empty" );
	//ONE_ASSERT( names.count() == 1, "Namespaces are not implemented yet" );
	
	const Index* context = this;

	const Definition* definition;
	// First search all defintions in current context
	while ( context != 0 ) {

		Size i = context->definitions.findKey( name );
		if ( i == (Size)-1 ) // If not found, continue searching in parent context
			context = context->parent;
		else {
			definition = &context->definitions.at(i).value;
			if ( index != 0 )
				*index = context;
			break;
		}
	}

	return definition;
}

Type Index::findType( const StringBase& name ) const {

	if ( name == "str" )
		return Type( true );
	else if ( name == "num" )
		return Type( false );
	
	const Definition* def = this->findDefinition( name );
	if ( def == 0 )
			throw TypeNotFoundException();
	return Type( def );
}
