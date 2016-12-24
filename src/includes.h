#ifndef _ONE_INCLUDES_H
#define _ONE_INCLUDES_H

#include <chi/array.h>
#include <chi/string.h>


namespace one {

	class Includes {
	public:
		static chi::Array<chi::String<>> searchDirs();
	};
}

#endif//_ONE_INCLUDES_H
