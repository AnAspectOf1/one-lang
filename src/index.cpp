#include "index.h"

using namespace chi;
using namespace one;


CSPtr<Definition> Index::findDefinition( const StringBase& name, const Index** index ) const {
	//ONE_ASSERT( names.count() == 0, "The names in Context::findDefinition can not be empty" );
	//ONE_ASSERT( names.count() == 1, "Namespaces are not implemented yet" );
	
	CSPtr<Definition> definition;
	const Index* context = this;

	
	/*while ( context != 0 ) {
		printf("C:\t%p\n", context);
		for ( Size j = 0; j < context->definitions.count(); j++ ) {
			printf("D:\t%s\n", context->definitions.at(j).key.ptr());
		}

		context = context->parent;
	}
	context = this;*/

	while ( context != 0 ) {

		// First, check if it is an argument
		Size i = context->arguments.findKey( name );
		if ( i != (Size)-1 ) {
			definition = context->arguments.at(i).value;
			if ( index != 0 )	*index = context->caller;
			break;
		}

		// Then check if it is any other definition
		i = context->definitions.findKey( name );
		if ( i == (Size)-1 ) // If not found, continue searching in parent context
			context = context->parent;
		else {
			definition = context->definitions.at(i).value;
			if ( index != 0 )	*index = context;
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
	
	CSPtr<Definition> def = this->findDefinition( name );
	if ( def == 0 )
			throw TypeNotFoundException();
	return Type( def );
}
