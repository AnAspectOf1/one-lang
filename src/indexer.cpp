#include "indexer.h"

using namespace chi;
using namespace one;


Indexer::Indexer( const StatementList* stats ) : document(stats) {}

Index Indexer::index() const {
	Index index;

	for ( Size i = 0; i < this->document->count(); i++ ) {
		CSPtr<Statement> statement = this->document->at(i);

		if ( statement->type() == StatementType_Definition ) {
			const Statement* test = statement;
			const DefinitionStatement* definition = static_cast<const DefinitionStatement*>(test);
			index.definitions.add( *definition->name, definition->body );
		}
		// TODO: Implement namespaces
	}

	return index;
}
