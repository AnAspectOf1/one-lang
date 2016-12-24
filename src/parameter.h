#ifndef _ONE_PARAMETER_H
#define _ONE_PARAMETER_H

#include "type.h"
#include <chi/int.h>
#include <chi/ptr.h>
#include <chi/string.h>


namespace one {

	class Parameter {
	public:
		bool has_type;
		Type type;
		chi::CSPtr<chi::StringBase> type_name;
		chi::Size name_pos, type_pos;

		Parameter() : has_type(false) {}
		Parameter( chi::Size name_pos ) : has_type(false), name_pos(name_pos) {}
		Parameter( const Type& type, chi::CSPtr<chi::StringBase> type_name, chi::Size name_pos, chi::Size type_pos ) : has_type(true), type(type), type_name(type_name), name_pos(name_pos), type_pos(type_pos) {}
	};
}

#endif//_ONE_PARAMETER_H
