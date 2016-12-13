#ifndef _ONE_TRACKER_STREAM_H
#define _ONE_TRACKER_STREAM_H

#include "stream.h"
#include <chi/int.h>

namespace one {

	class TrackerStream : public ReadStream {
	private:
		void _countNewlines( const chi::Buffer<> buffer ) {
			for ( chi::Size i = 0; i < buffer.count(); i++ ) {
				if ( buffer[i] == '\n' ) {
					this->newlines_read++;
					this->cur_line_read = 0;
				}
				else
					this->cur_line_read++;
			}
		}

	protected:
		ReadStream* stream;
		unsigned int bytes_read;
		unsigned int newlines_read;
		unsigned int cur_line_read;

	public:
		TrackerStream( ReadStream* stream ) : stream(stream), bytes_read(0), newlines_read(0), cur_line_read(0) {}

		unsigned int bytesRead() const	{ return this->bytes_read; }
		unsigned int newlinesRead() const	{ return this->newlines_read; }
		unsigned int curLineRead() const	{ return this->cur_line_read; }

		chi::Buffer<> read( chi::Size size ) {
			chi::Buffer<> buffer = this->stream->read( size );
			this->bytes_read += size;
			this->_countNewlines( buffer );
			return buffer;
		}

		chi::Byte readByte() {
			chi::Byte byte = this->stream->readByte();
			this->bytes_read++;
			if ( byte == '\n' ) {
				this->newlines_read++;
				this->cur_line_read = 0;
			}
			else
				this->cur_line_read++;
			return byte;
		}

		void close() {
			this->stream->close();
		}
	};
}

#endif//_ONE_TRACKER_STREAM_H
