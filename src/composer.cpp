#include "composer.h"

#include "exception.h"

using namespace chi;
using namespace one;


Composer::Composer( chi::Linked& document ) : document(document) {}

DynamicBuffer Composer::compose( Linked& statements ) {
	DynamicBuffer buffer;

	Link<Statement*>* statement = (Link<Statement*>*)statements.first();

	do {
		buffer += this->composeStatement( **statement );
		statement = (Link<Statement*>*)statement->next;
	}
	while ( statement != 0 );

	return buffer;
}

DynamicBuffer Composer::composeDocument() {
	return this->compose( this->document );
}

DynamicBuffer Composer::composeStatement( Statement* statement ) {
	if ( statement->type() == StatementType_String )
		return this->composeString( (StringStatement*)statement );
	
	ONE_ASSERT( true, "Unrecognized statement type encountered: %d", statement->type() );
	return DynamicBuffer();
}

Buffer<> Composer::composeString( StringStatement* statement ) {
	Buffer<> buffer( statement->string->length() );
	
	for ( Size i = 0; i < buffer.count(); i++ ) {
		buffer[i] = (*statement->string)[i];
	}

	return buffer;
}
