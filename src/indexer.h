#ifndef _ONE_DEFINITION_INDEX_H
#define _ONE_DEFINITION_INDEX_H

#include "index.h"
#include "statement.h"
#include <chi/map.h>


namespace one {

	class Indexer {
		const StatementList* document;

	public:
		Indexer( const StatementList* document );

		Index index() const;
	};

}

#endif//_ONE_DEFINITION_INDEX_H
