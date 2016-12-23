#ifndef _ONE_DEFINITION_H
#define _ONE_DEFINITION_H

#include "file.h"
#include "parameter.h"
#include "statement.h"
#include <chi/map.h>
#include <chi/ptr.h>
#include <chi/string.h>


namespace one {

	class Definition {
	public:
		// The file & position from the statement or parameter from which this definition was constructed.
		// Used by the Composer class for generating errors.
		FilePos pos;
		// A pointer to the index that the body definition adheres to. 
		// This does not include the index of the parameters which is constructed whenever a identity statement uses this definition.
		// This index contains all global definitions that the body of this definition might use.
		//const Index* index;
		// The parameters of this definition.
		chi::ArrayMap<Parameter> params;
		// The body statement.
		chi::CSPtr<Statement> body;

		Definition() {}
		Definition( const FilePos& pos, chi::CSPtr<Statement> body, const chi::ArrayMap<Parameter> params = chi::ArrayMap<Parameter>() ) : pos(pos), params(params), body(body) {}
	};
}

#endif//_ONE_DEFINITION_H
