#include "indexer.h"

using namespace chi;
using namespace one;


Index Indexer::index() const {
	Index index;

	for ( Size i = 0; i < this->document.size(); i++ ) {
		SPtr<Statement> statement = this->document[i];

		if ( statement->type() == StatementType_Definition ) {
			DefinitionStatement* definition = statement;
			index.definitions.add( definition->name, definition->body );
		}
		// TODO: Implement namespaces
	}

	return index;
}
