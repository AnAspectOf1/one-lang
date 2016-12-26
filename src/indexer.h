#ifndef _ONE_DEFINITION_INDEX_H
#define _ONE_DEFINITION_INDEX_H

#include "index.h"
#include "statement.h"
#include <qi/map.h>


namespace one {

	class Indexer {
		const StatementList* document;

		void countDefinitions( qi::Size* defs, qi::Size* labels ) const;

	public:
		Indexer( const StatementList* document );

		// Creates an Index from all the definitions from DefinitionSt
		Index index( const Index* parent = 0 ) const;
	};

}

#endif//_ONE_DEFINITION_INDEX_H
