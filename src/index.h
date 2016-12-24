#ifndef _ONE_INDEX_H
#define _ONE_INDEX_H

#include "definition.h"
#include "exception.h"
#include "type.h"
#include <chi/map.h>
#include <chi/ptr.h>
#include <chi/string.h>


namespace one {

	class TypeNotFoundException : public Exception {};


	class Indexer;


	// The index is a library of all available definitions, that also points towards a parent index to which more global definitions can be found
	class Index {
	protected:

	public:
		// The parent index refers to the index that holds more global definitions.
		// The caller index refers to the index that holds the definitions of the calling identity.
		const Index* parent,* caller;
		chi::ArrayMap<chi::SPtr<Definition>> definitions, arguments;
		// The index maintains a map to _pointers_ of definitions because the location of the definition is used to compare definitions
		// If this was simply an ArrayMap<Definition>, it would be tricky because when the map is copied the definitions are in a new memory location.
		Index( const Index* parent = 0, const Index* caller = 0 ) : parent(parent), caller(caller) {}
		Index( const Index* parent,  const chi::Map<chi::SPtr<Definition>>& definitions ) : parent(parent), caller(0), definitions(definitions) {}
		Index( const Index* parent, const Index* caller, const chi::Map<chi::SPtr<Definition>>& arguments ) : parent(parent), caller(caller), arguments(arguments) {}
		Index( const chi::Map<chi::SPtr<Definition>>& definitions ) : parent(0), caller(0), definitions(definitions) {}

		// Returns a shared pointer to the definition that is found from this index.
		// If body_index is provided, the pointer that is pointed to by this parameter is set to point towards the Index from which the definition's body can be composed.
		chi::CSPtr<Definition> findDefinition( const chi::StringBase& name, const Index** body_index = 0 ) const;
		Type findType( const chi::StringBase& name ) const;
	};
}

#endif//_ONE_INDEX_H
