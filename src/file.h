#ifndef _ONE_FILE_H
#define _ONE_FILE_H

#include <chi/ptr.h>
#include <chi/stream.h>
#include <chi/string.h>


namespace one {

	struct FilePos {
		chi::ReadSeekStream* file;
		chi::CSPtr<chi::StringBase> filename;
		chi::Size pos;

		FilePos() : file(0), pos(0) {}

		FilePos( chi::ReadSeekStream* file, const chi::CSPtr<chi::StringBase>& filename, chi::Size pos ) : file(file), filename(filename), pos(pos) {}

		FilePos move( unsigned int new_pos ) const {
			FilePos file_pos = *this;
			file_pos.pos = new_pos;
			return file_pos;
		}
	};
}

#endif//_ONE_FILE_H
