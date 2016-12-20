#ifndef _ONE_FILE_H
#define _ONE_FILE_H

#include <chi/string.h>


namespace one {

	struct FilePos {
		chi::String<> filename;
		unsigned int pos;

		FilePos( const chi::StringBase& filename, unsigned int pos ) : filename(filename), pos(pos) {}
	};
}

#endif//_ONE_FILE_H
