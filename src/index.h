#ifndef _ONE_INDEX_H
#define _ONE_INDEX_H

#include "definition.h"
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
		const Index* parent;
		chi::ArrayMap<Definition> definitions;

		Index( const Index* parent = 0 ) : parent(parent) {}
		Index( const Index* parent,  const chi::Map<Definition>& definitions ) : parent(parent), definitions(definitions) {}
		Index( const chi::Map<Definition>& definitions ) : parent(0), definitions(definitions) {}

		const Definition* findDefinition( const chi::StringBase& name, const Index** index = 0 ) const;
		Type findType( const chi::StringBase& name ) const;
	};
}

#endif//_ONE_INDEX_H
