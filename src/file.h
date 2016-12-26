#ifndef _ONE_FILE_H
#define _ONE_FILE_H

#include <qi/ptr.h>
#include <qi/stream.h>
#include <qi/string.h>


namespace one {

	struct FilePos {
		qi::ReadSeekStream* file;
		qi::CSPtr<qi::StringBase> filename;
		qi::Size pos;

		FilePos() : file(0), pos(0) {}

		FilePos( qi::ReadSeekStream* file, const qi::CSPtr<qi::StringBase>& filename, qi::Size pos ) : file(file), filename(filename), pos(pos) {}

		FilePos move( unsigned int new_pos ) const {
			FilePos file_pos = *this;
			file_pos.pos = new_pos;
			return file_pos;
		}
	};
}

#endif//_ONE_FILE_H
