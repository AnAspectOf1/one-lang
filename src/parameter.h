#ifndef _ONE_PARAMETER_H
#define _ONE_PARAMETER_H

#include "type.h"


namespace one {

	class Parameter {
	public:
		bool has_type;
		Type type;
		unsigned int name_pos, type_pos;

		Parameter() : has_type(false) {}
		Parameter( unsigned int name_pos ) : has_type(false), name_pos(name_pos) {}
		Parameter( const Type& type, unsigned int name_pos, unsigned int type_pos ) : has_type(true), type(type), name_pos(name_pos), type_pos(type_pos) {}
	};
}

#endif//_ONE_PARAMETER_H
