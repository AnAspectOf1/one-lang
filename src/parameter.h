#ifndef _ONE_PARAMETER_H
#define _ONE_PARAMETER_H

#include "type.h"
#include <qi/int.h>
#include <qi/ptr.h>
#include <qi/string.h>


namespace one {

	class Parameter {
	public:
		bool has_type;
		Type type;
		qi::CSPtr<qi::StringBase> type_name;
		qi::Size name_pos, type_pos;

		Parameter() : has_type(false) {}
		Parameter( qi::Size name_pos ) : has_type(false), name_pos(name_pos) {}
		Parameter( const Type& type, qi::CSPtr<qi::StringBase> type_name, qi::Size name_pos, qi::Size type_pos ) : has_type(true), type(type), type_name(type_name), name_pos(name_pos), type_pos(type_pos) {}
	};
}

#endif//_ONE_PARAMETER_H
