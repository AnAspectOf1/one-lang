#ifndef _ONE_INDEX_H
#define _ONE_INDEX_H

#include "statement.h"
#include <chi/map.h>
#include <chi/ptr.h>


namespace one {

	// The index maintains a library of all available definitions and their namespaces
	class Index {
	public:
		chi::Map<chi::CSPtr<DefinitionStatement>> definitions;
		//chi::Map<Index> namespaces;
	};
}

#endif//_ONE_INDEX_H
