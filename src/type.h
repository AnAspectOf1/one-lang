#ifndef _ONE_TYPE_H
#define _ONE_TYPE_H

#include "exception.h"
#include "statement.h"
#include <chi/string.h>


namespace one {

	class TypeNotFoundException : public Exception {};
	
	class Type {
	protected:
		bool _is_str;

	public:
		const DefinitionStatement* def;

		Type( const DefinitionStatement* def );
		Type( bool is_str );

		bool is_def() const;
		bool is_num() const;
		bool is_str() const;
	};
}

#endif//_ONE_TYPE_H
