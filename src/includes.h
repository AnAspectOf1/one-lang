#ifndef _ONE_INCLUDES_H
#define _ONE_INCLUDES_H

#include <qi/array.h>
#include <qi/string.h>


namespace one {

	class Includes {
	public:
		static qi::Array<qi::String<>> searchDirs();
	};
}

#endif//_ONE_INCLUDES_H
