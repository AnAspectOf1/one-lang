#ifndef _ONE_DEFINITION_INDEX_H
#define _ONE_DEFINITION_INDEX_H

#include "statement.h"
#include <chi/map.h>


namespace one {

	// The index maintains a library of all available definitions and their namespaces
	class Index {
	public:
		chi::Map<chi::CSPtr<DefinitionStatement>> definitions;
		//chi::Map<Index> namespaces;
	};

	class Indexer {
		const StatementList* document;

	public:
		Indexer( const StatementList* document );

		Index index() const;
	};

}

#endif//_ONE_DEFINITION_INDEX_H
