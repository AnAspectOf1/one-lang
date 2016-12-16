#ifndef _ONE_DEFINITION_INDEX_H
#define _ONE_DEFINITION_INDEX_H

#include "statement.h"

namespace one {

	// The index maintains a library of all available definitions and their namespaces
	class Index {
	protected:
		chi::Map<chi::SPtr<Statement>> definitions;
		chi::Map<Index>	namespaces;

	public:
		 
	};

	class Indexer {
		StatementList document;

	public:
		Indexer( StatementList document ) : document(document) {}

		Index index() const;
	};

}

#endif//_ONE_DEFINITION_INDEX_H
